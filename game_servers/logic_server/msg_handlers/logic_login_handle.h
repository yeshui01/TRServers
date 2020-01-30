/*=======================================================
# Author       : mknight
# Last modified: 2019-12-15 16:00
# Email        : 824338670@qq.com
# Filename     : logic_login_handle.h
# Description  : 
# =======================================================*/
#ifndef __TR_LOGIC_LOGIN_HANDLE_H__
#define __TR_LOGIC_LOGIN_HANDLE_H__

#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/net_message.h"
#include "tr_msg_packet/msg_handler.h"
#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_log/log_module.h"
class LogicLoginHandler : public IMessageHandler
{
public:
    LogicLoginHandler();
	virtual ~LogicLoginHandler();
    // 绑定消息处理接口
	void BindMsgHandle() override;

public:
   // 消息处理-创角
    DECLARE_MSG_HANDLER_FUN(OnLogicCreateRole);
};

#endif // __TR_LOGIC_LOGIN_HANDLE_H__