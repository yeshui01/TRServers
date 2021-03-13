/*=======================================================
# Author       : mknight
# Last modified: 2019-09-30 10:30
# Email        : 824338670@qq.com
# Filename     : data_table_item.cpp
# Description  : 
# =======================================================*/
#include "data_table_item.h"
#include <stdexcept>

DataField::DataField(EFieldValueType type) : val_type(type)
{
    InitValue();
}
DataField::DataField(const DataField &rfield)
{
    val_type = rfield.val_type;
    field_val = rfield.field_val;
    field_index = rfield.field_index;
    primary = rfield.primary;
    if (E_FIELD_VALUE_TYPE_STRING == val_type)
    {
        field_val.val_str = new std::string();
        field_val.val_str->assign(rfield.field_val.val_str->c_str(), rfield.field_val.val_str->size());
    }
    val_changed = true;
}
void DataField::InitValue()
{
    if (val_type == E_FIELD_VALUE_TYPE_STRING)
    {
        field_val.val_str = new std::string("");
    }
    else
    {
        field_val.val_int = 0;
        field_val.val_smint = 0;
        field_val.val_bigint = 0;
    }
}

DataField::~DataField()
{
    if (val_type == E_FIELD_VALUE_TYPE_STRING)
    {
        if (field_val.val_str)
        {
            delete field_val.val_str;
            field_val.val_str = nullptr;
        }
    }
}

DataTableItem::DataTableItem(int32_t table_id)
{
    table_id_  = table_id;
}
DataTableItem::~DataTableItem()
{

}
// 添加表字段值,返回字段的索引
int8_t DataTableItem::AddField(EFieldValueType val_type, bool is_primary)
{
    DataField one_field(val_type);
    one_field.primary = is_primary;
    one_field.field_index = static_cast<int8_t>(fields_.size());
    fields_.push_back(one_field);
    return one_field.field_index;
}
// 设置字段的值
bool DataTableItem::SetFieldBoolValue(int8_t field_index, bool val)
{
    auto field_ptr = HoldField(field_index);
    if (!field_ptr)
    {
        return false;
    }
    field_ptr->field_val.val_bool = val;
    field_ptr->val_changed = true;
    if (E_DB_STATUS_NONE == db_status_)
    {
        SetDbStatus(E_DB_STATUS_UPDATE);
    }
    return true;
}

bool DataTableItem::SetFieldSmallIntValue(int8_t field_index, int16_t val)
{
    auto field_ptr = HoldField(field_index);
    if (!field_ptr)
    {
        return false;
    }
    field_ptr->field_val.val_smint = val;
    field_ptr->val_changed = true;
    if (E_DB_STATUS_NONE == db_status_)
    {
        SetDbStatus(E_DB_STATUS_UPDATE);
    }
    return true;
}

bool DataTableItem::SetFieldIntValue(int8_t field_index, int32_t val)
{
    auto field_ptr = HoldField(field_index);
    if (!field_ptr)
    {
        return false;
    }
    field_ptr->field_val.val_int = val;
    field_ptr->val_changed = true;
    if (E_DB_STATUS_NONE == db_status_)
    {
        SetDbStatus(E_DB_STATUS_UPDATE);
    }
    return true;
}

bool DataTableItem::SetFieldBigIntValue(int8_t field_index, int64_t val)
{
    auto field_ptr = HoldField(field_index);
    if (!field_ptr)
    {
        return false;
    }
    field_ptr->field_val.val_bigint = val;
    field_ptr->val_changed = true;
    if (E_DB_STATUS_NONE == db_status_)
    {
        SetDbStatus(E_DB_STATUS_UPDATE);
    }
    return true;
}

bool DataTableItem::SetFieldFloatValue(int8_t field_index, float val)
{
    auto field_ptr = HoldField(field_index);
    if (!field_ptr)
    {
        return false;
    }
    field_ptr->field_val.val_f = val;
    field_ptr->val_changed = true;
    if (E_DB_STATUS_NONE == db_status_)
    {
        SetDbStatus(E_DB_STATUS_UPDATE);
    }
    return true;
}

