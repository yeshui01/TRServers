/*=======================================================
# Author       : mknight
# Last modified: 2019-10-04 15:25
# Email        : 824338670@qq.com
# Filename     : login_global.h
# Description  : 
# =======================================================*/
#ifndef __TR_LOGIN_GLOBAL_H__
#define __TR_LOGIN_GLOBAL_H__

#include "tr_common/singleton.h"
#include "tr_database/database_helper.h"

class LoginGlobal : public TSingleton<LoginGlobal>
{
public:
    LoginGlobal();
    ~LoginGlobal();

    bool Init();
    void Stop();
public:
    TRDataBaseHelper db_helper_;    // 数据库管理助手
};

#define g_LoginGlobal LoginGlobal::Instance()

#endif // __TR_LOGIN_GLOBAL_H__
