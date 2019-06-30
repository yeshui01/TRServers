/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-29 20:54
 * Email        : 824338670@qq.com
 * Filename     : game_server.h
 * Description  : 游戏服务器基类
 * ======================================================*/

#ifndef __TR_GAME_SERVER_H__
#define __TR_GAME_SERVER_H__
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
#include "server_define.h"
#include <string>

using GameParentClass = TBaseServer;
class GameServer : public TBaseServer
{
public:
	GameServer(int32_t index);
	~GameServer();
	virtual bool Init();
public:
    // 新的连接建立了
    virtual void OnNewConnectComeIn(TConnection * new_connection);
    // 运行前的检查
    virtual bool RunStepCheck();
    // 正常运行
    virtual bool RunStepRunning();
    // 启动监听
    bool StartLocalListen(std::string server_name);
    bool StartLocalListenByIndex(std::string server_name, int32_t index);
    // 根据服务器节点类型获取服务器名字
    std::pair<bool, std::string> GetServerNameByNodeType(EServerRouteNodeType route_type);
    // 根据服务器名字获取路由节点类型
    std::pair<bool, EServerRouteNodeType> GetRouteTypeByServerName(const std::string server_name);
    // 根据服务器路由节点获取服务区类型
    std::pair<bool, EServerType> GetServerTypeByNodeType(EServerRouteNodeType route_type);
protected:
    // 启动时连接其他服务器
    virtual bool BootUpConnectServer();
    // 即将运行
    virtual bool RunStepWillRun();
protected:
	EServerType server_type_ = EServerType::E_SERVER_TYPE_INVALID_SERVER;
    EServerRouteNodeType node_type_ = EServerRouteNodeType::E_SERVER_ROUTE_NODE_NONE;
    int32_t index_ = 0;
};
#endif // __TR_GAME_SERVER_H__
