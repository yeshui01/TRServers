
/*=======================================================
# Author       : mknight
# Last modified: 2020-01-30 10:04
# Email        : 824338670@qq.com
# Filename     : data_table_service.h
# Description  : 
# =======================================================*/

#ifndef __TR_DATA_TABLE_SERVICE_H__
#define __TR_DATA_TABLE_SERVICE_H__

#include "tr_common/base_type_def.h"
#include "tr_common/singleton.h"

#include "tr_database/data_table_item.h"
#include "data_tables/data_table_base.h"

#include <map>
#include <utility>
#include <memory>

class DataPlayer;
namespace sql
{
    class PreparedStatement;
}
namespace protos
{
    class pb_data_module;
    class pb_table_item;
    class rep_E_FRAME_MSG_XS_TO_DATA_LOAD_PLAYER_TABLES;
}
class DataTableItem;
class TRDataBase;

class DataTableService : public TSingleton<DataTableService>
{
public:
    DataTableService();
    ~DataTableService();

    void SaveDataPlayer(DataPlayer * p);
    bool LoadDataPlayerTablesFromDB(int64_t role_id, DataPlayer *p);
    void UpdatePlayerTablesFromPbTables(DataPlayer *p, protos::pb_data_module * pb_table);
    // 填充预处理语句的参数
    bool FillPrestmtParams(sql::PreparedStatement * pre_stmt, const DataTableItem * tb_item);

    void AttatchGameDB(TRDataBase * game_db);
    void DumpPlayerTableToRepMsg(DataPlayer * p, protos::rep_E_FRAME_MSG_XS_TO_DATA_LOAD_PLAYER_TABLES * rep_msg);
protected:
    TRDataBase * game_db_ = nullptr;
};

#define g_DataTableService DataTableService::Instance()

#endif // __TR_DATA_TABLE_SERVICE_H__