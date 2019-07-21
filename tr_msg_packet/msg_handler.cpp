/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-28 13:55
 * Email        : 824338670@qq.com
 * Filename     : msg_handler.cpp
 * Description  : 
 * ======================================================*/
#include "msg_handler.h"
#include "net_message.h"
#include "log_module.h"
#include "net_connection.h"

IMessageHandler::IMessageHandler()
{
}

IMessageHandler::~IMessageHandler()
{

}

EMsgHandleResult IMessageHandler::HandleMsg(int32_t msg_type, TConnection *session_pt, const NetMessage * message_pt)
{
	if (!message_pt)
		return EMsgHandleResult::E_MSG_HANDLE_NOT_RETURN;

	auto it_handle_fun = msg_handlers_.find(msg_type);
	if (it_handle_fun != msg_handlers_.end())
	{
		// return it_handle_fun->second(const_cast<TConnection*>(messag_pt->GetConnection()), messag_pt);
		return it_handle_fun->second(session_pt, message_pt);
	}
	else
	{
		TERROR("not found msg_type handler, msg_type:" << msg_type << ", msg_class:" << message_pt->GetMsgClass());
	}
	return EMsgHandleResult::E_MSG_HANDLE_NOT_RETURN;
}

void IMessageHandler::ClearContent()
{
	rep_content_ = "";
}

const std::string & IMessageHandler::GetRepContent()
{
	return rep_content_;
}

void IMessageHandler::BindMsgHandle()
{

}

void IMessageHandler::AddHandleRepCallback(std::function<void (void)> && handle_finish_cb)
{
	v_after_msg_handle_cb_.push_back(handle_finish_cb);
}

void IMessageHandler::RunRepCallback()
{
	if (v_after_msg_handle_cb_.empty())
		return;
	for (auto & f : v_after_msg_handle_cb_)
	{
		f();
	}
	v_after_msg_handle_cb_.clear();
}