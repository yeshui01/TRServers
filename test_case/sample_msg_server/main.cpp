#include <iostream>
#include <string>
#include <cstdint>

#include "tr_timer/time_wheel.h"
#include "log_module.h"
#include "net_common.h"
#include "net_socket.h"
#include "cycle_buffer.h"
#include "obj_pool.h"
#include "singleton.h"
#include "net_connection.h"
#include "base_server.h"
#include "net_epoll.h"
#include "log_module.h"
#include "msg_dispatcher.h"
#include "msg_handler.h"
#include "msg_queue.h"
#include "net_message.h"
#include "common_define.h"
#include <string>

class MsgConnection : public TConnection
{
public:
	MsgConnection();
	~MsgConnection();
	void AfterReadData(int32_t read_size) override;
	void OnClose() override;
};

// #define g_ConnectPool TSingleton<TObjPool<EchoConnection> >::Instance()

MsgConnection::MsgConnection() : TConnection()
{
}
MsgConnection::~MsgConnection()
{
}
void MsgConnection::AfterReadData(int32_t read_size)
{
	TDEBUG("msg connection afterreaddata, read_size:" << read_size
		<< ", recv_buffer_rest_space:" << GetRecvBuffer().GetRestSpace()
		<< ", data_size:" << GetRecvBuffer().PeekDataSize()
		<< ", read_index:" << GetRecvBuffer().GetReadIndex()
		<< ", write_index:" << GetRecvBuffer().GetWriteIndex());
	// 读取消息到消息队列
	NetMsgHead msg_head;
	if (GetRecvBuffer().PeekDataSize() < msg_head.Size())
		return;
	// peek 数据包头
	char buffer_head[216] = "";
	int32_t real_read_size = GetRecvBuffer().ReadData(buffer_head, msg_head.Size(), true);
	if (real_read_size < msg_head.Size())
		return;

	if (!msg_head.UnSerialize(buffer_head, sizeof(buffer_head)))
	{
		TERROR("msg head unserialize msg failed, maybe logic error!!!");
		return;
	}
	if (msg_head.CalcPacketSize() > GetRecvBuffer().PeekDataSize())
	{
		// 数据还未接受完整
		return;
	}
	int32_t packet_size = msg_head.CalcPacketSize();
	// 读取数据包
	TDEBUG("fetch package, read_index:" << GetRecvBuffer().GetReadIndex()
		<< ", write_index:" << GetRecvBuffer().GetWriteIndex());
	
    static char * packet_buffer = nullptr;
    static int32_t packet_buffer_size = 0;
    if (!packet_buffer)
    {
        // 初始默认10k
        packet_buffer = new char [10240];
        packet_buffer_size = 10240;
    }
    if (packet_buffer)
    {
        if (packet_buffer_size < packet_size)
        {
            TERROR("packet body maybe too large, please check!!!!");
            // 重新分配更大的内存
            SAFE_DELETE_PTR(packet_buffer);
            packet_buffer = new char [packet_size];
            packet_buffer_size = packet_size;
        }
    }
    
	GetRecvBuffer().ReadData(packet_buffer, packet_size);
	NetMessage * message_pt = new NetMessage();
	if (message_pt->UnSerialize(packet_buffer, packet_size))
	{
		message_pt->SetConnection(this);
		g_MsgQueue.AddMsg(message_pt);
		TDEBUG("add new msg to queue, head_size:" << msg_head.Size() << ",msg_class:" << message_pt->GetMsgClass()
			<< ", msg_type:" << message_pt->GetMsgType()
			<< ", req_no:" << message_pt->GetReqNo()
			<< ", rep_no:" << message_pt->GetRepNo()
			<< ", confirm:" << message_pt->GetConfirm()
			<< ", content_size:" << msg_head.ContentSize()
			<< ", content:" << message_pt->GetContent());
	}
	else
	{
		TERROR("msg UnSerialize failed");
		SAFE_DELETE_PTR(message_pt)
	}
	TDEBUG("after fetch package, read_index:" << GetRecvBuffer().GetReadIndex()
		<< ", write_index:" << GetRecvBuffer().GetWriteIndex());
}

void MsgConnection::OnClose()
{
	TConnection::OnClose();
	TDEBUG("MsgConnection OnClose");
}

// server
class MsgServer : public TBaseServer
{
public:
	MsgServer();
	~MsgServer();
	virtual bool Init()
	{
		return true;
	}
};

MsgServer::MsgServer()
{
	TDEBUG("MsgServer construct")
}

MsgServer::~MsgServer()
{
	TDEBUG("MsgServer destruct")
}

// handler
class SampleMsgHandler : public IMessageHandler
{
public:
	SampleMsgHandler();
	virtual ~SampleMsgHandler();

	// 绑定消息处理接口
	virtual void BindMsgHandle();

protected:
	EMsgHandleResult MsgRepTest(TConnection * session_pt, const NetMessage * message_pt);
};

SampleMsgHandler::SampleMsgHandler()
{

}

SampleMsgHandler::~SampleMsgHandler()
{
	
}

// 绑定消息处理接口
void SampleMsgHandler::BindMsgHandle()
{
	// msg_handlers_.insert(std::make_pair(1, std::bind(&SampleMsgHandler::MsgRepTest, this, std::placeholders::_1, std::placeholders::_2)));
	MSG_BIND_HANDLER(1, SampleMsgHandler, MsgRepTest);
}

EMsgHandleResult SampleMsgHandler::MsgRepTest(TConnection * session_pt, const NetMessage * message_pt)
{
	TDEBUG("MsgRepTest");
	rep_content_ = "test rep msg";
	return EMsgHandleResult::E_MSG_HANDLE_RETURN_CONTENT;
}

// test
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		TERROR("argc num error!!");
		return 0;
	}
	int32_t port = std::stoi(std::string(argv[1]));
 	TDEBUG("hello msg server");
	MsgServer msg_server;
	msg_server.InitConnectionPool<MsgConnection>(1);
	msg_server.Bind("127.0.0.1", port);
	TDEBUG("bind addr, port:" << port);

	// 注册一个消息处理
	// SampleMsgHandler * msg_handler = new SampleMsgHandler();
	// g_MsgDispatcher.RegisterMsgHandler(1, msg_handler);
	g_MsgDispatcher.RegisterMsgHandlerEx<SampleMsgHandler>(1);
	msg_server.Listen();
	msg_server.AddLoopRun([](time_t cur_mtime)
	{
		g_MsgDispatcher.DispatchMessage();
	});
	msg_server.RunService();
	// delete msg_handler;
	return 0;
}
