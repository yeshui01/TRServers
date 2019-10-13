/*=======================================================
# Author       : mknight
# Last modified: 2019-10-04 15:25
# Email        : 824338670@qq.com
# Filename     : login_global.cpp
# Description  : 
# =======================================================*/
#include "login_global.h"
#include "tr_log/log_module.h"
#include "tr_common/common_define.h"
#include "server_common/server_config.h"
#include "database_tools/database_interface.h"
LoginGlobal::LoginGlobal()
{

}

LoginGlobal::~LoginGlobal()
{

}

bool LoginGlobal::Init()
{
    TDEBUG("LoginGlobal::Init");
    // 连接数据库
    std::shared_ptr<TRDataBase> acc_database(new TRDataBase());
    auto const & jv_db = g_ServerConfig.GetJsonConfig()["account_db"];
    if (!acc_database->Init(jv_db["host"].asString(), 
        jv_db["user"].asString(), 
        jv_db["pswd"].asString(), 
        jv_db["db_name"].asString(),
        1))
    {
        TERROR("account_db init failed");
        return false;
    }
    acc_database->StartWorkers();
    acc_database->Execute("set NAMES 'utf8'");
    db_helper_.AddDataBase(E_DATABASE_ID_ACCOUNT_1, acc_database);
    return true;
}

void LoginGlobal::Stop()
{
    TINFO("LoginGlobal::Stop");
    db_helper_.Stop();
}