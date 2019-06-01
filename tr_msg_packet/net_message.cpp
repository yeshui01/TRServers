/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-28 13:40
 * Email        : 824338670@qq.com
 * Filename     : net_message.cpp
 * Description  : 
 * ======================================================*/
#include "net_message.h"

int32_t NetMsgHead::ContentSize()
{
	return content_size;
}
  
int32_t NetMsgHead::CalcPacketSize()
{
	return Size() + ContentSize();
}

bool NetMsgHead::Serialize(char * buffer, int32_t buffer_len)
{
	if (!buffer)
		return false;
	if (Size() > buffer_len)
		return false;
	int32_t * data_pt = reinterpret_cast<int32_t*>(buffer);
	*data_pt = msg_class;		// msg_class
	++data_pt;
	*data_pt = msg_type;		// msg_type
	++data_pt;
	*data_pt = confirm;			// confirm
	++data_pt;
	*data_pt = content_size;	// content_size
	++data_pt;
	int64_t *big_field_pt = reinterpret_cast<int64_t*>(data_pt);
	*big_field_pt = req_no;	// req_no
	++big_field_pt;
	*big_field_pt = rep_no;	// rep_no
	++big_field_pt;
	return true;
}

bool NetMsgHead::UnSerialize(const char * buffer, const int32_t buffer_len)
{
	if (!buffer)
	  return false;
	
	if (buffer_len < Size())
	  return false;

	const int32_t * data_pt = reinterpret_cast<const int32_t*>(buffer);
	msg_class = *data_pt;	// msg_class
	++data_pt;
	msg_type = *data_pt;	// msg_type
	++data_pt;
	confirm = *data_pt;		// confirm
	++data_pt;
	content_size = *data_pt;	// content_size
	++data_pt;
	const int64_t *big_field_pt = reinterpret_cast<const int64_t*>(data_pt);
	req_no = *big_field_pt;		// req_no
	++big_field_pt;
	rep_no = *big_field_pt;		// rep_no
	return true;
}


NetMessage::NetMessage()
{
	
}

NetMessage::NetMessage(int32_t msg_class, int32_t msg_type)
{
	msg_head_.msg_class = msg_class;
	msg_head_.msg_type = msg_type;
}

NetMessage::~NetMessage()
{

}
 
// 获取消息模块大类
int32_t NetMessage::GetMsgClass() const
{
	return msg_head_.msg_class;
}

int32_t NetMessage::GetMsgClass()
{
  return msg_head_.msg_class;
}

// 获取消息模块下的类型
int32_t NetMessage::GetMsgType() const
{
  return msg_head_.msg_type;
}

int32_t NetMessage::GetMsgType()
{
	return msg_head_.msg_type;
}

// 获取请求id 
int64_t NetMessage::GetReqNo() const
{
	return msg_head_.req_no;
}
int64_t NetMessage::GetReqNo()
{
	return msg_head_.req_no;
}

// 获取回复id
int64_t NetMessage::GetRepNo() const
{
	return msg_head_.rep_no;
}

int64_t NetMessage::GetRepNo()
{
	return msg_head_.rep_no;
}
 
void NetMessage::SetContent(const std::string &str_content)
{
	content_ = str_content;
	msg_head_.content_size = content_.size();
}

void NetMessage::SetContent(const char* buffer, const int32_t len)
{
	if (buffer && len > 0)
	{
		content_ = std::string(buffer, len);
		msg_head_.content_size = content_.size();
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
	msg_head_.req_no = req_no;	
}

void NetMessage::SetRepNo(int64_t rep_no)
{
	msg_head_.rep_no = rep_no;
}

int32_t NetMessage::GetConfirm() const
{
	return msg_head_.confirm;
}

int32_t NetMessage::GetConfirm()
{
	return msg_head_.confirm;
}

bool NetMessage::Serialize(char * buffer, int32_t buffer_len)
{
	if (!buffer)
	  	return false;
	int32_t packet_size = SerializeByteNum();
	if (buffer_len < packet_size)
	  return false;

	if (msg_head_.Serialize(buffer, buffer_len))
	{
		memcpy(buffer+msg_head_.Size(), content_.c_str(), content_.length());
	}
	else
	{
		return false;
	}
	return true;	
}

int32_t NetMessage::SerializeByteNum()
{
	int32_t packet_size = msg_head_.Size() + msg_head_.ContentSize();
	return packet_size;
}

bool NetMessage::UnSerialize(const char * buffer, const int32_t buffer_len)
{
	if (!buffer)
	  return false;
	
	if (buffer_len < msg_head_.Size())
	  return false;
	
	if (msg_head_.UnSerialize(buffer, buffer_len))
	{
		if (SerializeByteNum() - HeadSize() > 0)
		{
			content_ = std::string(reinterpret_cast<const char*>(buffer + msg_head_.Size()), SerializeByteNum() - HeadSize());
		}
	}
	else 
	{
		msg_head_.Reset();
		return false;
	}
	return true;
}

int32_t NetMessage::HeadSize()
{
	return msg_head_.Size();
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

TConnection * NetMessage::GetConnection() const
{
	return connection_pt_;
}