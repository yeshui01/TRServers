/*=======================================================
# Author       : mknight
# Last modified: 2020-01-26 15:04
# Email        : 824338670@qq.com
# Filename     : data_table_base.h
# Description  : 
# =======================================================*/
#ifndef __DATA_TABLE_BASE_H__
#define __DATA_TABLE_BASE_H__

#include "tr_database/data_table_item.h"
#include "tr_common/base_type_def.h"
// 数据表id
enum EDataTableID
{
    E_DATA_TABLE_ID_GAME_START = 100,
    E_DATA_TABLE_ID_ROLE_BASE,


    E_DATA_TABLE_ID_END
};
// 数据表项基类
class DataTableBase : public DataTableItem
{
public:
    DataTableBase(int32_t table_id);
    virtual ~DataTableBase();
    virtual void Serialize();
    virtual void UnSerialize();
};


#endif // __DATA_TABLE_BASE_H__
