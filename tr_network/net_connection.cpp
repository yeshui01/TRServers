/********************************************************
# Author       : mknight
# Last modified: 2019-05-02 11:19
# Email        : 824338670@qq.com
# Filename     : net_connection.h
# Description  : 
# *******************************************************/
#include "net_connection.h"
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <memory.h>
#include "net_socket.h"
#include "log_module.h"
#include "connect_manager.h"
#include "base_server.h"
TConnection::TConnection()
{
    conn_id_ = connection_id_index_;
    ++connection_id_index_;
}
TConnection::~TConnection()
{
}

// 接受连接
void TConnection::HandleAccept()
{
    TSocket::HandleAccept();
}
// 收数据
void TConnection::HandleRecv()
{
    TSocket::HandleRecv();
}
// 写数据
void TConnection::HandleWrite()
{
    TSocket::HandleWrite();
}
// 出错处理
void TConnection::HandleError()
{
    TSocket::HandleError();
}

void TConnection::OnClose()
{
    // g_ConnectMgr.DelConnection(this);
    if (server_ && !reconnect_)
    {
        // 需要重连的不回收
        server_->RecycleConnect(this);
    }
}

void TConnection::AfterReadData(int32_t read_size)
{
    // // do nothing,这里不处理，派生类自己处理
    // TDEBUG("msg connection afterreaddata, read_size:" << read_size
	// 	<< ", recv_buffer_rest_space:" << GetRecvBuffer().GetRestSpace()
	// 	<< ", data_size:" << GetRecvBuffer().PeekDataSize()
	// 	<< ", read_index:" << GetRecvBuffer().GetReadIndex()
	// 	<< ", write_index:" << GetRecvBuffer().GetWriteIndex());
	// // 读取消息到消息队列
	// do
	// {
	// 	NetMsgHead msg_head;
	// 	if (GetRecvBuffer().PeekDataSize() < msg_head.Size())
	// 		break;
	// 	// peek 数据包头
	// 	char buffer_head[256] = "";
	// 	int32_t real_read_size = GetRecvBuffer().ReadData(buffer_head, msg_head.Size(), true);
	// 	if (real_read_size < msg_head.Size())
	// 		break;

	// 	if (!msg_head.UnSerialize(buffer_head, sizeof(buffer_head)))
	// 	{
	// 		TERROR("msg head unserialize msg failed, maybe logic error!!!");
	// 		break;
	// 	}
	// 	if (msg_head.CalcPacketSize() > GetRecvBuffer().PeekDataSize())
	// 	{
	// 		// 数据还未接受完整
	// 		break;
	// 	}
	// 	int32_t packet_size = msg_head.CalcPacketSize();
	// 	if (packet_size > 10240)
	// 	{
	// 		TWARN("packet maybe too large, msg(" << msg_head.msg_class << "," << msg_head.msg_type << ")");
	// 	}
	// 	// 读取数据包
	// 	TDEBUG("fetch package, read_index:" << GetRecvBuffer().GetReadIndex()
	// 										<< ", write_index:" << GetRecvBuffer().GetWriteIndex());

	// 	auto packet_buffer = g_ServerBuffer.HoldMsgBuffer(packet_size);
	// 	GetRecvBuffer().ReadData(packet_buffer->buffer, packet_size);
	// 	NetMessage *message_pt = g_NetMsgPool.ApplyMsg();
	// 	if (message_pt->UnSerialize(packet_buffer->buffer, packet_size))
	// 	{
	// 		message_pt->SetConnection(this);
	// 		g_MsgQueue.AddMsg(message_pt);
	// 		TDEBUG("add new msg to queue, head_size:" << msg_head.Size()
	// 												  << ",msg_class:" << message_pt->GetMsgClass()
	// 												  << ", msg_type:" << message_pt->GetMsgType()
	// 												  << ", req_no:" << message_pt->GetReqNo()
	// 												  << ", rep_no:" << message_pt->GetRepNo()
	// 												  << ", confirm:" << message_pt->GetConfirm()
	// 												  << ", content_size:" << msg_head.ContentSize()
	// 												  << ", param:" << message_pt->GetParam());
	// 	}
	// 	else
	// 	{
	// 		TERROR("msg UnSerialize failed");
	// 		g_NetMsgPool.RecycleMsg(message_pt);
	// 	}
	// 	TDEBUG("after fetch package, read_index:" << GetRecvBuffer().GetReadIndex()
	// 											  << ", write_index:" << GetRecvBuffer().GetWriteIndex()
	// 											  << ", recvbuff.space:" << GetRecvBuffer().GetSpace());
	// } while (GetRecvBuffer().PeekDataSize() > 0);
}

// 关联一个server
void TConnection::AttachServer(TBaseServer * server)
{
    server_ = server;
}
// 获取关联的server
TBaseServer * TConnection::GetServer()
{
    return server_;
}

int32_t TConnection::GetConnId()
{
    return conn_id_;
}


// 重连
void TConnection::Reconnect()
{
    if (GetFd() != INVALID_SOCKET_FD)
    {
        TERROR("Reconnect fail, GetFd() != INVALID_SOCKET_FD, connec_id:" << conn_id_);
        return;
    }
    
    if (ESocketOpCode::E_SOCKET_OP_CODE_CORRECT != Connect(connect_addr_.ip, connect_addr_.port))
    {
        TERROR("reconnect fail, ip:" << connect_addr_.ip << ", port:" << connect_addr_.port);
    }
    else
    {
        GetRecvBuffer().Reset();
        GetWriteBuffer().Reset();
        if (server_)
        {
            server_->OnNewConnectComeIn(this);
        }
        TWARN("reconnect succ, connectid:" << conn_id_ << ", ip:" << connect_addr_.ip << ", port:" << connect_addr_.port);
        if (reconnect_func_ != nullptr)
        {
            reconnect_func_(this);
        }
        else
        {
            TERROR("try connect, but reconnect handle is nullptr");
        }
    }
}
// 设置是否重连
void TConnection::SetNeedReconnect(bool connect)
{
	reconnect_ = connect;
}
// 设置重连时候的操作
void TConnection::SetReConnectAction(std::function<void (TConnection *connect_pt)> && reconnect_func)
{
	reconnect_func_ = reconnect_func;
}

// 连接标识id，从1000开始
int32_t TConnection::connection_id_index_ = 1000;