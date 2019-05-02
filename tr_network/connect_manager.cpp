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
        return connections_.insert(std::make_pair(connect_pt->GetFd(), connect_pt)).second;
    }
    return false;
}

// 从管理器删除连接
void ConnectManager::DelConnection(TConnection * connect_pt)
{
    auto it = connections_.find(connect_pt->GetFd());
    if (it != connections_.end())
    {
        connections_.erase(it);
    }
}
