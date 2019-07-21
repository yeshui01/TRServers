/*=======================================================
# Author       : mknight
# Last modified: 2019-06-29 15:05
# Email        : 824338670@qq.com
# Filename     : logic_server.cpp
# Description  : 
# =======================================================*/

#include "logic_server.h"
#include "server_common/server_config.h"
#include "tr_timer/time_wheel.h"
#include "log_module.h"
#include "net_common.h"
#include "net_socket.h"
#include "cycle_buffer.h"
#include "obj_pool.h"
#include "singleton.h"
#include "net_connection.h"
#include "base_server.h"
#include "net_epoll.h"
#include "log_module.h"
#include "msg_dispatcher.h"
#include "msg_handler.h"
#include "msg_queue.h"
#include "net_message.h"
#include "common_define.h"
#include "server_common/server_session.h"
#include "server_common/server_info_manager.h"
#include <string>

LogicServer::LogicServer(int32_t index):GameServer(index)
{
    server_type_ = EServerType::E_SERVER_TYPE_LOGIC_SERVER;
    node_type_ = EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC;
}

LogicServer::~LogicServer()
{

}

bool LogicServer::Init()
{
    if (!LogicParentClass::Init())
    {
        return false;
    }
    return true;
}

// 运行前的检查
bool LogicServer::RunStepCheck()
{
    if (!StartLocalListenByIndex("logic_server", index_))
    {
        Stop();
        return false;
    }
    // if (!BootUpConnectServer())
    // {
    //     Stop();
    //     TERROR("boot up connect failed");
    //     return false;
    // }
    return true;
}

// 正常运行
bool LogicServer::RunStepRunning()
{
    LogicParentClass::RunStepRunning();
    return true;
}

void LogicServer::OnNewConnectComeIn(TConnection * new_connection)
{
    LogicParentClass::OnNewConnectComeIn(new_connection);
}

// 即将运行
bool LogicServer::RunStepWillRun()
{
    if (!LogicParentClass::RunStepWillRun())
    {
        return false;
    }
    // 通知其他服务器自己的服务器节点数据
    std::vector<EServerRouteNodeType> v_node_type = {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT,
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA,
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER};
    for (auto && node_type : v_node_type)
    {
        RegServerInfoToOtherServers(node_type, 0);
    }
    return true;
}

bool LogicServer::RunStepWaiting()
{
    return LogicParentClass::RunStepWaiting();
}