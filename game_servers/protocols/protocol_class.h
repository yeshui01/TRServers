/*=======================================================
# Author       : mknight
# Last modified: 2019-06-01 13:27
# Email        : 824338670@qq.com
# Filename     : protocol_class.h
# Description  : 
# =======================================================*/

#ifndef __TR_PROTOCOL_CLASS_H__
#define __TR_PROTOCOL_CLASS_H__

#include "base_type_def.h"

enum class EProtocolClass : int32_t
{
    E_PROTOCOL_CLASS_FRAME = 1,     // 框架类别的协议
    E_PROTOCOL_CLASS_LOGIN = 2,     // 登录类别的协议
    E_PROTOCOL_CALSS_MAX            // 协议类别最大临界值
};

#endif // __TR_PROTOCOL_CLASS_H__
