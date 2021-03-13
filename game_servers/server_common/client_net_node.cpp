/*=======================================================
# Author       : mknight
# Last modified: 2019-11-02 10:40
# Email        : 824338670@qq.com
# Filename     : client_net_node.cpp
# Description  : 
# =======================================================*/

#include "client_net_node.h"
#include "tr_common/func_tools.h"
#include "tr_log/log_module.h"
#include "tr_timer/server_time.h"
ClientUserManager::ClientUserManager()
{

}

ClientUserManager::~ClientUserManager()
{

}

void ClientUserManager::UpdateUserNode(int64_t user_id, EServerRouteNodeType node_type, int32_t node_index, int32_t zone_id/* = -1*/)
{
    ClientUser &user_data = client_users_[user_id];
    user_data.user_id = user_id;
    auto & node_info = user_data.net_nodes_[node_type];
    node_info.node_type = node_type;
    node_info.status = EClientNodeStatus::E_CLIENT_NODE_STATUS_NORMAL;
    node_info.node_index = node_index;
    node_info.zone_id = zone_id;
    TINFO("update user node, user_id:" << user_id << ", node_type:" << int32_t(node_type) << ", node_index" << node_index);
}

void ClientUserManager::DeleteUser(int64_t user_id)
{
    auto it = client_users_.find(user_id);
    if (it != client_users_.end())
    {
        TINFO("delete user from client user manager, user_id:" << user_id << ", role_id:" << it->second.role_id);
        if (it->second.role_id > 0)
        {
            auto it_role = role_users_.find(it->second.role_id);
            if (it_role != role_users_.end())
            {
                role_users_.erase(it_role);
            }
        }
        
        client_users_.erase(it);
    }
}

void ClientUserManager::DeleteUserNode(int64_t user_id, EServerRouteNodeType node_type)
{
    auto it = client_users_.find(user_id);
    if (it != client_users_.end())
    {
        auto it_node = it->second.net_nodes_.find(node_type);
        if (it_node != it->second.net_nodes_.end())
        {
            it->second.net_nodes_.erase(it_node);
            TINFO("delete user node, user_id:" << user_id << ", node_type:" << int32_t(node_type));
        } 
    }
}

void ClientUserManager::AttachUserRole(int64_t user_id, int64_t role_id)
{
    auto user_info = FuncTools::GetMapValue(client_users_, user_id);
    if (user_info)
    {
        user_info->role_id = role_id;
        TINFO("user attach role, user_id:" << user_id << ", role_id:" << role_id);
        role_users_[role_id] = user_info;
    }
}

ClientUser * ClientUserManager::HoldClientUser(int64_t user_id)
{
    return FuncTools::GetMapValue(client_users_, user_id);
}

void ClientUserManager::UpdateUserStatus(int64_t user_id, EUserStatus e_status)
{
    auto & user_info = client_users_[user_id];
    if (user_info.user_id == 0)
    {
        user_info.user_id = user_id;
    }
    user_info.status = e_status;
    user_info.status_time = g_ServerTime.NowTime();
    TDEBUG("UpdateUserStatus,user_id:" << user_id << "e_status:" << int32_t(e_status));
}

EUserStatus ClientUserManager::GetUserStatus(int64_t user_id)
{
    auto user_pt = FuncTools::GetMapValue(client_users_, user_id);
    return user_pt ? user_pt->status : EUserStatus::E_CLIENT_USER_STATUS_NONE;
}

ClientNetNode * ClientUserManager::GetRoleNode(int64_t role_id, EServerRouteNodeType node_type)
{
    auto it_role = role_users_.find(role_id);
    if (it_role != role_users_.end())
    {
        auto user_info = it_role->second;
        auto it_node = user_info->net_nodes_.find(node_type);
        if (it_node != user_info->net_nodes_.end())
        {
            return &(it_node->second);
        }
    }
    return nullptr;
}

void ClientUserManager::DetachUserRole(int64_t user_id, int64_t role_id)
{
    auto it_role = role_users_.find(role_id);
    if (it_role != role_users_.end())
    {
        role_users_.erase(it_role);
    }
    auto user_info = FuncTools::GetMapValue(client_users_, user_id);
    if (user_info)
    {
        user_info->role_id = 0;
    }
}

const ClientUser * ClientUserManager::GetRoleUser(int64_t role_id)
{
    auto it_role = role_users_.find(role_id);
    return it_role != role_users_.end() ? it_role->second : nullptr;
}
