/*=======================================================
# Author       : mknight
# Last modified: 2019-11-02 10:40
# Email        : 824338670@qq.com
# Filename     : client_net_node.h
# Description  : 
# =======================================================*/

#ifndef __CLIENT_NET_NODE_H__
#define __CLIENT_NET_NODE_H__
#include "common_define.h"
#include "tr_common/base_type_def.h"
#include "tr_common/singleton.h"
#include "server_common/server_define.h"
#include <map>
enum class EClientNodeStatus : int8_t
{
    E_CLIENT_NODE_STATUS_NONE = 0,
    E_CLIENT_NODE_STATUS_CHANGING = 1,  // 变更中
    E_CLIENT_NODE_STATUS_NORMAL = 2,    // 正常
    E_CLIENT_NODE_STATUS_ERROR = 3      // 异常
};

enum class EUserStatus : int32_t
{
    E_CLIENT_USER_STATUS_NONE = 0,          // 初始状态
    E_CIENT_USER_STATUS_LOGIN_ING = 1,      // 登录中
    E_CIENT_USER_STATUS_LOGINED = 2,        // 登录成功
    E_CLIENT_USER_STATUS_CHANGE_VIEW = 3,   // 视图变更中
    E_CLIENT_USER_STATUS_NORMAL = 4,        // 正常进行游戏
    E_CLIENT_USER_STATUS_OFFLINE_ING = 5,   // 离线中
    E_CLIENT_USER_STATUS_OFFLINE = 6,       // 离线
};
// 一个客户端节点信息
struct ClientNetNode
{
    EServerRouteNodeType node_type = EServerRouteNodeType::E_SERVER_ROUTE_NODE_NONE;
    EClientNodeStatus status = EClientNodeStatus::E_CLIENT_NODE_STATUS_NONE;
    int32_t node_index = 0;
};

struct ClientUser
{
    int64_t user_id = 0;
    int64_t role_id = 0;    // 同一时间只允许一个角色
    std::map<EServerRouteNodeType, ClientNetNode> net_nodes_;    // key:node_type value:ClientNetNode
    EUserStatus status = EUserStatus::E_CLIENT_USER_STATUS_NONE;
};

class ClientUserManager : public TSingleton<ClientUserManager>
{
public:
    ClientUserManager();
    ~ClientUserManager();

    void UpdateUserNode(int64_t user_id, EServerRouteNodeType node_type, int32_t node_index);
    void DeleteUser(int64_t user_id);
    void DeleteUserNode(int64_t user_id, EServerRouteNodeType node_type);
    void AttachUserRole(int64_t user_id, int64_t role_id);
    ClientUser * HoldClientUser(int64_t user_id);
    void UpdateUserStatus(int64_t user_id, EUserStatus e_status);
    EUserStatus GetUserStatus(int64_t user_id);
protected:
    std::map<int64_t,  ClientUser> client_users_; // key1:user_id value:ClientUser
};

#define g_ClientNetNodeMgr ClientUserManager::Instance()

#endif // __CLIENT_NET_NODE_H__
