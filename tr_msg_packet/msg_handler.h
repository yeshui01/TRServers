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
#include <vector>
// 消息处理结果类型
enum class EMsgHandleResult : int32_t
{
	E_MSG_HANDLE_NOT_RETURN = 0,			// 不需要返回
	E_MSG_HANDLE_RETURN_CONTENT = 1,		// 返回消息
};

class NetMessage;
class TConnection;

using msg_type_handler_t = std::function<EMsgHandleResult (TConnection *session_pt, const NetMessage * message_pt) >;

class IMessageHandler
{
public:
	IMessageHandler();
	virtual ~IMessageHandler();

	// 处理消息入口
	virtual EMsgHandleResult HandleMsg(int32_t msg_type, TConnection *session_pt, const NetMessage * message_pt);
	// 绑定消息处理接口
	virtual void BindMsgHandle() = 0;
	// 获取返回消息的内容
	const std::string & GetRepContent();
	// 清理返回消息的内容
	void ClearContent();
	// 添加消息处理后的回调处理,消息处理后立刻执行一次注册的回调
	void AddHandleRepCallback(std::function<void (void)> && handle_finish_cb);
	// 执行一轮消息处理后的回调
	void RunRepCallback();
	
protected:
	// 返回消息的内容缓存
	std::string rep_content_;
	// 消息类型对应的处理成员
	// key:msg_type value:member function ptr
	std::map<int32_t,  msg_type_handler_t> msg_handlers_;
	// 处理完消息后的回调处理
	std::vector<std::function<void (void)> > v_after_msg_handle_cb_;
};


#define MSG_BIND_HANDLER(MSG_TYPE, CLASS_NAME, MEMBER_FUN) msg_handlers_.insert(std::make_pair(MSG_TYPE, std::bind(&CLASS_NAME::MEMBER_FUN, this, std::placeholders::_1, std::placeholders::_2)));

#define RETURN_REP_CONTENT(rep_msg) rep_msg.SerializeToString(&rep_content_); \
return EMsgHandleResult::E_MSG_HANDLE_RETURN_CONTENT;

#define RETURN_NO_HANDLE return EMsgHandleResult::E_MSG_HANDLE_NOT_RETURN;

#define SET_ISOK_AND_RETURN_CONTENT(ISOK_CODE, rep_msg) rep_msg.set_isok(INT_PROTOERR(ISOK_CODE)); \
RETURN_REP_CONTENT(rep_msg)

// 声明消息处理的成员函数
#define DECLARE_MSG_HANDLER_FUN(FUNC_NAME) EMsgHandleResult FUNC_NAME(TConnection *session_pt, const NetMessage * message_pt);

// 开始消息处理
#define TR_BEGIN_HANDLE_MSG(HANDLER_NAME, FUNC_NAME, MSG_NAME) EMsgHandleResult HANDLER_NAME::FUNC_NAME(TConnection *session_pt, const NetMessage * message_pt) \
{\
    REQMSG(MSG_NAME) req; \
    REPMSG(MSG_NAME) rep; \
	TINFO(""#HANDLER_NAME"::"#FUNC_NAME);\
    if (!STRING_TO_PBMSG(message_pt->GetContent(), req)) \
    {\
        rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_PB_PARSE_ERROR));\
        TERROR("parse pbmsg failed");\
        rep.SerializeToString(&rep_content_);\
        return EMsgHandleResult::E_MSG_HANDLE_RETURN_CONTENT;\
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_PB_PARSE_ERROR, rep);\
    } \
	TINFO("req_" << #MSG_NAME << ":" << req.ShortDebugString());

// 结束消息处理,但是不用返回消息
#define TR_END_HANDLE_MSG_NO_RETURN_MSG RETURN_NO_HANDLE \
}

// 结束消息处理,需要返回消息内容
#define TR_END_HANDLE_MSG_AND_RETURN_MSG RETURN_REP_CONTENT(rep);\
}

#endif  // __MSG_HANDLER_H__

