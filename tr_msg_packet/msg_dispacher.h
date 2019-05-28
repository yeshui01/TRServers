/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-28 13:47
 * Email        : 824338670@qq.com
 * Filename     : msg_dispacher.h
 * Description  : 
 * ======================================================*/

#ifndef __MSG_DISPACHER_H__
#define __MSG_DISPACHER_H__

#include "base_type_def.h"
#include "singleton.h"
#include "msg_handler.h"
#include <map>

class NetMessage;
class MsgDispacher : public TSingleton<MsgDispacher>
{
public:
	MsgDispacher();
	~MsgDispacher();

public:
	void DispachMessage();
	// 注册消息处理器
	bool RegisterMsgHandler(int32_t msg_class, IMessageHandler * handler_pt);
	// 根据msg_class获取消息处理器
	IMessageHandler* FindHandler(int32_t msg_class);
protected:
	// key:msg_class value:handler
	std::map<int32_t, IMessageHandler*> msg_handlers_;
};

#define g_MsgDispacher MsgDispacher::Instance()
#endif	// __MSG_DIPACHER_H__