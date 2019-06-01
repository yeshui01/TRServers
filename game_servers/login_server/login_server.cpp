/*=======================================================
# Author       : mknight
# Last modified: 2019-06-01 13:02
# Email        : 824338670@qq.com
# Filename     : login_server.cpp
# Description  : 
# =======================================================*/

#include "login_server.h"
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
#include <string>

LoginServer::LoginServer()
{

}

LoginServer::~LoginServer()
{

}

bool LoginServer::Init()
{
    return g_ServerConfig.Load();
}

// 运行前的检查
bool LoginServer::RunStepCheck()
{
    return true;
}

// 正常运行
bool LoginServer::RunStepRunning()
{
    TBaseServer::RunStepRunning();
    return true;
}

void LoginServer::OnNewConnectComeIn(TConnection * new_connection)
{
    TBaseServer::OnNewConnectComeIn(new_connection);
}