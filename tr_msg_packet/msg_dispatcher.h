/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-28 13:47
 * Email        : 824338670@qq.com
 * Filename     : msg_dispachter.h
 * Description  : 
 * ======================================================*/

#ifndef __MSG_DISPACHER_H__
#define __MSG_DISPACHER_H__

#include "base_type_def.h"
#include "singleton.h"
#include "msg_handler.h"
#include <map>

class NetMessage;
class MsgDispatcher : public TSingleton<MsgDispatcher>
{
public:
	MsgDispatcher();
	~MsgDispatcher();

public:
	// 消息处理
	void DispatchMessage();
	// 注册消息处理器
	// bool RegisterMsgHandler(int32_t msg_class, IMessageHandler * handler_pt);
	template<class MsgHandlerType>
	bool RegisterMsgHandlerEx(int32_t msg_class)
	{
		auto it = msg_handlers_.find(msg_class);
		if (it == msg_handlers_.end())
		{
			MsgHandlerType * handler_pt = new MsgHandlerType();
			auto ret = msg_handlers_.insert(std::make_pair(msg_class, handler_pt));
			if (ret.second)
			{
				handler_pt->BindMsgHandle();
			}
			else
			{
				delete handler_pt;
			}
			return ret.second;
		}
		return false;
	}
	// 根据msg_class获取消息处理器
	IMessageHandler* FindHandler(int32_t msg_class);
protected:
	// key:msg_class value:handler
	std::map<int32_t, IMessageHandler*> msg_handlers_;
};

#define g_MsgDispatcher MsgDispatcher::Instance()
#endif	// __MSG_DIPACHER_H__