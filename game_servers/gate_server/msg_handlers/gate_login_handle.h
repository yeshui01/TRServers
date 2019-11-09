/*=======================================================
# Author       : mknight
# Last modified: 2019-11-02 12:30
# Email        : 824338670@qq.com
# Filename     : gate_login_handle.h
# Description  : 
# =======================================================*/

#ifndef __GATE_LOGIN_HANDLE_H__
#define __GATE_LOGIN_HANDLE_H__

#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/net_message.h"
#include "tr_msg_packet/msg_handler.h"
#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_log/log_module.h"

class GateLoginHandler : public IMessageHandler
{
public:
    GateLoginHandler();
	virtual ~GateLoginHandler();
    // GateLoginHandler
	void BindMsgHandle() override;

public:
    // 消息处理成员函数-登录
    EMsgHandleResult OnClientLogin(TConnection *session_pt, const NetMessage * message_pt);
    // 消息处理-创角
    DECLATE_MSG_HANDLER_FUN(OnCreateRole);
};
#endif // __GATE_LOGIN_HANDLE_H__