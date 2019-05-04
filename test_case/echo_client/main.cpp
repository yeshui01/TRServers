#include <error.h>
#include <signal.h>
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

#include <string>
#include <chrono>
#include <thread>

void signal_func(int32_t sig) 
{
	TDEBUG("signal_func");
}



class EchoClient : public TConnection
{
public:
	EchoClient();
	~EchoClient();
	void OnClose() override;
	void AfterReadData(int32_t read_size) override;
};

void EchoClient::AfterReadData(int32_t read_size)
{
	TDEBUG("EchoClient afterreaddata, read_size:" << read_size);
	// 直接回复数据
	char * buffer = new char[read_size];
	int32_t recv_s = Recv(buffer, read_size);
	TDEBUG("EchoClient recv data:" << buffer << ", data_len:" << recv_s);
}

EchoClient::EchoClient()
{
	TDEBUG("EchoClient construct");
}
EchoClient::~EchoClient()
{
	TDEBUG("EchoClient destruct");
}

void EchoClient::OnClose()
{
	TDEBUG("EchoClient OnClose");
}

int main(int argc, char* argv[])
{
 	std::cout << "hello test" << std::endl;
	if (argc < 2)
	{
		TERROR("client argc num is error");
		return 0;
	}
	signal(SIGPIPE,signal_func);
	int32_t port = std::stoi(std::string(argv[1]));
	EchoClient client;
	auto ret = client.Connect("127.0.0.1", port);
	if (ret !=  ESocketOpCode::E_SOCKET_OP_CODE_CORRECT)
	{
		TERROR("connect server failed");
		return 0;
	}
	int32_t count = 300;
	auto start_time = std::chrono::system_clock::now();
	for (int32_t i = 0; i < count; ++i)
	{
		TDEBUG("client ready send data");
		std::string data = "hello my echo server!";
		int32_t send_size = client.Send(data.c_str(), data.size() + 1);
		TDEBUG("send data to server, send_size:" << send_size);
		char buffer[1024];
		int32_t recv_size = client.WaitRecv(buffer, 1024);
		TDEBUG("EchoClient recv data:" << buffer << ", data_len:" << recv_size);
		// std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	auto end_time = std::chrono::system_clock::now();
	auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
	TDEBUG("total time microseconds(" << total_time.count() 
		<< "), ms(" << total_time.count()/1000 << "), s(" << total_time.count()/1000000 << ")");
	// std::string str;
	// while (1)
	// {
	// 	std::cin >> str;
	// 	TDEBUG("client ready send data");
	// 	int32_t send_size = client.Send(str.c_str(), str.size() + 1);
	// 	TDEBUG("send data to server, send_size:" << send_size);
	// }
	TDEBUG("echo client exit, errno:" << errno);
	return 0;
}
