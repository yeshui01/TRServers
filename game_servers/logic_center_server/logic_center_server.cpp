/*=======================================================
# Author       : mknight
# Last modified: 2020-02-01 16:36
# Email        : 824338670@qq.com
# Filename     : logic_center_server.cpp
# Description  : 
# =======================================================*/
#include "logic_center_server.h"
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
#include "tr_timer/server_time.h"
#include "global_timer.h"
LogicCenterServer::LogicCenterServer(int32_t index) : GameServer(index)
{
    server_type_ = EServerType::E_SERVER_TYPE_LOGIC_CENTER_SERVER;
    node_type_ = EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC_CENTER;
}

LogicCenterServer::~LogicCenterServer()
{

}
bool LogicCenterServer::Init()
{
    if (!LogicCenterParentClass::Init())
    {
        return false;
    }
    return true;
}

void LogicCenterServer::OnNewConnectComeIn(TConnection * new_connection)
{
    LogicCenterParentClass::OnNewConnectComeIn(new_connection);
}

bool LogicCenterServer::RunStepCheck()
{
    if (!StartLocalListen("logic_center_server"))
    {
        Stop();
        return false;
    }

    return true;
}

bool LogicCenterServer::RunStepRunning()
{
    return LogicCenterParentClass::RunStepRunning();
}

bool LogicCenterServer::RunStepWillRun()
{
    if (!LogicCenterParentClass::RunStepWillRun())
    {
        return false;
    }
    // 通知其他服务器自己的服务器节点数据
    std::vector<EServerRouteNodeType> v_node_type = {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT,
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA,
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER};
    for (auto && node_type : v_node_type)
    {
        RegServerInfoToOtherServers(node_type, index_);
    }
    return true;
}

bool LogicCenterServer::RunStepWaiting()
{
    return LogicCenterParentClass::RunStepWaiting();
}