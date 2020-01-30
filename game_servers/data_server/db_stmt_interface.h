/*=======================================================
# Author       : mknight
# Last modified: 2020-01-27 16:13
# Email        : 824338670@qq.com
# Filename     : db_stmt_interface.h
# Description  : 
# =======================================================*/
#ifndef __TR_DB_STMT_INTERFACE_H__
#define __TR_DB_STMT_INTERFACE_H__

// 预处理语句id枚举
enum EGmeDbPreStmtID
{
    E_PRE_STMT_ID_START = 0,
    // role_base
    E_PRE_STMT_ROLE_BASE_INSET,     // role_base insert
    E_PRE_STMT_ROLE_BASE_UPDATE,    // role_base update
    E_PRE_STMT_ROLE_BASE_DELETE,    // role_base delete

    E_PRE_STMT_ID_END
};

#endif // __TR_DB_STMT_INTERFACE_H__
