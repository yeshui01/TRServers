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


class GameServer : public TBaseServer
{
public:
	GameServer();
	~GameServer();
	virtual bool Init() = 0;
protected:
	EServerType server_type_ = EServerType::E_SERVER_TYPE_INVALID_SERVER;

};
#endif // __TR_GAME_SERVER_H__
