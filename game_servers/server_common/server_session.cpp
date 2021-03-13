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
#include "server_info_manager.h"
#include "server_time.h"
#include "server_buffer.h"
#include "msg_pool.h"
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
	do
	{
		NetMsgHead msg_head;
		if (GetRecvBuffer().PeekDataSize() < msg_head.Size())
			break;
		// peek 数据包头
		char buffer_head[256] = "";
		int32_t real_read_size = GetRecvBuffer().ReadData(buffer_head, msg_head.Size(), true);
		if (real_read_size < msg_head.Size())
			break;

		if (!msg_head.UnSerialize(buffer_head, sizeof(buffer_head)))
		{
			TERROR("msg head unserialize msg failed, maybe logic error!!!");
			break;
		}
		if (msg_head.CalcPacketSize() > GetRecvBuffer().PeekDataSize())
		{
			// 数据还未接受完整
			break;
		}
		int32_t packet_size = msg_head.CalcPacketSize();
		if (packet_size > 10240)
		{
			TWARN("packet maybe too large, msg(" << msg_head.msg_class << "," << msg_head.msg_type << ")");
		}
		// 读取数据包
		TDEBUG("fetch package, read_index:" << GetRecvBuffer().GetReadIndex()
											<< ", write_index:" << GetRecvBuffer().GetWriteIndex());

		auto packet_buffer = g_ServerBuffer.HoldMsgBuffer(packet_size);
		GetRecvBuffer().ReadData(packet_buffer->buffer, packet_size);
		NetMessage *message_pt = g_NetMsgPool.ApplyMsg();
		if (message_pt->UnSerialize(packet_buffer->buffer, packet_size))
		{
			message_pt->SetConnection(this);
			g_MsgQueue.AddMsg(message_pt);
			TDEBUG("add new msg to queue, head_size:" << msg_head.Size()
													  << ",msg_class:" << message_pt->GetMsgClass()
													  << ", msg_type:" << message_pt->GetMsgType()
													  << ", req_no:" << message_pt->GetReqNo()
													  << ", rep_no:" << message_pt->GetRepNo()
													  << ", confirm:" << message_pt->GetConfirm()
													  << ", content_size:" << msg_head.ContentSize()
													  << ", param:" << message_pt->GetParam());
		}
		else
		{
			TERROR("msg UnSerialize failed");
			g_NetMsgPool.RecycleMsg(message_pt);
		}
		TDEBUG("after fetch package, read_index:" << GetRecvBuffer().GetReadIndex()
												  << ", write_index:" << GetRecvBuffer().GetWriteIndex()
												  << ", recvbuff.space:" << GetRecvBuffer().GetSpace());
	} while (GetRecvBuffer().PeekDataSize() > 0);
}

void ServerSession::OnClose()
{
	for (auto & cb : close_callbacks_)
	{
		cb(this);
	}
	if (!reconnect_)
	{
		close_callbacks_.clear();
		if (GetChannelType() == ESessionChannelType::E_CHANNEL_SERVER_TO_SERVER)
		{
			g_ServerManager.DeleteServerInfo(GetConnId());
		}
		channel_type_ = ESessionChannelType::E_CHANNEL_NONE;
		channel_info_.Reset();
	}
	else
	{
		// 需要重连, 1秒钟后重连
		reconnect_time_ = g_ServerTime.NowTime() + 1;
	}
	
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

const SessionChannelInfo & ServerSession::GetChannelInfo()
{
	return channel_info_;
}

// 设置通道服务器信息
void ServerSession::SetChannelServerInfo(EServerRouteNodeType route_type, int32_t index_id, int32_t server_id)
{
	if (channel_info_.node_type == EServerRouteNodeType::E_SERVER_ROUTE_NODE_NONE)
	{
		channel_info_.node_type = route_type;
		channel_info_.server_index = index_id;
		channel_info_.server_id = server_id;
	}
	else 
	{
		TERROR("set channel server info failed, because server_type is binded to :" << (int32_t)(channel_info_.node_type));
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

int32_t ServerSession::CalcServerId(EServerRouteNodeType route_type, int32_t index_id)
{
	return index_id * 1000 + int32_t(route_type);
}

void ServerSession::AddCloseCallback(session_close_callback_t && close_cb)
{
	close_callbacks_.push_back(close_cb);
}
