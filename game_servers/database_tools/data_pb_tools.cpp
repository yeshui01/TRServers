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
    void DumpToOnePbTbItem(const DataTableItem * tb_item, db_protos::pb_table_item * pb_item)
    {
        if (!tb_item || !pb_item)
            return;
        
        auto & v_fields = tb_item->GetAllFields();
        for (auto & v : v_fields)
        {
            pb_item->set_db_status(tb_item->GetDbStatus());
            auto pb_field = pb_item->add_field_list();
            pb_field->set_val_type(v.val_type);
            pb_field->set_field_index(v.field_index);
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
};
