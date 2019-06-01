/********************************************************
# Author       : mknight
# Last modified: 2019-05-02 13:08
# Email        : 824338670@qq.com
# Filename     : connect_manager.cpp
# Description  : 
# *******************************************************/
#include "connect_manager.h"
#include "net_common.h"
#include "log_module.h"
#include "net_socket.h"

ConnectManager::ConnectManager()
{
}
ConnectManager::~ConnectManager()
{

}

// 添加连接到管理器
bool ConnectManager::AddConnction(TConnection * connect_pt)
{
    if (connect_pt)
    {
        return connections_.insert(std::make_pair(connect_pt->GetConnId(), connect_pt)).second;
    }
    return false;
}

// 从管理器删除连接
void ConnectManager::DelConnection(TConnection * connect_pt)
{
    auto it = connections_.find(connect_pt->GetConnId());
    if (it != connections_.end())
    {
        connections_.erase(it);
    }
}

void ConnectManager::Traversal(std::function<void (TConnection *)> && visit_fun)
{
    for (auto it = connections_.begin(); it != connections_.end(); ++it)
    {
        visit_fun(it->second);
    }
}