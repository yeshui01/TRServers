/*=======================================================
# Author       : mknight
# Last modified: 2020-01-27 19:15
# Email        : 824338670@qq.com
# Filename     : data_login_handle.cpp
# Description  : 
# =======================================================*/
#include "data_login_handle.h"

#include "protocol_class.h"
#include "protocol_login.h"
#include "protcl_login.pb.h"

#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
#include "server_common/game_msg_helper.h"
#include "server_common/server_config.h"
#include "protocol_error_code.h"
#include "server_common/client_net_node.h"
#include "data_server/data_player.h"
#include "data_server/data_global.h"
#include "database_tools/database_interface.h"
DataLoginHandler::DataLoginHandler()
{

}

DataLoginHandler::~DataLoginHandler()
{

}

void DataLoginHandler::BindMsgHandle()
{
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_ROOT2DATA_CREATE_ROLE), DataLoginHandler, OnDataCreateRole);
    
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_ROOT2DATA_FETCH_ROLE_SNAPSHOT), DataLoginHandler, OnDataFetchSnapshot);
}

TR_BEGIN_HANDLE_MSG(DataLoginHandler, OnDataCreateRole, E_LOGIN_MSG_ROOT2DATA_CREATE_ROLE)
{
    int64_t acc_id = req.acc_id();
    int64_t role_id = req.role_id();
    const std::string &nickname = req.nickname();
    auto database_pt = g_DataGlobal.db_helper_.HoldDataBase(E_DATABASE_ID_GAME);
    if (!database_pt)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_GAME_DB_NOT_FOUND, rep);
    }
    std::vector<DataTableItem> v_local_set;
    std::stringstream ss_sql;
    ss_sql << "insert into role_base values(" 
        << role_id << ","
        << acc_id << ","
        << "'" << nickname << "'" << ","
        << 0 << ","
        << g_ServerConfig.GetZoneId() << ","
        << 0
        << ")";
    TDEBUG("ss_sql:" << ss_sql.str());
    // database_pt->Query(ss_sql.str(), v_local_set);
    // if (v_local_set.size() > 0)
    // {
    //     SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_NICKNAME_EXISTED, rep);
    // }
    // 插入一条记录
    if (database_pt->ExecuteUpdate(ss_sql.str()) < 1)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_PARAM_ERROR, rep);
    }
    else
    {
        TINFO("insert new role info success,role_id:" << role_id << ", acc_id:" << acc_id);
        // 初始化一个玩家
        DataPlayer* data_player = g_DataGlobal.GetDataPlayer(role_id);
        if (!data_player)
        {
            data_player = new DataPlayer();
            data_player->role_base_.HoldData().SetRoleId(role_id);
            data_player->role_base_.HoldData().SetAccId(acc_id);
            data_player->role_base_.HoldData().SetNickname(nickname);
            g_DataGlobal.AddDataPlayer(role_id, data_player);
        }
    }
}
TR_END_HANDLE_MSG_AND_RETURN_MSG

TR_BEGIN_HANDLE_MSG(DataLoginHandler, OnDataFetchSnapshot, E_LOGIN_MSG_ROOT2DATA_FETCH_ROLE_SNAPSHOT)
{
    int64_t acc_id = req.acc_id();
    auto database_pt = g_DataGlobal.db_helper_.HoldDataBase(E_DATABASE_ID_GAME);
    if (!database_pt)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_GAME_DB_NOT_FOUND, rep);
    }
    std::vector<DataTableItem> v_local_set;
    std::stringstream ss_sql;
    ss_sql << "select role_id, nickname from role_base where acc_id = " << acc_id << ";";
    TDEBUG("sql:" << ss_sql.str());
    database_pt->Query(ss_sql.str(), v_local_set);
    for (auto & one : v_local_set)
    {
        auto pb_snapshot = rep.add_role_list();
        pb_snapshot->set_role_id(one.GetFieldBigIntValue(0));
        pb_snapshot->set_nickname(one.GetFieldStringValue(1));
    }
}
TR_END_HANDLE_MSG_AND_RETURN_MSG
