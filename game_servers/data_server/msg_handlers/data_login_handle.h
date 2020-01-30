/*=======================================================
# Author       : mknight
# Last modified: 2020-01-27 19:15
# Email        : 824338670@qq.com
# Filename     : data_login_handle.h
# Description  : 
# =======================================================*/

#ifndef __TR_DATA_LOGIN_HANDLE_H__
#define __TR_DATA_LOGIN_HANDLE_H__

#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/net_message.h"
#include "tr_msg_packet/msg_handler.h"
#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_log/log_module.h"

class DataLoginHandler : public IMessageHandler
{
public:
    DataLoginHandler();
	virtual ~DataLoginHandler();
    // DataLoginHandler
	void BindMsgHandle() override;

public:
    // 消息处理-创角
    DECLARE_MSG_HANDLER_FUN(OnDataCreateRole);
    // 获取快照列表
    DECLARE_MSG_HANDLER_FUN(OnDataFetchSnapshot);
};
#endif // __TR_DATA_LOGIN_HANDLE_H__