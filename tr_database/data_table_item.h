/*=======================================================
# Author       : mknight
# Last modified: 2019-09-30 10:28
# Email        : 824338670@qq.com
# Filename     : data_table_item.h
# Description  : 
# =======================================================*/
#ifndef __DATA_TABLE_ITEM_H__
#define __DATA_TABLE_ITEM_H__

#include "tr_common/base_type_def.h"

#include <string>
#include <vector>
#include <map>
#include <functional>
enum EFieldValueType : int8_t
{
    E_FIELD_VALUE_TYPE_NONE = 0,        // 无效类型
    E_FIELD_VALUE_TYPE_BOOL = 1,        // bool类型
    E_FIELD_VALUE_TYPE_SMALL_INT = 2,   // 16位整数
    E_FIELD_VALUE_TYPE_INT = 3,         // 32位整数
    E_FIELD_VALUE_TYPE_BIG_INT = 4,     // 64位整数
    E_FIELD_VALUE_TYPE_FLOAT = 5,       // 浮点数
    E_FIELD_VALUE_TYPE_STRING = 6,      // string
};
// 数据状态
enum EDbStatus : int8_t
{
    E_DB_STATUS_NONE = 0,
    E_DB_STATUS_INSERT = 1, // 插入
    E_DB_STATUS_UPDATE = 2, // 更新
    E_DB_STATUS_DELETE = 3, // 删除
};
// 数据库字段
class DataField
{
public:
    DataField() = delete;
    DataField(EFieldValueType type);
    DataField(const DataField &rfield);
    ~DataField();
    void InitValue();
public:
    EFieldValueType val_type;
    union FieldValue
    {
        bool val_bool;
        int16_t val_smint;
        int32_t val_int;
        int64_t val_bigint;
        std::string * val_str;
        float val_f;
    };
    union FieldValue field_val;
    int8_t field_index = 0;
    bool primary = false;  // 是否主键
    bool val_changed = false; // 值是否发生变化
};

// 数据表描述
struct DataFieldDescribe
{
    int8_t field_index_ = 0;
    std::string field_name; // 字段名子
    EFieldValueType val_type = E_FIELD_VALUE_TYPE_NONE;
    bool primary_ = false;  // 是否主键
};

struct DataTableDescribe
{
    std::string table_name;                     // 数据表名
    std::vector<DataFieldDescribe> field_info;  // 字段信息
};

// 数据库表的数据项
class DataTableItem
{
public:
    DataTableItem(int32_t table_id);
    ~DataTableItem();

    // 添加表字段值,返回字段的索引
    int8_t AddField(EFieldValueType val_type, bool is_primary);
    // 设置字段的值
    bool SetFieldBoolValue(int8_t field_index, bool val);
    bool SetFieldSmallIntValue(int8_t field_index, int16_t val);
    bool SetFieldIntValue(int8_t field_index, int32_t val);
    bool SetFieldBigIntValue(int8_t field_index, int64_t val);
    bool SetFieldFloatValue(int8_t field_index, float val);
    bool SetFieldStringValue(int8_t field_index, const char* str, int32_t len);
    bool SetFieldStringValue(int8_t field_index, const std::string & str);
    // 获取字段的值
    bool GetFieldBoolValue(int8_t field_index);
    bool GetFieldBoolValue(int8_t field_index) const;
    int16_t GetFieldSmallIntValue(int8_t field_index);
    int16_t GetFieldSmallIntValue(int8_t field_index) const;
    int32_t GetFieldIntValue(int8_t field_index);
    int32_t GetFieldIntValue(int8_t field_index) const;
    int64_t GetFieldBigIntValue(int8_t field_index);
    int64_t GetFieldBigIntValue(int8_t field_index) const;
    float GetFieldFloatValue(int8_t field_index);
    float GetFieldFloatValue(int8_t field_index) const;
    const std::string & GetFieldStringValue(int8_t field_index);
    const std::string & GetFieldStringValue(int8_t field_index) const;
    const std::vector<DataField> & GetAllFields();
    const std::vector<DataField> & GetAllFields() const;
    EDbStatus GetDbStatus();
    EDbStatus GetDbStatus() const;
    int32_t TableId();
    int32_t TableId() const;
    size_t GetFieldsSize();
    size_t GetFieldsSize() const;
    void SetDbStatus(EDbStatus db_status, bool force = false);
    void ClearDbStatus();
    // 交换字段
    void SwapFields(DataTableItem & tb_item);
    // 设置字段值
    void UpdateFieldsValueByItem(const DataTableItem & tb_item);
    // 字段是否变化
    bool FieldsChange();
    bool FieldsChange() const;
    void SetFieldChange(int32_t field_index);
public:
    // 获取字段数据
    const DataField * GetField(int32_t field_index);
    const DataField * GetField(int32_t field_index) const;
protected:
    DataField * HoldField(int32_t field_index);

protected:
    std::vector<DataField> fields_;
    int32_t table_id_ = 0;
    EDbStatus db_status_ = E_DB_STATUS_NONE;
};

// 数据表项管理器
template<class TbItemType, class key_type, int32_t table_id>
class DataTableItemMgr
{
public:
    DataTableItemMgr()
    {
        table_id_ = table_id;
    }
    ~DataTableItemMgr()
    {
        for (auto it = data_items_.begin(); it != data_items_.end(); ++it)
        {
            delete it->second;
        }
        data_items_.clear();
    }

    TbItemType * HoldItem(key_type key)
    {
        auto it = data_items_.find(key);
        if (it == data_items_.end())
            return nullptr;
        
        if (it->second->GetDbStatus() == E_DB_STATUS_DELETE)
        {
            return nullptr;
        }
        
        it->second->SetDbStatus(E_DB_STATUS_UPDATE);
        return it->second;
    }

