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
ClientUserManager::ClientUserManager()
{

}

ClientUserManager::~ClientUserManager()
{

}

void ClientUserManager::UpdateUserNode(int64_t user_id, EServerRouteNodeType node_type, int32_t node_index)
{
    ClientUser &user_data = client_users_[user_id];
    user_data.user_id = user_id;
    auto & node_info = user_data.net_nodes_[node_type];
    node_info.node_type = node_type;
    node_info.status = EClientNodeStatus::E_CLIENT_NODE_STATUS_NORMAL;
    node_info.node_index = node_index;
    TINFO("update user node, user_id:" << user_id << ", node_type:" << int32_t(node_type) << ", node_index" << node_index);
}

void ClientUserManager::DeleteUser(int64_t user_id)
{
    auto it = client_users_.find(user_id);
    if (it != client_users_.end())
    {
        client_users_.erase(it);
        TINFO("delete user from client user manager, user_id:" << user_id);
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
}

EUserStatus ClientUserManager::GetUserStatus(int64_t user_id)
{
    auto user_pt = FuncTools::GetMapValue(client_users_, user_id);
    return user_pt ? user_pt->status : EUserStatus::E_CLIENT_USER_STATUS_NONE;
}