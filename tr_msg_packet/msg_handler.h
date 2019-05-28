/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-28 13:55
 * Email        : 824338670@qq.com
 * Filename     : msg_handler.h
 * Description  : 
 * ======================================================*/

#ifndef __MSG_HANDLER_H__
#define __MSG_HANDLER_H__

#include "base_type_def.h"
#include <string>
#include <functional>
#include <map>
// 消息处理结果类型
enum class EMsgHandleResult : int32_t
{
	E_MSG_HANDLE_NOT_RETURN = 0,			// 不需要返回
	E_MSG_HANDLE_RETURN_CONTENT = 1,		// 返回消息
};

class NetMessage;
class IMessageHandler
{
public:
	IMessageHandler();
	virtual ~IMessageHandler();

	// 处理消息入口
	virtual EMsgHandleResult HandleMsg(int32_t msg_type, const NetMessage * messag_pt);
	// 绑定消息处理接口
	virtual void BindMsgHandle() = 0;
	// 获取返回消息的内容
	const std::string & GetRepContent();
	// 清理返回消息的内容
	void ClearContent();
protected:
	// 返回消息的内容缓存
	std::string rep_content_;
	// 消息类型对应的处理成员
	// key:msg_type value:member function ptr
	std::map<int32_t, std::function<EMsgHandleResult (const NetMessage * messag_pt)> > msg_handlers_;
};

#endif  // __MSG_HANDLER_H__

