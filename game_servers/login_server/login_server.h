/*=======================================================
# Author       : mknight
# Last modified: 2019-06-01 13:01
# Email        : 824338670@qq.com
# Filename     : login_server.h
# Description  : 
# =======================================================*/
#ifndef __TR_LOGIN_SERVER_H__
#define __TR_LOGIN_SERVER_H__

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

using LoginParentClass = GameServer;
class LoginServer : public GameServer
{
public:
	LoginServer(int32_t index);
	~LoginServer();
	bool Init() override;
public:
    // 新的连接建立了
    void OnNewConnectComeIn(TConnection * new_connection) override;
    // 运行前的检查
    bool RunStepCheck() override;
    // 正常运行
    bool RunStepRunning() override;
    // 即将运行
    bool RunStepWillRun() override;
    // 注册消息处理
    void RegisterMsgHandle() override;
    // 停服
    bool RunStepStop() override;
};

#endif 	// __TR_LOGIN_SERVER_H__
