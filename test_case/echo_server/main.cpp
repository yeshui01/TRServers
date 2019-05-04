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
#include <string>
class EchoConnection : public TConnection
{
public:
	EchoConnection();
	~EchoConnection();
	void AfterReadData(int32_t read_size) override;
	void OnClose() override;
};

// #define g_ConnectPool TSingleton<TObjPool<EchoConnection> >::Instance()

EchoConnection::EchoConnection() : TConnection()
{
}
EchoConnection::~EchoConnection()
{
}
void EchoConnection::AfterReadData(int32_t read_size)
{
	TDEBUG("echo connection afterreaddata, read_size:" << read_size);
	// 直接回复数据
	int32_t sended_size = 0;
	int32_t send_total = 0;
	char buffer[1024];
	do 
	{
		int32_t recv_s = Recv(buffer, 1024);
		if (0 == recv_s)
		{
			break;
		}
		sended_size = Send(buffer, recv_s);
		send_total += sended_size;
		if (send_total >= read_size)
		{
			break;
		}
		TDEBUG("echo connect send data:" << buffer << ", data_len:" << recv_s);
	} while (sended_size > 0);
}

void EchoConnection::OnClose()
{
	TConnection::OnClose();
	// g_ConnectPool.Push(this);
	TDEBUG("EchoConnection OnClose");
}

// server
class EchoServer : public TBaseServer
{
public:
	EchoServer();
	~EchoServer();
	virtual bool Init()
	{
		return true;
	}
	// virtual TConnection * AllocateConnect();
};

EchoServer::EchoServer()
{
	TDEBUG("EchoServer construct")
}

EchoServer::~EchoServer()
{
	TDEBUG("EchoServer destruct")
}

// TConnection * EchoServer::AllocateConnect()
// {
// 	return g_ConnectPool.Pop();
// }

// test
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		TERROR("argc num error!!");
		return 0;
	}
	int32_t port = std::stoi(std::string(argv[1]));
 	TDEBUG("hello echo server");
	//g_ConnectPool.Init(3);
	EchoServer echo_server;
	echo_server.InitConnectionPool<EchoConnection>(1);
	echo_server.Bind("127.0.0.1", port);
	TDEBUG("bind addr, port:" << port);
	echo_server.Listen();
	
	// // 加入事件循环
	// Epoll epoll;
	// epoll.Create(100);
	// epoll.RegSockEvent(&echo_server, EPOLLIN);
	// echo_server.AddLoopRun([&epoll](time_t cur_time)
	// {
	// 	epoll.EventsWatch();
	// });
	echo_server.RunService();
	return 0;
}