    const TbItemType * GetItem(key_type key)
    {
        auto it = data_items_.find(key);
        if (it == data_items_.end())
            return nullptr;
        
        return it->second->GetDbStatus() != E_DB_STATUS_DELETE ? it->second:nullptr;
    }

    const TbItemType * FindItemC(std::function<bool (const TbItemType * data_pt)> && filter)
    {
        const TbItemType * ret = nullptr;
        for (auto it = data_items_.begin(); it != data_items_.end(); ++it)
        {
            if (it->second->GetDbStatus() == E_DB_STATUS_DELETE)
            {
                continue;
            }
            if (filter(it->second))
            {
                ret = it->second;
                break;
            }
        }

        return ret;
    }
    TbItemType * FindItem(std::function<bool (TbItemType * data_pt)> && filter)
    {
        TbItemType * ret = nullptr;
        for (auto it = data_items_.begin(); it != data_items_.end(); ++it)
        {
            if (it->second->GetDbStatus() == E_DB_STATUS_DELETE)
            {
                continue;
            }
            if (filter(it->second))
            {
                ret = it->second;
                break;
            }
        }

        return ret;
    }

    TbItemType * AddItem(key_type key)
    {
        auto it = data_items_.find(key);
        if (it != data_items_.end())
        {
            if (it->second->GetDbStatus() == E_DB_STATUS_DELETE)
            {
                // 覆盖
                it->second->SetDbStatus(E_DB_STATUS_UPDATE);
                return it->second;
            }
            else
            {
                // 已经存在了
                return nullptr;   
            }
        }
        auto new_item_pt = new TbItemType(table_id);
        if (!new_item_pt)
        {
            return nullptr;
        }
        auto check_ret = data_items_.insert(std::make_pair(key, new_item_pt));
        if (check_ret.second == false)
        {
            delete new_item_pt;
            return nullptr;
        }
        new_item_pt->SetDbStatus(E_DB_STATUS_INSERT);
        return new_item_pt;
    }
    void DeleteItem(key_type key)
    {
        auto it = data_items_.find(key);
        if (it != data_items_.end())
        {
            it->second->SetDbStatus(E_DB_STATUS_DELETE);
            // delete it->second;
            // data_items_.erase(it);
        }
    }
    int32_t GetTableId()
    {
        return table_id_;
    }

    void ForEach(std::function<void(TbItemType * item_data)> && visitor)
    {
        for (auto it = data_items_.begin(); it != data_items_.end(); ++it)
        {
            if (it->second->GetDbStatus() == E_DB_STATUS_DELETE)
            {
                continue;
            }
            visitor(it->second);
        }
    }
    void ForEach(std::function<void(const TbItemType * item_data)> && visitor) const
    {
        for (auto it = data_items_.begin(); it != data_items_.end(); ++it)
        {
            if (it->second->GetDbStatus() == E_DB_STATUS_DELETE)
            {
                continue;
            }
            visitor(it->second);
        }
    }
    void ForEachForDB(std::function<void(const TbItemType * item_data)> && visitor) const
    {
        for (auto it = data_items_.begin(); it != data_items_.end(); ++it)
        {
            visitor(it->second);
        }
    }
    void ForEachForDB(std::function<void(TbItemType * item_data)> && visitor)
    {
        for (auto it = data_items_.begin(); it != data_items_.end(); ++it)
        {
            visitor(it->second);
        }
    }
    void ForEachWithBreak(std::function<bool(const TbItemType * item_data)> && visitor) const
    {
        for (auto it = data_items_.begin(); it != data_items_.end(); ++it)
        {
            if (it->second->GetDbStatus() == E_DB_STATUS_DELETE)
            {
                continue;
            }
            if (visitor(it->second))
            {
                break;
            }
        }
    }
    void ForEachWithBreak(std::function<bool(TbItemType * item_data)> && visitor)
    {
        for (auto it = data_items_.begin(); it != data_items_.end(); ++it)
        {
            if (it->second->GetDbStatus() == E_DB_STATUS_DELETE)
            {
                continue;
            }
            if (visitor(it->second))
            {
                break;
            }
        }
    }
    // 刷新item,回收需要删除的item
    void FlushItem()
    {
        for (auto it = data_items_.begin(); it != data_items_.end();)
        {
            if (it->second->GetDbStatus() == E_DB_STATUS_DELETE)
            {
                delete it->second;
                data_items_.erase(it++);
            }
            else
            {
                it->second->ClearDbStatus();
                ++it;
            }
        }
    }
    size_t Size()
    {
        return data_items_.size();
    }
    void SetUserData(int32_t user_data)
    {
        user_data_ = user_data;
    }
    int32_t GetUserData()
    {
        return user_data_;
    }
    void ChangeTableId(int32_t new_tb_id)
    {
        table_id_ = new_tb_id;
    }
protected:
    std::map<key_type, TbItemType* > data_items_;    // key:item value:tb_item
    int32_t table_id_ = 0;
    int32_t user_data_ = 0;
};

template<class TbItemType, int32_t table_id>
class OneTableItemMgr
{
public:
    OneTableItemMgr() : table_item_(table_id)
    {
    }

    ~OneTableItemMgr()
    {
    }

    TbItemType & HoldData()
    {
        table_item_.SetDbStatus(E_DB_STATUS_UPDATE);
        return table_item_;
    }
    const TbItemType & GetData()
    {
        return table_item_;
    }
    const TbItemType & GetData() const
    {
        return table_item_;
    }
protected:
    TbItemType table_item_;
};

#endif // __DATA_TABLE_ITEM_H__
