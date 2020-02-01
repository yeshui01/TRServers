/*=======================================================
# Author       : mknight
# Last modified: 2020-01-31 11:10
# Email        : 824338670@qq.com
# Filename     : center_login_handle.h
# Description  : 
# =======================================================*/
#ifndef __TR_CENTER_LOGIN_HANDLE_H__
#define __TR_CENTER_LOGIN_HANDLE_H__

#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/net_message.h"
#include "tr_msg_packet/msg_handler.h"
#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_log/log_module.h"
class CenterLoginHandler : public IMessageHandler
{
public:
    CenterLoginHandler();
	virtual ~CenterLoginHandler();
    // 绑定消息处理接口
	void BindMsgHandle() override;

public:
   // 消息处理-创角
    DECLARE_MSG_HANDLER_FUN(OnCSEnterGame);
};

#endif // __TR_CENTER_LOGIN_HANDLE_H__