bool DataTableItem::SetFieldStringValue(int8_t field_index, const char* str, int32_t len)
{
    if (!str || len < 1)
        return false;
    auto field_ptr = HoldField(field_index);
    if (!field_ptr)
    {
        return false;
    }
    if (field_ptr->field_val.val_str)
    {
        field_ptr->field_val.val_str->assign(str, len);
    }
    else
    {
        field_ptr->field_val.val_str = new std::string(str, len);
    }
    field_ptr->val_changed = true;
    if (E_DB_STATUS_NONE == db_status_)
    {
        SetDbStatus(E_DB_STATUS_UPDATE);
    }
    return true;
}
bool DataTableItem::SetFieldStringValue(int8_t field_index, const std::string & str)
{
    auto field_ptr = HoldField(field_index);
    if (!field_ptr)
    {
        return false;
    }
    if (field_ptr->field_val.val_str)
    {
        field_ptr->field_val.val_str->assign(str.c_str(), str.size());
        // delete field_ptr->field_val.val_str;
        // field_ptr->field_val.val_str = new std::string(str);
    }
    else
    {
        field_ptr->field_val.val_str = new std::string(str);
    }
    field_ptr->val_changed = true;
    if (E_DB_STATUS_NONE == db_status_)
    {
        SetDbStatus(E_DB_STATUS_UPDATE);
    }
    return true;
}
const DataField * DataTableItem::GetField(int32_t field_index)
{
    if (field_index >= 0 && field_index < int32_t(fields_.size()))
    {
        return &fields_[field_index];
    }
    return nullptr;
}
const DataField * DataTableItem::GetField(int32_t field_index) const
{
    if (field_index >= 0 && field_index < int32_t(fields_.size()))
    {
        return &fields_[field_index];
    }
    return nullptr;
}
DataField * DataTableItem::HoldField(int32_t field_index)
{
    if (field_index >= 0 && field_index < int32_t(fields_.size()))
    {
        return &fields_[field_index];
    }
    return nullptr;
}

bool DataTableItem::GetFieldBoolValue(int8_t field_index)
{
    auto val_ptr = GetField(field_index);
    if (val_ptr)
        return val_ptr->field_val.val_bool;
    
    return false;
}

bool DataTableItem::GetFieldBoolValue(int8_t field_index) const
{
    auto val_ptr = GetField(field_index);
    if (val_ptr)
        return val_ptr->field_val.val_bool;
    
    return false;
}

int16_t DataTableItem::GetFieldSmallIntValue(int8_t field_index)
{
    auto val_ptr = GetField(field_index);
    if (val_ptr)
        return val_ptr->field_val.val_smint;
    return 0;
}

int16_t DataTableItem::GetFieldSmallIntValue(int8_t field_index) const
{
    auto val_ptr = GetField(field_index);
    if (val_ptr)
        return val_ptr->field_val.val_smint;
    return 0;
}

int32_t DataTableItem::GetFieldIntValue(int8_t field_index)
{
    auto val_ptr = GetField(field_index);
    if (val_ptr)
        return val_ptr->field_val.val_int;
    return 0;
}
int32_t DataTableItem::GetFieldIntValue(int8_t field_index) const
{
    auto val_ptr = GetField(field_index);
    if (val_ptr)
    {
        if (val_ptr->val_type == E_FIELD_VALUE_TYPE_BIG_INT)
        {
            return val_ptr->field_val.val_bigint;
        }
        else if (val_ptr->val_type == E_FIELD_VALUE_TYPE_INT)
        {
            return val_ptr->field_val.val_int;
        }
    }
    return 0;
}

int64_t DataTableItem::GetFieldBigIntValue(int8_t field_index)
{
    auto val_ptr = GetField(field_index);
    if (val_ptr)
        return val_ptr->field_val.val_bigint;
    return 0;
}
int64_t DataTableItem::GetFieldBigIntValue(int8_t field_index) const 
{
    auto val_ptr = GetField(field_index);
    if (val_ptr)
    {
        if (val_ptr->val_type == E_FIELD_VALUE_TYPE_BIG_INT)
        {
            return val_ptr->field_val.val_bigint;
        }
        else if (val_ptr->val_type == E_FIELD_VALUE_TYPE_INT)
        {
            return val_ptr->field_val.val_int;
        }
    }
        
    return 0;
}

float DataTableItem::GetFieldFloatValue(int8_t field_index)
{
    auto val_ptr = GetField(field_index);
    if (val_ptr)
        return val_ptr->field_val.val_f;
    return 0.0f;
}

float DataTableItem::GetFieldFloatValue(int8_t field_index) const
{
    auto val_ptr = GetField(field_index);
    if (val_ptr)
        return val_ptr->field_val.val_f;
    return 0.0f;
}

