/*=======================================================
# Author       : mknight
# Last modified: 2019-07-20 19:10
# Email        : 824338670@qq.com
# Filename     : protocol_error_code.h
# Description  : 
# =======================================================*/

#ifndef __PROTOCOL_ERROR_CODE_H__
#define __PROTOCOL_ERROR_CODE_H__

#include "base_type_def.h"

enum class EProtocolErrorCode : int32_t
{
    E_PROTOCOL_ERR_CORRECT = 1,             // 正常
    E_PROTOCOL_ERR_PB_PARSE_ERROR = 2,      // 协议解析异常
    E_PROTOCOL_ERR_NO_DATABASE = 3,         // 没有对应的数据库 
    E_PROTOCOL_ERR_ACCNAME_EXISTED = 4,     // 账号名字已经存在
    E_PROTOCOL_ERR_PARAM_ERROR = 5,         // 参数错误
    E_PROTOCOL_ERR_ACCOUNT_PSWD_ERROR = 6,  // 账号密码错误
    E_PROTOCOL_ERR_ACCOUNT_NOT_EXISTED = 7, // 账号不存在
    E_PROTOCOL_ERR_ACCOUNT_LOGINGED = 8,    // 账号已经登录
    E_PROTOCOL_ERR_EXCEPTION = 9,           // 异常错误
    E_PROTOCOL_ERR_INNER_ERROR = 10,        // 内部错误
    E_PROTOCOL_ERR_INVALID_SESSION = 11,    // 非法的连接
    E_PROTOCOL_ERR_ACCOUNT_UNLOGIN = 12,    // 账号未登录
    E_PROTOCOL_ERR_ACCOUNT_STATUS = 13,     // 账号状态异常
    E_PROTOCOL_ERR_NICKNAME_EXISTED = 14,   // 角色名已存在
    E_PROTOCOL_ERR_GAME_DB_NOT_FOUND = 15,  // 没有游戏服务器数据库
    E_PROTOCOL_ERR_ACCOUNT_DB_NOT_FOUND = 16,  // 没有账号数据库
    E_PROTOCOL_ERR_ROLE_ONLINE = 17,          // 角色已经在线
    E_PROTOCOL_ERR_NOT_FOUND_PLAYER = 18,      // 找不到玩家数据
    E_PROTOCOL_ERR_ACCOUNT_ROLE_NOT_MATCH = 19,// 角色账号信息不匹配
};

#define INT_PROTOERR(err_code) static_cast<int32_t>(EProtocolErrorCode::err_code)

#endif // __PROTOCOL_ERROR_CODE_H__