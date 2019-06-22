/*=======================================================
# Author       : mknight
# Last modified: 2019-06-01 12:18
# Email        : 824338670@qq.com
# Filename     : server_session.cpp
# Description  : 
# =======================================================*/
#include "server_session.h"
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
ServerSession::ServerSession() : TConnection()
{

}

ServerSession::~ServerSession()
{
}

void ServerSession::AfterReadData(int32_t read_size)
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
	char buffer_head[256] = "";
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
		TDEBUG("add new msg to queue, head_size:" << msg_head.Size() 
            << ",msg_class:" << message_pt->GetMsgClass()
			<< ", msg_type:" << message_pt->GetMsgType()
			<< ", req_no:" << message_pt->GetReqNo()
			<< ", rep_no:" << message_pt->GetRepNo()
			<< ", confirm:" << message_pt->GetConfirm()
			<< ", content_size:" << msg_head.ContentSize());
	}
	else
	{
		TERROR("msg UnSerialize failed");
		SAFE_DELETE_PTR(message_pt)
	}
	TDEBUG("after fetch package, read_index:" << GetRecvBuffer().GetReadIndex()
		<< ", write_index:" << GetRecvBuffer().GetWriteIndex()
		<< ", recvbuff.space:" << GetRecvBuffer().GetSpace());
}

void ServerSession::OnClose()
{
    // TODO:

    TConnection::OnClose();
}

// 获取会话通道类型
ESessionChannelType ServerSession::GetChannelType()
{
	return channel_type_;
}

// 设置会话通道类型
void ServerSession::SetChannelType(ESessionChannelType e_type)
{
	channel_type_ = e_type;
	if (ESessionChannelType::E_CHANNEL_NONE == channel_type_)
	{
		// 清理通道数据
		channel_info_.Reset();
	}
}

const SessionChannelInfo & ServerSession::GetChannleInfo()
{
	return channel_info_;
}

// 设置通道服务器信息
void ServerSession::SetChannelServerInfo(EServerType server_type, int32_t index_id, int32_t server_id)
{
	if (channel_info_.server_type == EServerType::E_SERVER_TYPE_INVALID_SERVER)
	{
		channel_info_.server_type = server_type;
		channel_info_.server_index = index_id;
		channel_info_.server_id = server_id;
	}
	else 
	{
		TERROR("set channel server info failed, because server_type is binded to :" << (int32_t)(channel_info_.server_type));
	}
}
// 设置通道客户端标识
void ServerSession::SetChannelUserId(int64_t user_id)
{
	if (channel_info_.user_id == 0)
	{
		channel_info_.user_id = user_id;
	}
	else 
	{
		TERROR("set channel user info failed, because user_id is binded to :" << channel_info_.user_id);
	}
}