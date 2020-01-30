/*=======================================================
# Author       : mknight
# Last modified: 2020-01-26 20:28
# Email        : 824338670@qq.com
# Filename     : data_global.cpp
# Description  : 
# =======================================================*/
#include "data_global.h"
#include "tr_log/log_module.h"
#include "tr_common/common_define.h"
#include "server_common/server_config.h"
#include "database_tools/database_interface.h"
#include "data_player.h"
#include "db_stmt_interface.h"

DataGlobal::DataGlobal()
{

}

DataGlobal::~DataGlobal()
{
    for (auto it = data_players_.begin(); it != data_players_.end(); ++it)
    {
        SAFE_DELETE_PTR(it->second);
    }
}

bool DataGlobal::Init()
{
    TDEBUG("DataGlobal::Init");
    if (!InitDB())
    {
        return false;
    }
    return true;
}

void DataGlobal::Stop()
{
    TINFO("DataGlobal::Stop");
    db_helper_.Stop();
}

DataPlayer * DataGlobal::GetDataPlayer(int64_t role_id)
{
    auto it = data_players_.find(role_id);
    return it != data_players_.end() ? it->second : nullptr;
}

bool DataGlobal::AddDataPlayer(int64_t role_id, DataPlayer * p)
{
    if (!p)
    {
        return false;
    }
    if (p->role_base_.HoldData().GetRoleId() < 1)
    {
        p->role_base_.HoldData().SetRoleId(role_id);
    }
    auto ret = data_players_.insert({role_id, p});
    return ret.second;
}

bool DataGlobal::InitDB()
{
    TINFO("data_global init db");
    // 连接数据库
    std::shared_ptr<TRDataBase> game_database(new TRDataBase());
    auto const & jv_db = g_ServerConfig.GetJsonConfig()["game_db"];
    if (!game_database->Init(jv_db["host"].asString(), 
        jv_db["user"].asString(), 
        jv_db["pswd"].asString(), 
        jv_db["db_name"].asString(),
        1))
    {
        TERROR("account_db init failed");
        return false;
    }
    game_database->SetConnsUtf8Char();
    game_database->InstallPreStmts([](sql::Connection * conn, TRDBPrepareStmtMgr * stmt_mgr)
    {
        // 安装预处理语句
        {
            // role_base表
            std::shared_ptr<sql::PreparedStatement> prestmt_ptr;
            prestmt_ptr.reset(conn->prepareStatement("insert into role_base values (?, ?, ?, ?, ?, ?)"));
            stmt_mgr->AddPreStmt(E_PRE_STMT_ROLE_BASE_INSET, prestmt_ptr);

            prestmt_ptr.reset(conn->prepareStatement("update role_base set acc_id = ?, nickname = ?, create_time = ?, zone_id = ?,\
            inited = ? where role_id = ?"));
            stmt_mgr->AddPreStmt(E_PRE_STMT_ROLE_BASE_UPDATE, prestmt_ptr);
            // 这个表，业务层不需要删除
        }
        
    });
    
    game_database->StartWorkers();
    db_helper_.AddDataBase(E_DATABASE_ID_GAME, game_database);
    return true;
}

void DataGlobal::FrameUpdate(time_t cur_mtime)
{
    db_helper_.FrameUpdate();
}