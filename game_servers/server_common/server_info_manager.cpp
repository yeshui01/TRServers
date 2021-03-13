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
#include "server_session.h"
ServerInfoManager::ServerInfoManager()
{

}

ServerInfoManager::~ServerInfoManager()
{
    
}

// 添加服务器信息
bool ServerInfoManager::AddServerInfo(ServerSession *session_pt, EServerRouteNodeType node_type,
    EServerType s_type, int32_t index, int32_t zone_id)
{
    if (!session_pt)
        return false;
    TINFO("AddServerInfo, sessionid:" << session_pt->GetConnId() << ", fd:" << session_pt->GetFd()
        << ", sessionptr:" << (int64_t)session_pt << ", node_type:" << int32_t(node_type)
        << ", index:" << index << ", server_type:" << int32_t(s_type) << ", zoneid:" << zone_id);
    ServerInfo info(session_pt, node_type, s_type, index, zone_id);
    auto ret = server_infos_.insert(std::make_pair(session_pt->GetConnId(), info));
    if (!ret.second)
    {
        return false;
    }
    auto & new_info = ret.first->second;
    type_servers_[int8_t(s_type)].push_back(&new_info);
    route_servers_[int8_t(node_type)].push_back(&new_info);
    TDEBUG("node serveraddr:" << int64_t(&new_info) << ", node_type:" << (int32_t)new_info.node_type
    << ", server_type:" << (int32_t)new_info.server_type << ", index:" << index);
    session_pt->SetChannelType(ESessionChannelType::E_CHANNEL_SERVER_TO_SERVER);
    session_pt->SetChannelServerInfo(node_type, index, ServerSession::CalcServerId(node_type, index));
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

ServerInfo* ServerInfoManager::GetRouteNodeInfo(EServerRouteNodeType node_type, int32_t index, int32_t zone_id)
{
    auto v_server_info = GetRouteTypeServers(node_type);
    if (!v_server_info)
    {
        TDEBUG("not found node_type : " << int32_t(node_type));
        return nullptr;
    }
    
    ServerInfo * ret = nullptr;
    for (auto && v : *v_server_info)
    {
        // TDEBUG("v->route_type:" << int32_t(v->route_type) << ", v->index:" << v->server_index << " vptr:" << int64_t(v));
        if (v->node_type == node_type && v->server_index == index && v->zone_id == zone_id)
        {
            ret = v;
            break;
        }
    }
    return ret;
}
// 删除一个服务器信息
void ServerInfoManager::DeleteServerInfo(int32_t session_id)
{
    auto it = server_infos_.find(session_id);
    if (it == server_infos_.end())
        return;
    TINFO("DeleteServerInfo, session_id:" << session_id);
    server_infos_.erase(it);
    type_servers_.clear();
    route_servers_.clear();
    for (auto it_server = server_infos_.begin(); it_server != server_infos_.end(); ++it_server)
    {
        type_servers_[int8_t(it_server->second.server_type)].push_back(&it_server->second);
        route_servers_[int8_t(it_server->second.node_type)].push_back(&it_server->second);
    }
}

// 设置当前服务器数据
void ServerInfoManager::SetCurrentServerInfo(EServerRouteNodeType node_type, EServerType s_type, int32_t index)
{
    cur_node_type_ = node_type;
    cur_server_type_ = s_type;
    cur_server_index_ = index;
}

// 获取当前服务器node_type
EServerRouteNodeType ServerInfoManager::GetCurrentNodeType()
{
    return cur_node_type_;
}

// 获取当前服务器类型
EServerType ServerInfoManager::GetCurrentServerType()
{
    return cur_server_type_;
}

// 获取当前服务器index
int32_t ServerInfoManager::GetCurrentServerIndex()
{
    return cur_server_index_;
}

void ServerInfoManager::Traversal(std::function<void (ServerInfo * server_info_pt)> && visitor)
{
    for (auto it = server_infos_.begin(); it != server_infos_.end(); ++it)
    {
        visitor(&(it->second));
    }
}