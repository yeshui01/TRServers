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
#include "logic_server.h"


#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/loggingmacros.h>

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr <<" bootup param size error" << std::endl;
		return 0;
	}
	std::string log_config = std::string("log_config/logic_server") + std::string(argv[1]) + std::string("_log.properties");
	// log4cplus::Logger logger = log4cplus::Logger::getRoot();
	// log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(log_config.c_str()));
	g_ServerLog.Init(log_config);
	TDEBUG("hello root server");
	LogicServer logic_server(std::stoi(argv[1]));
	logic_server.RunService();
	return 0;
}
