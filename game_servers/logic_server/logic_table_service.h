/*=======================================================
# Author       : mknight
# Last modified: 2020-01-29 15:58
# Email        : 824338670@qq.com
# Filename     : logic_table_service.h
# Description  : 数据表的保存和加载 
# =======================================================*/

#ifndef __TR_LOGIC_TABLE_SERVICE_H__
#define __TR_LOGIC_TABLE_SERVICE_H__

#include "tr_common/base_type_def.h"
#include "tr_common/singleton.h"

#include "tr_database/data_table_item.h"
#include "data_tables/data_table_base.h"
#include "data_tables/tb_role_base.h"


#include <map>
#include <utility>
#include <memory>

class LogicPlayer;

// 玩家的数据表
struct LogicPlayerTables
{
    int64_t role_id = 0;
    OneTableItemMgr<TbRoleBase, E_DATA_TABLE_ID_ROLE_BASE> role_base;

};

class LogicTableService : public TSingleton<LogicTableService>
{
public:
    LogicTableService();
    ~LogicTableService();

    void Update(time_t cur_time);
    /**
     * 创建玩家数据表数据模块
     * @param  role_id               : 角色id
     * @return {LogicPlayerTables*}  : 成功返回玩家数据表模块,失败返回null
     */
    LogicPlayerTables * CreatePlayerTables(int64_t role_id);
    LogicPlayerTables * HoldPlayerTables(int64_t role_id);

protected:
    void SavePlayerData(LogicPlayerTables & player_tbs);


protected:
    time_t last_update_time_ = 0;
    std::map<int64_t, std::shared_ptr<LogicPlayerTables> > player_data_tbs_;  // key:role_id value:玩家数据表集合
};

#define g_LogicTableService  LogicTableService::Instance()

#endif // __TR_LOGIC_TABLE_SERVICE_H__