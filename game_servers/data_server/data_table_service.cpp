/*=======================================================
# Author       : mknight
# Last modified: 2020-01-30 10:04
# Email        : 824338670@qq.com
# Filename     : data_table_service.cpp
# Description  : 
# =======================================================*/

#include "data_table_service.h"
#include "data_global.h"
#include "data_tables/tb_role_base.h"
#include "tr_log/log_module.h"
#include "db_stmt_interface.h"
#include "database_tools/database_interface.h"
#include "data_player.h"
#include "database_tools/data_pb_tools.h"

#include "pb_srcfiles/data_pb_common.pb.h"

DataTableService::DataTableService()
{

}

DataTableService::~DataTableService()
{

}

void DataTableService::SaveDataPlayer(DataPlayer * p)
{
    if (!p)
    {
        return;
    }
    // 从数据库加载
    auto game_db = g_DataGlobal.db_helper_.HoldDataBase(E_DATABASE_ID_GAME);
    if (!game_db)
    {
        TERROR("not found game_db");
        return;
    }
    {
        // role base
        auto & role_base = p->role_base_.HoldData();
        if (E_DB_STATUS_UPDATE == role_base.GetDbStatus())
        {
            auto pre_stmt = game_db->HoldMainPreStmt(E_PRE_STMT_ROLE_BASE_UPDATE);
            // 更新
            if (!pre_stmt)
            {
                TERROR("not found role base update prestmt:");
                return;
            }
            FillPrestmtParams(pre_stmt, &role_base);
            pre_stmt->executeUpdate();  // 更新数据库
            TINFO("update table role_base, role_id:" << role_base.GetRoleId());
            role_base.ClearDbStatus();
        }
    }
}

bool DataTableService::LoadDataPlayerTablesFromDB(int64_t role_id, DataPlayer *p)
{
    if (role_id < 0 || !p)
    {
        return false;
    }
    // 从数据库加载
    auto game_db = g_DataGlobal.db_helper_.HoldDataBase(E_DATABASE_ID_GAME);
    if (!game_db)
    {
        TERROR("not found game_db");
        return false;
    }
    std::stringstream ss_sql;
    std::vector<DataTableItem> v_local_set;
    {
        // role_base
        ss_sql << "select * from role_base where role_id = " << role_id;
        game_db->Query(ss_sql.str(), v_local_set);
        if (v_local_set.size() > 0)
        {
            auto & role_base = p->role_base_.HoldData();
            role_base.SwapFields(v_local_set[0]);
        }
        else
        {
            TERROR("not found role base:" << role_id);
            return false;
        }
    }
    return true;
}

bool DataTableService::FillPrestmtParams(sql::PreparedStatement * pre_stmt, 
    const DataTableItem * tb_item)
{
    if (!pre_stmt || !tb_item)
    {
        return false;
    }
    auto & v_fields = tb_item->GetAllFields();
    int32_t param_index = 0;
    std::vector<const DataField *> v_param_fields;
    std::vector<const DataField *> v_key_fields;
    for (auto & one_field : v_fields)
    {
        if (one_field.primary)
        {
            v_key_fields.push_back(&one_field);
        }
        else
        {
            v_param_fields.push_back(&one_field);
        }
    }
    v_param_fields.insert(v_param_fields.end(), v_key_fields.begin(), v_key_fields.end());
    for (auto & one_field : v_param_fields)
    {
        ++param_index;
        try
        {
            switch (one_field->val_type)
            {
            case E_FIELD_VALUE_TYPE_BOOL:
            {
                pre_stmt->setBoolean(param_index, one_field->field_val.val_bool);
                break;
            }
            case E_FIELD_VALUE_TYPE_SMALL_INT:
            {
                pre_stmt->setInt(param_index, one_field->field_val.val_smint);
                break;
            }
            case E_FIELD_VALUE_TYPE_INT:
            {
                pre_stmt->setInt(param_index, one_field->field_val.val_int);
                break;
            }
            case E_FIELD_VALUE_TYPE_BIG_INT:
            {
                pre_stmt->setInt64(param_index, one_field->field_val.val_bigint);
                break;
            }
            case E_FIELD_VALUE_TYPE_FLOAT:
            {
                pre_stmt->setDouble(param_index, one_field->field_val.val_f);
                break;
            }
            case E_FIELD_VALUE_TYPE_STRING:
            {
                pre_stmt->setString(param_index, *(one_field->field_val.val_str));
                break;
            }
            default:
            {
                TERROR("unhandled val type:" << one_field->val_type);
                break;
            }
            }
        }
        catch (sql::SQLException &e)
        {
            TERROR("mysql err_code:" << e.getErrorCode() << ", err:" << e.what() << ", sql_state:" << e.getSQLState()
                << ", param_index:" << param_index);
            
            break;
        }
    }
    return true;
}

void DataTableService::AttatchGameDB(TRDataBase * game_db)
{
    game_db_ = game_db;
}

void DataTableService::UpdatePlayerTablesFromPbTables(DataPlayer *p, protos::pb_data_module * pb_table)
{
    if (!p || !pb_table)
    {
        return;
    }
    if (pb_table->table_id() == E_DATA_TABLE_ID_ROLE_BASE)
    {
        // role_base更新
        auto & role_base = p->role_base_.HoldData();
        if (pb_table->data_items_size() > 0)
        {
            DbProtoTools::LoadFromOnePbTbItem(&role_base, pb_table->mutable_data_items(0), true);
        }
    }

    // SaveDataPlayer(p);
}