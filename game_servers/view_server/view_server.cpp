/*=======================================================
# Author       : mknight
# Last modified: 2020-02-01 16:37
# Email        : 824338670@qq.com
# Filename     : view_server.cpp
# Description  : 
# =======================================================*/
#include "view_server.h"

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

ViewServer::ViewServer(int32_t index):GameServer(index)
{
    server_type_ = EServerType::E_SERVER_TYPE_VIEW_SERVER;
    node_type_ = EServerRouteNodeType::E_SERVER_ROUTE_NODE_VIEW;
}

ViewServer::~ViewServer()
{

}

bool ViewServer::Init()
{
    if (!ViewParentClass::Init())
    {
        return false;
    }
    
    return true;
}

// 运行前的检查
bool ViewServer::RunStepCheck()
{
    if (!StartLocalListenByIndex("view_server", index_))
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
bool ViewServer::RunStepRunning()
{
    ViewParentClass::RunStepRunning();
    return true;
}

void ViewServer::OnNewConnectComeIn(TConnection * new_connection)
{
    ViewParentClass::OnNewConnectComeIn(new_connection);
}

// 即将运行
bool ViewServer::RunStepWillRun()
{
    if (!ViewParentClass::RunStepWillRun())
    {
        return false;
    }
    // // 通知其他服务器自己的服务器节点数据
    std::vector<EServerRouteNodeType> v_node_type = {EServerRouteNodeType::E_SERVER_ROUTE_NODE_VIEW_MANAGER};
    for (auto && node_type : v_node_type)
    {
        RegServerInfoToOtherServers(node_type, index_);
    }
    return true;
}

bool ViewServer::RunStepWaiting()
{
    return ViewParentClass::RunStepWaiting();
}