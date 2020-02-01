/*=======================================================
# Author       : mknight
# Last modified: 2020-01-31 11:37
# Email        : 824338670@qq.com
# Filename     : root_global.cpp
# Description  : 
# =======================================================*/
#include "root_global.h"
#include "server_common/server_config.h"

RootGlobal::RootGlobal()
{

}

RootGlobal::~RootGlobal()
{

}

bool RootGlobal::Init()
{
    int32_t logic_num = g_ServerConfig.GetJsonConfig()["root_server"]["logic_server_num"].asInt();
    for (int32_t i = 0; i < logic_num; ++i)
    {
        logic_server_player_num_[i] = 0;
    }
    return true;
}

void RootGlobal::Stop()
{

}

int32_t RootGlobal::AlocateLogicServerIndex(int64_t role_id)
{
    auto it = role_logic_index_.find(role_id);
    if (it != role_logic_index_.end())
    {
        return it->second;
    }
    else if (logic_server_player_num_.size() > 1)
    {
        int32_t logic_index = role_id % logic_server_player_num_.size();
        role_logic_index_[role_id] = logic_index;
        logic_server_player_num_[logic_index] += 1;
        return logic_index;
    }
    return 0;
}

void RootGlobal::RemoveRoleLogicIndex(int64_t role_id)
{
    auto it = role_logic_index_.find(role_id);
    if (it != role_logic_index_.end())
    {
        int32_t logic_index = it->second;
        logic_server_player_num_[logic_index] -= 1;
        
        role_logic_index_.erase(it);
    }
}