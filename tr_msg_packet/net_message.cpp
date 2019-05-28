/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-28 13:40
 * Email        : 824338670@qq.com
 * Filename     : net_message.cpp
 * Description  : 
 * ======================================================*/
#include "net_message.h"

NetMessage::NetMessage()
{
	
}

NetMessage::NetMessage(int32_t msg_class, int32_t msg_type) 
	: msg_class_(msg_class), msg_type_(msg_type)
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

void NetMessage::SetContent(const char* buffer, const int32_t len)
{
	if (buffer && len > 0)
	{
		content_ = std::string(buffer, len);
	}
}
 
const std::string & NetMessage::GetContent()
{
	return content_;
}

const std::string & NetMessage::GetContent() const
{
	return content_;
}

void NetMessage::SetReqNo(int64_t req_no)
{
	req_no_ = req_no;	
}

void NetMessage::SetRepNo(int64_t rep_no)
{
	rep_no_ = rep_no;
}

int32_t NetMessage::GetConfirm() const
{
	return confirm_;
}

int32_t NetMessage::GetConfirm()
{
	return confirm_;
}

bool NetMessage::Serialize(char * buffer, int32_t buffer_len)
{
	if (!buffer)
	  	return false;
	int32_t packet_size = SerializeByteNum();
	if (buffer_len < packet_size)
	  return false;
	
	int32_t * data_pt = reinterpret_cast<int32_t*>(buffer);
	*data_pt = packet_size;		// 数据包总大小
	++data_pt;
	*data_pt = msg_class_;		// msg_class
	++data_pt;
	*data_pt = msg_type_;		// msg_type
	++data_pt;
	*data_pt = confirm_;		// confirm
	++data_pt;
	int64_t *big_field_pt = reinterpret_cast<int64_t*>(data_pt);
	*big_field_pt = req_no_;	// req_no
	++big_field_pt;
	*big_field_pt = rep_no_;	// rep_no
	++big_field_pt;
	// content
	buffer = reinterpret_cast<char*>(big_field_pt);
	memcpy(buffer, content_.c_str(), content_.length());
	return true;	
}

int32_t NetMessage::SerializeByteNum()
{
	int32_t packet_size = 0;
	packet_size = sizeof(int32_t)
	  	+ sizeof(msg_class_) 
	  	+ sizeof(msg_type_) 
		+ sizeof(req_no_) 
		+ sizeof(rep_no_) 
		+ sizeof(confirm_) 
		+ content_.length();
	return packet_size;
}

bool NetMessage::UnSerialize(const char * buffer, const int32_t buffer_len)
{
	if (!buffer)
	  return false;
	
	if (buffer_len < sizeof(int32_t))
	  return false;

	const int32_t * data_pt = reinterpret_cast<const int32_t*>(buffer);
	int32_t packet_size = *data_pt;
	if (buffer_len < packet_size)
	  return false;

	++data_pt;
	msg_class_ = *data_pt;
	++data_pt;
	msg_type_ = *data_pt;
	++data_pt;
	confirm_ = *data_pt;
	++data_pt;
	const int64_t *big_field_pt = reinterpret_cast<const int64_t*>(data_pt);
	req_no_ = *big_field_pt;
	++big_field_pt;
	rep_no_ = *big_field_pt;
	++big_field_pt;
	// content
	content_ = std::string(reinterpret_cast<const char*>(big_field_pt), packet_size - HeadSize());
	return true;
}

int32_t NetMessage::HeadSize()
{
	int32_t head_size = 0;
	head_size = sizeof(int32_t)
	  	+ sizeof(msg_class_) 
	  	+ sizeof(msg_type_) 
		+ sizeof(req_no_) 
		+ sizeof(rep_no_) 
		+ sizeof(confirm_);
	return head_size;
}

// 设置连接
void NetMessage::SetConnection(TConnection * connection_pt)
{
	connection_pt_ = connection_pt;
}
// 获取连接
TConnection * NetMessage::GetConnection()
{
	return connection_pt_;
}