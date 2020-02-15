/*=======================================================
# Author       : mknight
# Last modified: 2020-02-15 14:17
# Email        : 824338670@qq.com
# Filename     : world_server.h
# Description  : 
# =======================================================*/
#ifndef __TR_WORLD_SERVER_H__
#define __TR_WORLD_SERVER_H__

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

using WorldParentClass = GameServer;

class WorldServer : public GameServer
{
public:
	WorldServer(int32_t index);
	~WorldServer();
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
    // 等待运行
    bool RunStepWaiting() override;
    // 注册消息处理
    void RegisterMsgHandle() override;
};

#endif // __TR_WORLD_SERVER_H__
