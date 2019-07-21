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
    E_PROTOCOL_ERR_CORRECT = 1,     // 正常
    E_PROTOCOL_ERR_PB_PARSE_ERROR = 2,  // 协议解析异常
};

#define INT_PROTOERR(err_code) static_cast<int32_t>(EProtocolErrorCode::err_code)

#endif // __PROTOCOL_ERROR_CODE_H__