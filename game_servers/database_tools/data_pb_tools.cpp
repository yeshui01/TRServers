/*=======================================================
# Author       : mknight
# Last modified: 2019-09-30 15:34
# Email        : 824338670@qq.com
# Filename     : data_pb_tools.cpp
# Description  : 
# =======================================================*/

#include "tr_database/data_table_item.h"
#include "data_pb_common.pb.h"

namespace DbProtoTools
{
    void DumpToOnePbTbItem(const DataTableItem * tb_item, protos::pb_table_item * pb_item, bool incre)
    {
        if (!tb_item || !pb_item)
            return;
        pb_item->set_db_status(tb_item->GetDbStatus());
        auto & v_fields = tb_item->GetAllFields();
        for (auto & v : v_fields)
        {
            if (incre && !v.val_changed)
            {
                // 增量保存,没有变化的不用保存
                continue;
            }
            auto pb_field = pb_item->add_field_list();
            pb_field->set_val_type(v.val_type);
            pb_field->set_field_index(v.field_index);
            pb_field->set_primary(v.primary);
            switch (v.val_type)
            {
                case E_FIELD_VALUE_TYPE_BOOL:
                {
                    pb_field->set_val_bool(v.field_val.val_bool);
                    break;
                }
                case E_FIELD_VALUE_TYPE_SMALL_INT:
                {
                    pb_field->set_val_int(v.field_val.val_smint);
                    break;
                }
                case E_FIELD_VALUE_TYPE_INT:
                {
                    pb_field->set_val_int(v.field_val.val_int);
                    break;
                }
                case E_FIELD_VALUE_TYPE_BIG_INT:
                {
                    pb_field->set_val_bigint(v.field_val.val_bigint);
                    break;
                }
                case E_FIELD_VALUE_TYPE_FLOAT:
                {
                    pb_field->set_val_f(v.field_val.val_f);
                    break;
                }
                case E_FIELD_VALUE_TYPE_STRING:
                {
                    pb_field->set_val_str(*(v.field_val.val_str));
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
    void LoadFromOnePbTbItem(DataTableItem * tb_item, const protos::pb_table_item * pb_item, bool incre)
    {
        if (!tb_item || !pb_item)
            return;
        
        tb_item->SetDbStatus(static_cast<EDbStatus>(pb_item->db_status()));
        for (int32_t i = 0; i < pb_item->field_list_size(); ++i)
        {
            auto & pb_field = pb_item->field_list(i);
            EFieldValueType field_type = static_cast<EFieldValueType>(pb_field.val_type());
            int8_t field_index = pb_field.field_index();
            if (!incre)
            {
                tb_item->AddField(field_type, pb_field.primary());
            }
            
            switch (field_type)
            {
                case E_FIELD_VALUE_TYPE_BOOL:
                {
                    tb_item->SetFieldBoolValue(field_index, pb_field.val_bool());     
                    break;
                }
                case E_FIELD_VALUE_TYPE_SMALL_INT:
                {
                    tb_item->SetFieldSmallIntValue(field_index, pb_field.val_int()); 
                    break;
                }
                case E_FIELD_VALUE_TYPE_INT:
                {
                    tb_item->SetFieldIntValue(field_index, pb_field.val_int()); 
                    break;
                }
                case E_FIELD_VALUE_TYPE_BIG_INT:
                {
                    tb_item->SetFieldBigIntValue(field_index, pb_field.val_bigint()); 
                    break;
                }
                case E_FIELD_VALUE_TYPE_FLOAT:
                {
                    tb_item->SetFieldFloatValue(field_index, pb_field.val_f()); 
                    break;
                }
                case E_FIELD_VALUE_TYPE_STRING:
                {
                    tb_item->SetFieldStringValue(field_index, pb_field.val_str().c_str(), pb_field.val_str().size()); 
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }

    template<class TbItemType, class key_type, int32_t table_id>
    void LoadFromPbTableItems(DataTableItemMgr<TbItemType, key_type, table_id> & tbitem_mgr, protos::pb_data_module *pb_data_module, bool incre)
    {
        if (!pb_data_module)
        {
            return;
        }
        for (int32_t i = 0; i < pb_data_module->data_items_size(); ++i)
        {
            // auto & pb_item = pb_data_module->data_items(i);
            // TODO:
        }
    }

    template<class TbItemType, class key_type, int32_t table_id>
    void DumpToPbTableItems(const DataTableItemMgr<TbItemType, key_type, table_id> & tbitem_mgr, protos::pb_data_module *pb_data_module, bool incre)
    {
        if (!pb_data_module)
            return;
        pb_data_module->set_table_id(table_id);
        tbitem_mgr.ForEach([pb_module{pb_data_module}, incre](const TbItemType * item_info)
        {
            auto pb_item = pb_module->add_data_items();
            DumpToOnePbTbItem(item_info, pb_item, incre);
        });
    }
};
