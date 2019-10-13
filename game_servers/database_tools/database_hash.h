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

namespace DatabaseHash
{
    // 根据账号名Hash到账号数据库id
    int32_t AccountDataBaseHashIdByAccName(std::string acc_name);

    // 根据账号名字Hash到账号数据表名
    std::string AccountTbNameHashByAccName(std::string acc_name);

};  // namespace DatabaseHash end


#endif // __TR_SERVER_DATABASE_HASH_H__