/*=======================================================
# Author       : mknight
# Last modified: 2020-02-01 16:40
# Email        : 824338670@qq.com
# Filename     : view_manager_server.cpp
# Description  : 
# =======================================================*/

#include "view_manager_server.h"
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

ViewManagerServer::ViewManagerServer(int32_t index):GameServer(index)
{
    server_type_ = EServerType::E_SERVER_TYPE_VIEW_MANAGER_SERVER;
    node_type_ = EServerRouteNodeType::E_SERVER_ROUTE_NODE_VIEW_MANAGER;
}

ViewManagerServer::~ViewManagerServer()
{

}

bool ViewManagerServer::Init()
{
    if (!ViewManagerParentClass::Init())
    {
        return false;
    }
    
    return true;
}

// 运行前的检查
bool ViewManagerServer::RunStepCheck()
{
    if (!StartLocalListen("view_manager_server"))
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
bool ViewManagerServer::RunStepRunning()
{
    ViewManagerParentClass::RunStepRunning();
    return true;
}

void ViewManagerServer::OnNewConnectComeIn(TConnection * new_connection)
{
    ViewManagerParentClass::OnNewConnectComeIn(new_connection);
}

// 即将运行
bool ViewManagerServer::RunStepWillRun()
{
    if (!ViewManagerParentClass::RunStepWillRun())
    {
        return false;
    }
    // // 通知其他服务器自己的服务器节点数据
    // std::vector<EServerRouteNodeType> v_node_type = {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT,
    //     EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA,
    //     EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER};
    // for (auto && node_type : v_node_type)
    // {
    //     RegServerInfoToOtherServers(node_type, index_);
    // }
    return true;
}

bool ViewManagerServer::RunStepWaiting()
{
    return ViewManagerParentClass::RunStepWaiting();
}