/*=======================================================
# Author       : mknight
# Last modified: 2020-01-27 11:20
# Email        : 824338670@qq.com
# Filename     : logic_player.h
# Description  : 
# =======================================================*/
#ifndef __TR_LOGIC_PLAYER_H__
#define __TR_LOGIC_PLAYER_H__
#include "tr_database/data_table_item.h"
#include "data_tables/data_table_base.h"
#include "data_tables/tb_role_base.h"

struct LogicPlayerTables;
class LogicPlayer
{
public:
	LogicPlayer(LogicPlayerTables * tbs_pt);
	~LogicPlayer();

public:
	// OneTableItemMgr<TbRoleBase, E_DATA_TABLE_ID_ROLE_BASE> role_base_;
	LogicPlayerTables * data_tbs_ = nullptr;	// 玩家数据表的数据
};


#endif // __TR_LOGIC_PLAYER_H__
