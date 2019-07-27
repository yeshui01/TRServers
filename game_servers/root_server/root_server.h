/*=======================================================
# Author       : mknight
# Last modified: 2019-06-29 15:04
# Email        : 824338670@qq.com
# Filename     : root_server.h
# Description  : 
# =======================================================*/
#ifndef __TR_ROOT_SERVER_H__
#define __TR_ROOT_SERVER_H__

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

using RootParentClass = GameServer;
class RootServer : public GameServer
{
public:
	RootServer(int32_t index);
	~RootServer();
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

public:
    void AddWaitStart(EServerRouteNodeType node_type);
    bool CheckAllWaitStart();
    // 获取等待启动的节点数量
    const std::map<EServerRouteNodeType, int32_t> & GetWaitStartNodes();
protected:
    // 等待启动指令的服务器节点
    std::map<EServerRouteNodeType, int32_t> node_type_waitstart_num_;    // key:node_type value:wait_num
    std::map<EServerRouteNodeType, int32_t> node_type_to_wait_num_;              // key:node_type value:num
};

#endif 	// __TR_ROOT_SERVER_H__
