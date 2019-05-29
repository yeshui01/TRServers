/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-28 13:50
 * Email        : 824338670@qq.com
 * Filename     : msg_dispacher.cpp
 * Description  : 
 * ======================================================*/

#include "msg_dispacher.h"
#include "msg_queue.h"
#include "log_module.h"
#include "net_message.h"
#include "common_define.h"
MsgDispacher::MsgDispacher()
{
}

MsgDispacher::~MsgDispacher()
{
	for (auto it = msg_handlers_.begin(); it != msg_handlers_.end(); ++it)
	{
		// delete it->second;
		// it->second = nullptr;
		SAFE_DELETE_PTR(it->second);
	}
	msg_handlers_.clear();
}

// // 注册消息处理器
// bool MsgDispacher::RegisterMsgHandler(int32_t msg_class, IMessageHandler * handler_pt)
// {
// 	if (!handler_pt)
// 		return false;
// 	handler_pt->BindMsgHandle();
// 	return msg_handlers_.insert(std::make_pair(msg_class, handler_pt)).second;
// }
// 根据msg_class获取消息处理器
IMessageHandler* MsgDispacher::FindHandler(int32_t msg_class)
{
	auto it = msg_handlers_.find(msg_class);
	return it != msg_handlers_.end() ? it->second : nullptr;
}

void MsgDispacher::DispachMessage()
{
	auto msg_pt = g_MsgQueue.PopMsg();
	while (msg_pt)
	{
		int32_t msg_class = msg_pt->GetMsgClass();
		auto msg_handler = FindHandler(msg_class);
		if (msg_handler)
		{
			EMsgHandleResult handle_result = msg_handler->HandleMsg(msg_pt->GetMsgType(), msg_pt->GetConnection(), msg_pt);
			if (EMsgHandleResult::E_MSG_HANDLE_RETURN_CONTENT == handle_result)
			{
				// 要返回数据
				NetMessage net_message(msg_pt->GetMsgClass(), msg_pt->GetMsgType());
				net_message.SetContent(msg_handler->GetRepContent());
				net_message.SetRepNo(msg_pt->GetReqNo());
				auto connection_pt = msg_pt->GetConnection();
				if (connection_pt)
				{
					// 是否已经关闭
					if (connection_pt->GetStatus() == ESocketStatus::E_SOCKET_STATUS_CONNECTED
						|| connection_pt->GetStatus() == ESocketStatus::E_SOCKET_STATUS_BE_CONNECT)
					{
						char msg_buffer[10240] = "";
						net_message.SetConnection(connection_pt);
						// 发送数据
						int32_t msg_total_size = net_message.SerializeByteNum();
						if (msg_total_size > sizeof(msg_buffer))
						{
							TERROR("msg_total_size is too large, msg_total_size:" << msg_total_size);
							// 采用动态内存分配
							char * new_tmp_buffer = new char[msg_total_size];
							net_message.Serialize(new_tmp_buffer, msg_total_size);
							connection_pt->Send(new_tmp_buffer, msg_total_size);
							delete [] new_tmp_buffer;
						}
						else
						{
							net_message.Serialize(msg_buffer, sizeof(msg_buffer));
							connection_pt->Send(msg_buffer, msg_total_size);
						}
					}
					else 
					{
						TERROR("msg connection status error, stauts:" << int32_t(connection_pt->GetStatus()));
					}
				}
				else 
				{
					TERROR("msg have no connection");
				}
				msg_handler->ClearContent();
			}
		}
		else 
		{
			TERROR("msg_handler is null, msg_class:" << msg_class);
		}
		delete msg_pt;
		msg_pt = nullptr;
		msg_pt = g_MsgQueue.PopMsg();
	}
}