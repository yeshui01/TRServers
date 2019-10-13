/*=======================================================
# Author       : mknight
# Last modified: 2019-10-04 15:35
# Email        : 824338670@qq.com
# Filename     : login_account_handle.h
# Description  : 
# =======================================================*/
#ifndef __TR_LOGIN_ACCOUNT_HANDLE_H__
#define __TR_LOGIN_ACCOUNT_HANDLE_H__

#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/net_message.h"
#include "tr_msg_packet/msg_handler.h"
#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_log/log_module.h"

class LoginAccountHandler : public IMessageHandler
{
public:
    LoginAccountHandler();
	virtual ~LoginAccountHandler();
    // 绑定消息处理接口
	void BindMsgHandle() override;

public:
    // 消息处理成员函数-注册账号
    EMsgHandleResult OnRegisnterAccount(TConnection *session_pt, const NetMessage * messag_pt);
};
#endif // __TR_LOGIN_ACCOUNT_HANDLE_H__
