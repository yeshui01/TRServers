/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-28 13:40
 * Email        : 824338670@qq.com
 * Filename     : msg_queue.cpp
 * Description  : 
 * ======================================================*/
#include "msg_queue.h"
#include "net_message.h"

TMessageQueue::TMessageQueue()
{
}

TMessageQueue::~TMessageQueue()
{
}

// 将消息放入队列
void TMessageQueue::AddMsg(NetMessage * message_pt)
{
	if (message_pt)
	{
		msg_queue_.push_back(message_pt);
	}
}

// 提取一个消息
NetMessage* TMessageQueue::PopMsg()
{
	if (!msg_queue_.empty())
	{
		NetMessage* msg = msg_queue_.front();
		msg_queue_.pop_front();
		return msg;
	}
	return nullptr;
}

// 获取队列大小
int32_t TMessageQueue::GetSize()
{
	return msg_queue_.size();
}
