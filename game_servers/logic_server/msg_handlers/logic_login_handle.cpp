/*=======================================================
# Author       : mknight
# Last modified: 2019-12-15 16:00
# Email        : 824338670@qq.com
# Filename     : logic_login_handle.cpp
# Description  : 
# =======================================================*/

#include "logic_login_handle.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "protocol_login.h"

#include "protcl_frame.pb.h"
#include "protcl_login.pb.h"

#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
#include "logic_server/logic_server.h"
#include "msg_handler.h"
#include "protocol_error_code.h"
LogicLoginHandler::LogicLoginHandler()
{

}

LogicLoginHandler::~LogicLoginHandler()
{

}

// 绑定消息处理接口
void LogicLoginHandler::BindMsgHandle()
{
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_ROOT2LOGIC_CREATE_ROLE), LogicLoginHandler, OnCreateRole);
}

TR_BEGIN_HANDLE_MSG(LogicLoginHandler, OnCreateRole, E_LOGIN_MSG_ROOT2LOGIC_CREATE_ROLE)
{
    // TODO:
}
TR_END_HANDLE_MSG_AND_RETURN_MSG