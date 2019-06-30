/*=======================================================
# Author       : mknight
# Last modified: 2019-06-29 13:04
# Email        : 824338670@qq.com
# Filename     : server_info_manager.cpp
# Description  : 
# =======================================================*/
#include "server_info_manager.h"
#include "tr_common/func_tools.h"
#include "tr_log/log_module.h"

ServerInfoManager::ServerInfoManager()
{

}

ServerInfoManager::~ServerInfoManager()
{
    
}

// 添加服务器信息
bool ServerInfoManager::AddServerInfo(int32_t session_id, EServerRouteNodeType node_type,
    EServerType s_type, int32_t index)
{
    auto ret = server_infos_.emplace(session_id, ServerInfo(session_id, node_type, s_type, index));
    if (!ret.second)
    {
        return false;
    }
    
    type_servers_[int8_t(s_type)].push_back(&ret.first->second);
    route_servers_[int8_t(node_type)].push_back(&ret.first->second);
    return true;
}
// 获取服务器信息
const ServerInfo *ServerInfoManager::GetServerInfo(int32_t session_id)
{
    return FuncTools::GetMapValue(server_infos_, session_id);
}
// 获取某个类型的服务器列表
const std::vector<ServerInfo*> * ServerInfoManager::GetTypeServers(EServerType s_type)
{
    return FuncTools::GetMapValue(type_servers_, (int8_t)s_type);
}
// 根据路由节点类型获取服务器列表
const std::vector<ServerInfo*> * ServerInfoManager::GetRouteTypeServers(EServerRouteNodeType node_type)
{
    return FuncTools::GetMapValue(route_servers_, (int8_t)(node_type));
}
// 删除一个服务器信息
void ServerInfoManager::DeleteServerInfo(int32_t session_id)
{
    auto it = server_infos_.find(session_id);
    if (it == server_infos_.end())
        return;
    server_infos_.erase(it);
    type_servers_.clear();
    route_servers_.clear();
    for (auto it_server = server_infos_.begin(); it_server != server_infos_.end(); ++it_server)
    {
        type_servers_[int8_t(it_server->second.server_type)].push_back(&it_server->second);
        route_servers_[int8_t(it_server->second.route_type)].push_back(&it_server->second);
    }
}

