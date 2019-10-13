/*=======================================================
# Author       : mknight
# Last modified: 2019-09-30 15:34
# Email        : 824338670@qq.com
# Filename     : data_pb_tools.h
# Description  : 
# =======================================================*/
#ifndef __DATA_PB_TOOLS_H__
#define __DATA_PB_TOOLS_H__

#include "tr_database/data_table_item.h"
#include <functional>
namespace db_protos
{
    class pb_data_module;
    class pb_table_item;
    class pb_table_field;
}


namespace DbProtoTools
{
    // 转储一条db数据到pb数据
    void DumpToOnePbTbItem(const DataTableItem * tb_item, db_protos::pb_table_item * pb_item);

    // 转储一个表的数据到pb_module
    template<class TbItemType, class key_type, int32_t table_id>
    void DumpToPbTableItems(const DataTableItemMgr<TbItemType, key_type, table_id> & tbitem_mgr, db_protos::pb_data_module *pb_data_module)
    {
        if (!pb_data_module)
            return;
        pb_data_module->set_table_id(table_id);
        tbitem_mgr.ForEach([pb_module{pb_data_module}](const TbItemType * item_info)
        {
            auto pb_item = pb_module->add_data_items();
            DumpToOnePbTbItem(item_info, pb_item);
        });
    }

    // 增量转储数据到pb数据TODO:
};

#endif // __DATA_PB_TOOLS_H__

