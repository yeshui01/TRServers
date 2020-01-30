/*=======================================================
# Author       : mknight
# Last modified: 2019-10-06 10:34
# Email        : 824338670@qq.com
# Filename     : databash_hash.h
# Description  : 
# =======================================================*/

#ifndef __TR_SERVER_DATABASE_HASH_H__
#define __TR_SERVER_DATABASE_HASH_H__
#include "tr_common/base_type_def.h"
#include <string>

#define ACCOUNT_TB_HASH_FACTOR 1000000000

namespace DatabaseHash
{
    // 根据账号名Hash到账号数据库id
    int32_t AccountDataBaseHashIdByAccName(std::string acc_name);

    // 根据账号名字Hash到账号数据表名
    std::string AccountTbNameHashByAccName(std::string acc_name);

    // 根据角色名字,hash到对应的表名
    std::string RoleNameTbNameHashByNickname(std::string nickname);

    // 根据账号id映射账号数据库id
    int32_t GetAccountDbID(int64_t acc_id);
    // 根据账号id映射到账号名
    std::string AccountTbNameHashByAccId(int64_t acc_id);
};  // namespace DatabaseHash end


#endif // __TR_SERVER_DATABASE_HASH_H__