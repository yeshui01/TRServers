/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-29 21:13
 * Email        : 824338670@qq.com
 * Filename     : main.cpp
 * Description  : 
 * ======================================================*/
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
#include <string>

#include "game_servers/server_common/server_config.h"
#include "login_server.h"


#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/loggingmacros.h>

int main(int argc, char* argv[])
{
	log4cplus::Logger logger = log4cplus::Logger::getRoot();
	// log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log_config/login_server_log.properties"));
	g_ServerLog.Init("log_config/login_server_log.properties");
	TDEBUG("hello login server");
	LoginServer login_server(0);
	login_server.RunService();
	// g_GameConfig.Load();
	// if (argc < 2)
	// {
	// 	TERROR("argc num error!!");
	// 	return 0;
	// }
	// int32_t port = std::stoi(std::string(argv[1]));
 // 	TDEBUG("hello msg server");
	// MsgServer msg_server;
	// msg_server.InitConnectionPool<MsgConnection>(1);
	// msg_server.Bind("127.0.0.1", port);
	// TDEBUG("bind addr, port:" << port);

	// // 注册一个消息处理
	// // SampleMsgHandler * msg_handler = new SampleMsgHandler();
	// // g_MsgDispatcher.RegisterMsgHandler(1, msg_handler);
	// g_MsgDispatcher.RegisterMsgHandlerEx<SampleMsgHandler>(1);
	// msg_server.Listen();
	// msg_server.AddLoopRun([](time_t cur_mtime)
	// {
	// 	g_MsgDispatcher.DispatchMessage();
	// });
	// msg_server.RunService();
	return 0;
}