/*=======================================================
# Author       : mknight
# Last modified: 2020-01-31 11:36
# Email        : 824338670@qq.com
# Filename     : root_global.h
# Description  : 
# =======================================================*/
#ifndef __TR_ROOT_GLOBAL_H__
#define __TR_ROOT_GLOBAL_H__

#include "tr_common/base_type_def.h"
#include "tr_common/singleton.h"
#include <map>
#include <utility>



class RootGlobal : public TSingleton<RootGlobal>
{
public:
    RootGlobal();
    ~RootGlobal();

    bool Init();
    void Stop();

    // 给角色分配一个logic_server
    int32_t AlocateLogicServerIndex(int64_t role_id);
    // 删除角色的logic_server记录
    void RemoveRoleLogicIndex(int64_t role_id);
    
protected:
    // 每个角色对应分配的逻辑服务器进程index
    std::map<int64_t, int32_t> role_logic_index_;   // key:role_id value:logic_server_index
    // 每个logic_server上当前的人数
    std::map<int32_t, int32_t> logic_server_player_num_;   // key:logic_serer_index, value:player_num
};

#define g_RootGlobal RootGlobal::Instance()

#endif // __TR_ROOT_GLOBAL_H__
