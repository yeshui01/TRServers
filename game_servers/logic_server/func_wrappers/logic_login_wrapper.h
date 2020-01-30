/*=======================================================
# Author       : mknight
# Last modified: 2020-01-28 09:28
# Email        : 824338670@qq.com
# Filename     : logic_login_wrapper.h
# Description  : 
# =======================================================*/
#ifndef __TR_LOGIC_LOGIN_WRAPPER_H__
#define __TR_LOGIC_LOGIN_WRAPPER_H__

#include "server_common/server_define.h"
#include "tr_log/log_module.h"

class LogicPlayer;

class LcLoginWrapper
{
public:
    LcLoginWrapper(LogicPlayer *p);
    ~LcLoginWrapper();
    
    /**
     * 创角初始化数据
     */
    void InitCreateData();
protected:
    LogicPlayer *player_;
};

#endif // __TR_LOGIC_LOGIN_WRAPPER_H__
