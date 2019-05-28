/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-28 13:40
 * Email        : 824338670@qq.com
 * Filename     : msg_queue.h
 * Description  : 
 * ======================================================*/
#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__
#include "base_type_def.h"
#include "singleton.h"
#include <list>

class NetMessage;
class TMessageQueue : public TSingleton<TMessageQueue>
{
public:
	TMessageQueue();
	~TMessageQueue();
	// 将消息放入队列
	void AddMsg(NetMessage * message_pt);
	// 提取一个消息
	NetMessage* PopMsg();
	// 获取队列大小
	int32_t GetSize();
protected:
	std::list<NetMessage*> msg_queue_;
};

#define g_MsgQueue TMessageQueue::Instance()

#endif // __MSG_QUEUE_H__
