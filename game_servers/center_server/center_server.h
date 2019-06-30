/*=======================================================
# Author       : mknight
# Last modified: 2019-06-29 15:04
# Email        : 824338670@qq.com
# Filename     : center_server.h
# Description  : 
# =======================================================*/
#ifndef __TR_CENTER_SERVER_H__
#define __TR_CENTER_SERVER_H__

#include <iostream>
#include <string>
#include <cstdint>

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
#include "server_common/server_define.h"
#include "server_common/game_server.h"
#include <string>

using CenterParentClass = GameServer;
class CenterServer : public GameServer
{
public:
	CenterServer(int32_t index);
	~CenterServer();
	virtual bool Init();
public:
    // 新的连接建立了
    virtual void OnNewConnectComeIn(TConnection * new_connection);
    // 运行前的检查
    virtual bool RunStepCheck();
    // 正常运行
    virtual bool RunStepRunning();
    // 即将运行
    virtual bool RunStepWillRun();
};

#endif 	// __TR_CENTER_SERVER_H__