const std::string & DataTableItem::GetFieldStringValue(int8_t field_index)
{
    auto val_ptr = GetField(field_index);
    if (!val_ptr)
    {
        throw std::invalid_argument("have no field value");
    }
    else if (val_ptr->field_val.val_str == nullptr)
    {
         throw std::invalid_argument("field have no string value");
    }
    
    return *(val_ptr->field_val.val_str);
}

const std::string & DataTableItem::GetFieldStringValue(int8_t field_index) const
{
    auto val_ptr = GetField(field_index);
    if (!val_ptr)
    {
        throw std::invalid_argument("have no field value");
    }
    else if (val_ptr->field_val.val_str == nullptr)
    {
        throw std::invalid_argument("field have no string value");
    }
    
    return *(val_ptr->field_val.val_str);
}

const std::vector<DataField> & DataTableItem::GetAllFields()
{
    return fields_;
}

const std::vector<DataField> & DataTableItem::GetAllFields() const
{
    return fields_;
}

EDbStatus DataTableItem::GetDbStatus()
{
    return db_status_;
}

EDbStatus DataTableItem::GetDbStatus() const 
{
    return db_status_;
}

int32_t DataTableItem::TableId()
{
    return table_id_;
}

int32_t DataTableItem::TableId() const
{
    return table_id_;
}

size_t DataTableItem::GetFieldsSize()
{
    return fields_.size();
}

size_t DataTableItem::GetFieldsSize() const
{
    return fields_.size();
}

void DataTableItem::SetDbStatus(EDbStatus db_status, bool force)
{
    if (force)
    {
        db_status_ = db_status;
        return;
    }
    if (E_DB_STATUS_NONE == db_status_)
    {
        db_status_ = db_status;
    }
    else if (E_DB_STATUS_INSERT == db_status_)
    {
       if (E_DB_STATUS_DELETE == db_status)
       {
           db_status_ = db_status;
       }
    }
    else if (E_DB_STATUS_DELETE == db_status_)
    {
        if (E_DB_STATUS_INSERT == db_status)
        {
            db_status_ = db_status;
            // db_status_ = E_DB_STATUS_UPDATE;
        }
    }
    else if (E_DB_STATUS_UPDATE == db_status_)
    {
        db_status_ = db_status;
    }
}

void DataTableItem::ClearDbStatus()
{
    db_status_ = E_DB_STATUS_NONE;
    for (auto & v : fields_)
    {
        v.val_changed = false;
    }
}

void DataTableItem::SwapFields(DataTableItem & tb_item)
{
    fields_.swap(tb_item.fields_);
}
void DataTableItem::UpdateFieldsValueByItem(const DataTableItem & tb_item)
{
    for (size_t i = 0; i < fields_.size(); ++i)
    {
        if (i < tb_item.fields_.size())
        {
            if (E_FIELD_VALUE_TYPE_STRING == tb_item.fields_[i].val_type)
            {
                if (fields_[i].field_val.val_str == nullptr)
                {
                    fields_[i].field_val.val_str = new std::string();
                }
                
                fields_[i].field_val.val_str->assign(tb_item.fields_[i].field_val.val_str->c_str(), tb_item.fields_[i].field_val.val_str->size());
            }
            else
            {
                fields_[i].field_val = tb_item.fields_[i].field_val;
            }
            
            fields_[i].val_type = tb_item.fields_[i].val_type;
            
            fields_[i].field_index = tb_item.fields_[i].field_index;
            // primary = rfield.primary;
            
            fields_[i].val_changed = true;
        }
    }
}
bool DataTableItem::FieldsChange()
{
    bool update = false;
    for (auto & v : fields_)
    {
        if (v.val_changed)
        {
            update = true;
            break;
        }
    }
    return update;
}
void DataTableItem::SetFieldChange(int32_t field_index)
{
    auto field_info = HoldField(field_index);
    if (field_info)
    {
        field_info->val_changed = true;
        if (E_DB_STATUS_NONE == db_status_)
        {
            SetDbStatus(E_DB_STATUS_UPDATE);
        }
    }
}
bool DataTableItem::FieldsChange() const
{
    bool update = false;
    for (auto & v : fields_)
    {
        if (v.val_changed)
        {
            update = true;
            break;
        }
    }
    return update;
}