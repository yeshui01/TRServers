/*=======================================================
# Author       : mknight
# Last modified: 2019-11-02 15:29
# Email        : 824338670@qq.com
# Filename     : root_login_handle.h
# Description  : 
# =======================================================*/

#ifndef __ROOT_LOGIN_MSG_HANDLE_H__
#define __ROOT_LOGIN_MSG_HANDLE_H__

#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/net_message.h"
#include "tr_msg_packet/msg_handler.h"
#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_log/log_module.h"

class RootLoginHandler : public IMessageHandler
{
public:
    RootLoginHandler();
	virtual ~RootLoginHandler();
    // RootLoginHandler
	void BindMsgHandle() override;

public:
    // 消息处理成员函数-登录
    EMsgHandleResult OnGateClientLogin(TConnection *session_pt, const NetMessage * message_pt);
};

#endif // __ROOT_LOGIN_MSG_HANDLE_H__