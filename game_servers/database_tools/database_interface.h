/*=======================================================
# Author       : mknight
# Last modified: 2019-10-04 14:18
# Email        : 824338670@qq.com
# Filename     : database_interface.h
# Description  : 
# =======================================================*/
#ifndef __DATABASE_INTERFACE_H__
#define __DATABASE_INTERFACE_H__
// 数据库id
enum EDataBaseID
{
    E_DATABASE_ID_ACCOUNT_1 = 1,        // 账号数据库1
    E_DATABASE_ID_ACCOUNT_2 = 2,        // 账号数据库2

    E_DATABASE_ID_GAME = 10,            // 游戏服务器
};

// 数据表id
enum EDatabaseTableID
{
    E_DATABASE_TABLE_ID_ROLE = 1,   // 角色基础数据表
};
#endif // __DATABASE_INTERFACE_H__
