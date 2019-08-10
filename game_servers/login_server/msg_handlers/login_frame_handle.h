/*=======================================================
# Author       : mknight
# Last modified: 2019-07-13 15:42
# Email        : 824338670@qq.com
# Filename     : login_frame_handle.h
# Description  : 
# =======================================================*/
#ifndef __LOGIN_FRAME_MSG_HANDLE_H__
#define __LOGIN_FRAME_MSG_HANDLE_H__
#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/net_message.h"
#include "tr_msg_packet/msg_handler.h"
#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_log/log_module.h"

class LoginFrameHandler : public IMessageHandler
{
public:
    LoginFrameHandler();
	virtual ~LoginFrameHandler();
    // 绑定消息处理接口
	void BindMsgHandle() override;

public:
    // 消息处理成员函数
    EMsgHandleResult OnRegisterServerInfo(TConnection *session_pt, const NetMessage * messag_pt);
    // 测试转发的消息
    EMsgHandleResult OnTestForwardMsg(TConnection *session_pt, const NetMessage * messag_pt);
};

#endif // __LOGIN_FRAME_MSG_HANDLE_H__