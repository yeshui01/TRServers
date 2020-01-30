/*=======================================================
# Author       : mknight
# Last modified: 2020-01-26 20:29
# Email        : 824338670@qq.com
# Filename     : data_player.h
# Description  : 
# =======================================================*/
#ifndef __TR_DATA_PLAYER_H__
#define __TR_DATA_PLAYER_H__
#include "tr_database/data_table_item.h"
#include "data_tables/data_table_base.h"
#include "data_tables/tb_role_base.h"
class DataPlayer
{
public:
    DataPlayer();
    ~DataPlayer();

public:
    OneTableItemMgr<TbRoleBase, E_DATA_TABLE_ID_ROLE_BASE> role_base_;
};

#endif // __TR_DATA_PLAYER_H__
