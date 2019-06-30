/*=======================================================
# Author       : mknight
# Last modified: 2019-06-29 13:04
# Email        : 824338670@qq.com
# Filename     : server_info_manager.h
# Description  : 
# =======================================================*/

#ifndef __SERVERS_INFO_MANAGER_H__
#define __SERVERS_INFO_MANAGER_H__

#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "server_define.h"
#include "tr_common/singleton.h"
#include <vector>
#include <map>

struct ServerInfo
{
    ServerInfo(int32_t ss_id, EServerRouteNodeType node_type, EServerType s_type, int32_t index)
    {
        session_id = ss_id;
        route_type = node_type;
        server_type = s_type;
        server_index = index;
    }
    int32_t session_id = 0; // 连接id
    EServerRouteNodeType route_type = EServerRouteNodeType::E_SERVER_ROUTE_NODE_NONE;   // 路由节点类型
    EServerType server_type = EServerType::E_SERVER_TYPE_INVALID_SERVER;                // 服务器类型
    int32_t server_index = 0;                                                           // 节点类型和服务器类型等同类服务器进程下的索引
};

class ServerInfoManager : public TSingleton<ServerInfoManager>
{
public:
    ServerInfoManager();
    ~ServerInfoManager();
    // 添加服务器信息
    bool AddServerInfo(int32_t session_id, EServerRouteNodeType node_type, EServerType s_type, int32_t index);
    // 获取服务器信息
    const ServerInfo *GetServerInfo(int32_t session_id);
    // 获取某个类型的服务器列表
    const std::vector<ServerInfo*> *GetTypeServers(EServerType s_type);
    // 根据路由节点类型获取服务器列表
    const std::vector<ServerInfo*> * GetRouteTypeServers(EServerRouteNodeType node_type);
    // 删除一个服务器信息
    void DeleteServerInfo(int32_t session_id);
protected:
    std::map<int32_t, ServerInfo> server_infos_;    // key:connection_id value:server_info
    std::map<int8_t, std::vector<ServerInfo*> > type_servers_;   // key:server_type value:sever_info
    std::map<int8_t, std::vector<ServerInfo*> > route_servers_;  // key:route_note_type value:server_info
};

#define g_ServerManager ServerInfoManager::Instance()
#endif // __SERVERS_INFO_MANAGER_H__