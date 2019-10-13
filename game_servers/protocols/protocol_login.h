/*=======================================================
# Author       : mknight
# Last modified: 2019-10-04 14:36
# Email        : 824338670@qq.com
# Filename     : protocol_login.h
# Description  : 
# =======================================================*/

#ifndef __TR_PROTOCOL_LOGIN_H__
#define __TR_PROTOCOL_LOGIN_H__

#include "base_type_def.h"

enum class ELoginMsgType : int32_t
{
    E_LOGIN_MSG_C2S_REGISTER_ACCOUNT                    = 1,  // 注册账号
    E_LOGIN_MSG_C2S_LOGIN                               = 2,  // c->s登录
    E_LOGIN_MSG_GG2ROOT_LOGIN                           = 3,  // gg->root请求登录
    E_LOGIN_MSG_ROOT2LOGIN_ACCT_CHECK                   = 4,  // root->login账号校验
};

#define INT_LOGINMSG(msg_type) int32_t(ELoginMsgType::msg_type)

#endif // __TR_PROTOCOL_LOGIN_H__