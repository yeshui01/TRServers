/*=======================================================
# Author       : mknight
# Last modified: 2020-01-29 15:58
# Email        : 824338670@qq.com
# Filename     : logic_table_service.cpp
# Description  : 
# =======================================================*/

#include "protocol_class.h"
#include "protocol_frame.h"

#include "logic_table_service.h"
#include "logic_player.h"
#include "pb_srcfiles/data_pb_common.pb.h"
#include "pb_srcfiles/protcl_frame.pb.h"
#include "tr_common/common_define.h"
#include "server_common/game_msg_helper.h"

#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
#include "logic_server/logic_server.h"
#include "protocol_error_code.h"
#include "tr_timer/server_time.h"
#include "server_common/server_config.h"
#include "database_tools/data_pb_tools.h"

LogicTableService::LogicTableService()
{
}

LogicTableService::~LogicTableService()
{
}

void LogicTableService::Update(time_t cur_time)
{
    if (cur_time - last_update_time_ < 3)
    {
        return;
    }
    for (auto it = player_data_tbs_.begin(); it != player_data_tbs_.end(); ++it)
    {
        SavePlayerData(*(it->second.get()));
    }

    last_update_time_ = cur_time;
}

LogicPlayerTables * LogicTableService::CreatePlayerTables(int64_t role_id)
{
    std::shared_ptr<LogicPlayerTables> one_player_data(new LogicPlayerTables);
    one_player_data->role_id = role_id;
    auto ret = player_data_tbs_.insert(std::make_pair(role_id, one_player_data));
    if (ret.second)
    {
        return ret.first->second.get();
    }
    return nullptr;
}

LogicPlayerTables * LogicTableService::HoldPlayerTables(int64_t role_id)
{
    auto it = player_data_tbs_.find(role_id);
    return it != player_data_tbs_.end() ? it->second.get() : nullptr;
}

void LogicTableService::SavePlayerData(LogicPlayerTables & player_tbs)
{
    REQMSG(E_FRAME_MSG_XS_TO_DATA_SAVE_PLAYER_TABLES) data_req;
    data_req.set_role_id(player_tbs.role_id);
    // role_base
    if (player_tbs.role_base.GetData().GetDbStatus() != E_DB_STATUS_NONE)
    {
        auto & role_base = player_tbs.role_base.HoldData();
        role_base.Serialize();
        auto pb_table = data_req.add_table_list();
        pb_table->set_table_id(role_base.TableId());
        DbProtoTools::DumpToOnePbTbItem(&role_base, pb_table->add_data_items());
        // 清理数据状态
        role_base.ClearDbStatus();
    }
    if (data_req.table_list_size() > 0)
    {
        // send to data
        TDEBUG("E_FRAME_MSG_XS_TO_DATA_SAVE_PLAYER_TABLES_req:" << data_req.ShortDebugString());
        g_MsgHelper.ForwardPbMessage(
            INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
            INT_FRAMEMSG(E_FRAME_MSG_XS_TO_DATA_SAVE_PLAYER_TABLES),
            data_req,
            EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA,
            0);
    }
}

void LogicTableService::LoadPlayerTableFromPbMsg(LogicPlayerTables & player_tbs, protos::rep_E_FRAME_MSG_XS_TO_DATA_LOAD_PLAYER_TABLES * rep_msg)
{
    if (!rep_msg)
    {
        return;
    }
    for (int32_t i = 0; i < rep_msg->table_list_size(); ++i)
    {
        auto pb_table = rep_msg->mutable_table_list(i);
        if (pb_table->table_id() == E_DATA_TABLE_ID_ROLE_BASE)
        {
            // role_base更新
            auto &role_base = player_tbs.role_base.HoldData();
            if (pb_table->data_items_size() > 0)
            {
                DbProtoTools::LoadFromOnePbTbItem(&role_base, pb_table->mutable_data_items(0), false);
                role_base.UnSerialize();
                role_base.ClearDbStatus();
            }
        }
    }
    
}
