#include "net_message.h"
 
NetMessage::NetMessage(TConnection * connection_pt, int32_t msg_class, int32_t msg_type) 
	: connection_pt_(connection_pt) , msg_class_(msg_class), msg_type_(msg_type)
{

}

NetMessage::~NetMessage()
{

}
 
// 获取消息模块大类
int32_t NetMessage::GetMsgClass() const
{
	return msg_class_;
}

int32_t NetMessage::GetMsgClass()
{
  return msg_class_;
}

// 获取消息模块下的类型
int32_t NetMessage::GetMsgType() const
{
  return msg_type_;
}

int32_t NetMessage::GetMsgType()
{
	return msg_type_;
}

// 获取请求id 
int64_t NetMessage::GetReqNo() const
{
	return req_no_;
}
int64_t NetMessage::GetReqNo()
{
	return req_no_;
}

// 获取回复id
int64_t NetMessage::GetRepNo() const
{
	return rep_no_;
}

int64_t NetMessage::GetRepNo()
{
	return rep_no_;
}
 
void NetMessage::SetContent(const std::string &str_content)
{
	content_ = str_content;
}

void NetMessage::SetContent(const char* buffer, int32_t len)
{
	content_ = std::string(buffer, len);
}
 
const std::string & NetMessage::GetContent()
{
	return content_;
}

const std::string & NetMessage::GetContent() const
{
	return content_;
}
