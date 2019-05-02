/********************************************************
# Author       : mknight
# Last modified: 2019-05-02 13:08
# Email        : 824338670@qq.com
# Filename     : connect_manager.h
# Description  : 
# *******************************************************/

#ifndef __CONNECT_MANAGER_H__
#define __CONNECT_MANAGER_H__
#include <map>
#include "net_connection.h"
#include "singleton.h"

class ConnectManager : public TSingleton<ConnectManager>
{
public:
    ConnectManager();
    ~ConnectManager();

    // 添加连接到管理器
    bool AddConnction(TConnection * connect_pt);
    // 从管理器删除连接
    void DelConnection(TConnection * connect_pt);
protected:
    std::map<int32_t, TConnection *> connections_;  // key: fd val:connection
};

#define g_ConnectMgr ConnectManager::Instance()
#endif  // __CONNECT_MANAGER_H__