/*=======================================================
# Author       : mknight
# Last modified: 2019-10-06 10:34
# Email        : 824338670@qq.com
# Filename     : databash_hash.cpp
# Description  : 
# =======================================================*/

#include "database_hash.h"
#include "database_interface.h"
#include <sstream>
namespace DatabaseHash
{
    // 根据账号名Hash到账号数据库id
    int32_t AccountDataBaseHashIdByAccName(std::string acc_name)
    {
        // 取前两个字节做哈希值,如果不足两个字节,默认返回第一个
        int32_t len = acc_name.length();
        if (len < 2)
            return E_DATABASE_ID_ACCOUNT_1;

        // 测试期间,全部返回第一个
        return E_DATABASE_ID_ACCOUNT_1;

        const char * start = acc_name.c_str();
        const uint16_t * hash_val = reinterpret_cast<const uint16_t *>(start);
        uint16_t db_id = (*hash_val) % 2 + 1;
        return int32_t(db_id);
    }
    // 根据账号名字Hash到账号数据表名
    std::string AccountTbNameHashByAccName(std::string acc_name)
    {
        // 取后两个字节做哈希值,如果不足两个字节,默认返回第一个
        int32_t len = acc_name.length();
        if (len < 2)
            return "account01";

        const char * start = & acc_name[acc_name.size() - 2];
        const uint16_t * hash_val = reinterpret_cast<const uint16_t *>(start);
        int32_t index = (*hash_val) % 10 + 1;  // account01 - account10

        std::stringstream ss;
        ss.width(2);
        ss.fill('0');
        ss << index;
        return "account" + ss.str();
    }
};  // namespace DatabaseHash end