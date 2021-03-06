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

enum ELoginMsgType : int32_t
{
    E_LOGIN_MSG_C2S_REGISTER_ACCOUNT                    = 1,  // 注册账号
    E_LOGIN_MSG_C2S_LOGIN                               = 2,  // c->s登录
    E_LOGIN_MSG_GG2ROOT_LOGIN                           = 3,  // gg->root请求登录
    E_LOGIN_MSG_ROOT2LOGIN_ACCT_CHECK                   = 4,  // root->login账号校验
    E_LOGIN_MSG_C2S_CREATE_ROLE                         = 5,  // c->s 创建角色
    E_LOGIN_MSG_GG2ROOT_CREATE_ROLE                     = 6,  // gg->root 创建角色
    E_LOGIN_MSG_ROOT2LOGIN_CREATE_ROLE                  = 7,  // root->login 创建角色
    E_LOGIN_MSG_ROOT2DATA_CREATE_ROLE                   = 8,  // root->data 创建角色
    E_LOGIN_MSG_ROOT2LOGIC_CREATE_ROLE                  = 9,  // root->logic 创建角色
    E_LOGIN_MSG_ROOT2DATA_FETCH_ROLE_SNAPSHOT           = 10, // 获取玩家角色快照

    E_LOGIN_MSG_C2S_ENTER_GAME                          = 11,  // 客户端请求进入游戏
    E_LOGIN_MSG_C2S_TEST_ROLE_MSG                       = 12,  // 测试角色相关的消息

    E_LOGIN_MSG_GG2CENTER_ENTER_GAME                    = 101,   // gate->center 进入游戏
    E_LOGIN_MSG_CENTER2ROOT_ENTER_GAME                  = 102,   // center->root 玩家进入游戏
    E_LOGIN_MSG_ROOT2LOGIC_ENTER_GAME                   = 103,   // root->logic 玩家进入游戏
    E_LOGIN_MSG_GATE2LOGIC_ENTER_GAME                   = 104,   // gate->logic 玩家进入游戏

};

#define INT_LOGINMSG(msg_type) static_cast<int32_t>(msg_type)

#endif // __TR_PROTOCOL_LOGIN_H__