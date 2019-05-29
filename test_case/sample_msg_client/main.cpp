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
#include "msg_dispatcher.h"
#include "msg_handler.h"
#include "msg_queue.h"
#include "net_message.h"

#include <string>
#include <chrono>
#include <thread>

void signal_func(int32_t sig) 
{
	TDEBUG("signal_func");
}



class MsgClient : public TConnection
{
public:
	MsgClient();
	~MsgClient();
	void OnClose() override;
	void AfterReadData(int32_t read_size) override;
};

void MsgClient::AfterReadData(int32_t read_size)
{
	TDEBUG("MsgClient afterreaddata, read_size:" << read_size);
	char * buffer = new char[read_size];
	int32_t recv_s = Recv(buffer, read_size);
	
	NetMessage net_message;
	if (net_message.UnSerialize(buffer, recv_s))
	{
		TDEBUG("msg UnSerialize success, msg_class:" << net_message.GetMsgClass() << ", msg_type:" << net_message.GetMsgType());
		TDEBUG("MsgClient recv data, msg content:" << net_message.GetContent() << ", data_len:" << recv_s);
	}
	else 
	{
		TERROR("msg UnSerialize failed");
	}
	delete [] buffer;
}

MsgClient::MsgClient()
{
	TDEBUG("MsgClient construct");
}
MsgClient::~MsgClient()
{
	TDEBUG("MsgClient destruct");
}

void MsgClient::OnClose()
{
	TDEBUG("MsgClient OnClose");
}

int main(int argc, char* argv[])
{
 	std::cout << "hello test" << std::endl;
	if (argc < 3)
	{
		TERROR("client argc num is error");
		return 0;
	}
	signal(SIGPIPE,signal_func);
	int32_t port = std::stoi(std::string(argv[1]));
	int32_t count = std::stoi(std::string(argv[2]));
	MsgClient client;
	auto ret = client.Connect("127.0.0.1", port);
	if (ret !=  ESocketOpCode::E_SOCKET_OP_CODE_CORRECT)
	{
		TERROR("connect server failed");
		return 0;
	}
	
	auto start_time = std::chrono::system_clock::now();
	static std::vector<std::string> s_test_content = {
		"hello req msg server!",
		"hello req msg server! 11111111",
		"hello req msg server! 2222222222222",
		"hello req msg server! 333"
	};
	for (int32_t i = 0; i < count; ++i)
	{
		TDEBUG("client ready send data");
		std::string data = s_test_content[i%s_test_content.size()];
		char buffer[1024];
		NetMessage net_message(1, 1);
		net_message.SetContent(data);
		net_message.Serialize(buffer, sizeof(buffer));
		TDEBUG("msg serialize_size:" << net_message.SerializeByteNum());
		int32_t send_size = client.Send(buffer, net_message.SerializeByteNum());
		TDEBUG("send data to server, send_size:" << send_size);
		
		int32_t recv_size = client.WaitRecv(buffer, sizeof(buffer));
		if (net_message.UnSerialize(buffer, recv_size))
		{
			TDEBUG("MsgClient recv data, msg_class:" << net_message.GetMsgClass()
				<< ", msg_type:" << net_message.GetMsgType()
				<< ", content:" << net_message.GetContent());
		}
		else
		{
			TERROR("recv msg unsirialize failed");
		}
	}
	auto end_time = std::chrono::system_clock::now();
	auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
	TDEBUG("total time microseconds(" << total_time.count() 
		<< "), ms(" << total_time.count()/1000 << "), s(" << total_time.count()/1000000 << ")");
	TDEBUG("echo client exit, errno:" << errno);
	return 0;
}
