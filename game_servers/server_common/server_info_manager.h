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


class ServerSession;
struct ServerInfo
{
    ServerInfo(ServerSession * session_pt, EServerRouteNodeType server_node_type, EServerType s_type, int32_t index, int32_t server_zone_id)
    {
        session = session_pt;
        node_type = server_node_type;
        server_type = s_type;
        server_index = index;
        zone_id = server_zone_id;
    }
    ServerSession * session = nullptr;                                                  // 节点会话
    EServerRouteNodeType node_type = EServerRouteNodeType::E_SERVER_ROUTE_NODE_NONE;   // 路由节点类型
    EServerType server_type = EServerType::E_SERVER_TYPE_INVALID_SERVER;                // 服务器类型
    int32_t server_index = 0;                                                           // 节点类型和服务器类型等同类服务器进程下的索引
    int32_t zone_id = 0;                                                                // 跨服的时候用                                                
};

class ServerInfoManager : public TSingleton<ServerInfoManager>
{
public:
    ServerInfoManager();
    ~ServerInfoManager();
    // 添加服务器信息
    bool AddServerInfo(ServerSession * session_pt, EServerRouteNodeType node_type, EServerType s_type, int32_t index, int32_t zone_id = 0);
    // 获取服务器信息
    const ServerInfo *GetServerInfo(int32_t session_id);
    // 获取某个类型的服务器列表
    const std::vector<ServerInfo*> *GetTypeServers(EServerType s_type);
    // 根据路由节点类型获取服务器列表
    const std::vector<ServerInfo*> * GetRouteTypeServers(EServerRouteNodeType node_type);
    // 根据路由节点类型和类型索引获取服务器信息
    const ServerInfo* GetRouteNodeInfo(EServerRouteNodeType node_type, int32_t index, int32_t zone_id = 0);
    // 删除一个服务器信息
    void DeleteServerInfo(int32_t session_id);
    // 设置当前服务器数据
    void SetCurrentServerInfo(EServerRouteNodeType node_type, EServerType s_type, int32_t index);
    // 获取当前服务器node_type
    EServerRouteNodeType GetCurrentNodeType();
    // 获取当前服务器类型
    EServerType GetCurrentServerType();
    // 获取当前服务器index
    int32_t GetCurrentServerIndex();
protected:
    std::map<int32_t, ServerInfo> server_infos_;    // key:connection_id value:server_info
    std::map<int8_t, std::vector<ServerInfo*> > type_servers_;   // key:server_type value:sever_info
    std::map<int8_t, std::vector<ServerInfo*> > route_servers_;  // key:route_note_type value:server_info

    EServerRouteNodeType cur_node_type_ = EServerRouteNodeType::E_SERVER_ROUTE_NODE_NONE;
    EServerType cur_server_type_ = EServerType::E_SERVER_TYPE_INVALID_SERVER;
    int32_t cur_server_index_ = 0;
};

#define g_ServerManager ServerInfoManager::Instance()
#endif // __SERVERS_INFO_MANAGER_H__