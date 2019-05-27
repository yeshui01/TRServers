#ifndef __NET_MESSAGE_H__
#define __NET_MESSAGE_H__

#include "base_type_def.h"
#include "net_connection.h"
#include <string>

class NetMessage
{
public:
  NetMessage() = delete;

  /* 消息构造函数
   * @param msg_class : 消息模块大类
   * @param msg_type  : 消息模块下的类型
   */
  NetMessage(TConnection * connection_pt, int32_t msg_class, int32_t msg_type);
  ~NetMessage();
	
  // 获取消息模块大类
  int32_t GetMsgClass() const;
  int32_t GetMsgClass();

  // 获取消息模块下的类型
  int32_t GetMsgType() const;
  int32_t GetMsgType();

  // 获取请求id 
  int64_t GetReqNo() const;
  int64_t GetReqNo();
  
  // 获取回复id
  int64_t GetRepNo() const;
  int64_t GetRepNo();

  // 设置消息内容
  void SetContent(const std::string &str_content);
  void SetContent(const char* buffer, int32_t len);
  // 获取消息内容
  const std::string & GetContent();
  const std::string & GetContent() const;
protected:
  TConnection * connection_pt_;	// 消息对应的连接
  int32_t msg_class_/* = 0*/;
  int32_t msg_type_/* = 0*/;

  int64_t req_no_ = -1;		// 请求id
  int64_t rep_no_ = -1;		// 回复id(跟请求id对应)
  int32_t confirm_ = 0;		// 校验数据
  std::string content_;		// 消息内容
};

#endif // __NET_MESSAGE_H__
