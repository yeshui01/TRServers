/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-28 13:40
 * Email        : 824338670@qq.com
 * Filename     : net_message.h
 * Description  : 
 * ======================================================*/
#ifndef __NET_MESSAGE_H__
#define __NET_MESSAGE_H__

#include "base_type_def.h"
#include "net_connection.h"
#include <string>

// 消息头
struct NetMsgHead
{
  NetMsgHead()
  {

  }

  void Reset()
  {
    msg_class = 0;
    msg_type = 0;
    req_no = 0;
    rep_no = 0;
    confirm = 0;
    content_size = 0;
    param = 0;
  }

  int32_t Size()
  {
    int32_t head_size = sizeof(msg_class)
                      + sizeof(msg_type)
                      + sizeof(req_no)
                      + sizeof(rep_no)
                      + sizeof(confirm)
                      + sizeof(content_size)
                      + sizeof(param);
    return head_size;
  }

  bool Serialize(char * buffer, int32_t buffer_len);

  bool UnSerialize(const char * buffer, const int32_t buffer_len);

  int32_t ContentSize();

  int32_t CalcPacketSize();
public:
  int32_t msg_class = 0;
  int32_t msg_type = 0;
  int64_t req_no = -1;   // 请求id
  int64_t rep_no = -1;   // 回复id(跟请求id对应)
  int64_t param = 0;     // 携带参数
  int32_t confirm = 0;   // 校验数据
  int32_t content_size = 0; // 数据内容大小
};

class NetMessage
{
public:
  NetMessage();

  /* 消息构造函数
   * @param msg_class : 消息模块大类
   * @param msg_type  : 消息模块下的类型
   */
  NetMessage(int32_t msg_class, int32_t msg_type);
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
  // 设置请求id
  void SetReqNo(int64_t req_no);
  
  // 获取回复id
  int64_t GetRepNo() const;
  int64_t GetRepNo();
  // 设置回复id
  void SetRepNo(int64_t rep_no);

  // 设置消息内容
  void SetContent(const std::string &str_content);
  void SetContent(const char* buffer, const int32_t len);
  // 获取消息内容
  const std::string & GetContent();
  const std::string & GetContent() const;

  // 获取校验标识
  int32_t GetConfirm() const;
  int32_t GetConfirm();

  // 序列化到指定buffer 
  bool Serialize(char * buffer, int32_t buffer_len);
  // 序列化大小计算
  int32_t SerializeByteNum();
  // 数据包头大小
  int32_t HeadSize();
  // 反序列化
  bool UnSerialize(const char * buffer, const int32_t buffer_len);

  // 设置连接
  void SetConnection(TConnection * connection_pt);
  // 获取连接
  TConnection * GetConnection();
  TConnection * GetConnection() const;
protected:
  TConnection * connection_pt_ = nullptr;	// 消息对应的连接
  // int32_t msg_class_/* = 0*/;
  // int32_t msg_type_/* = 0*/;

  // int64_t req_no_ = -1;		// 请求id
  // int64_t rep_no_ = -1;		// 回复id(跟请求id对应)
  // int32_t confirm_ = 0;		// 校验数据
  NetMsgHead msg_head_;   // 消息头
  std::string content_;		// 消息内容
};

#endif // __NET_MESSAGE_H__
