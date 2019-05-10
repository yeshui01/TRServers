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
#include "func_tools.h"
#include <chrono>
static void test_cycle_buffer()
{
	CycleBuffer<char> buffer(6);
	std::string str = "hello world";
	std::string str2 = "hello worl2";
	buffer.WriteData(str.c_str(), str.size() + 1);
	TDEBUG("buffer.data_size:" << buffer.PeekDataSize() << ", space:" << buffer.GetSpace() << ", restspace:" << buffer.GetRestSpace());
	char buffer_test[1024];
	int32_t readed_size = buffer.ReadData(buffer_test, 1024, true);
	TDEBUG("PeekData buffer.data_size:" << buffer.PeekDataSize() << ", space:" << buffer.GetSpace() << ", restspace:" << buffer.GetRestSpace()
		<< ",readed_size:" << readed_size);
	// buffer.GrowSpace(10);
	// TDEBUG("GrowSpace buffer.data_size:" << buffer.PeekDataSize() << ", space:" << buffer.GetSpace() << ", restspace:" << buffer.GetRestSpace());
	TDEBUG("buffer_test:" << buffer_test);
	readed_size = buffer.ReadData(buffer_test, 1024);
	TDEBUG("readed_size222:" << readed_size);
	TDEBUG("buffer_test222:" << buffer_test);
	TDEBUG("ReadData buffer.data_size:" << buffer.PeekDataSize() << ", space:" << buffer.GetSpace() << ", restspace:" << buffer.GetRestSpace());
	buffer.WriteData(str2.c_str(), str2.size() + 1);
	readed_size = buffer.ReadData(buffer_test, 1024);
	TDEBUG("buffer_test2:" << buffer_test);
	TDEBUG("ReadData22 buffer.data_size:" << buffer.PeekDataSize() << ", space:" << buffer.GetSpace() << ", restspace:" << buffer.GetRestSpace());
}

static void test_socket()
{
	TConnection sock_obj;
	auto bind_ret = sock_obj.Bind("127.0.0.1", 5030);
	TDEBUG("bind_ret = " << (int32_t)(bind_ret));
}

static void test_objpool()
{
	struct TestObj
	{
		int32_t id = 0;
		~TestObj()
		{
			TDEBUG("test obj destruct");
		}
	};
	TSingleton<TObjPool<TestObj>>::Instance().Init(3);
	auto obj = TSingleton<TObjPool<TestObj>>::Instance().Pop();
	TDEBUG("test_objpool");
	TSingleton<TObjPool<TestObj>>::Release();
}


static void test_chrono()
{
	using namespace std::chrono;
	TDEBUG("test chrono");
	TDEBUG("system_clock::now:" << system_clock::to_time_t(system_clock::now()));
	steady_clock::time_point t2 = steady_clock::now();
	//TDEBUG("steady_clock::now:" << system_clock::to_time_t(steady_clock::now()));
}

static void test_func_tools()
{
	TDEBUG("test func_tools");
	auto r = FuncTools::SafeAdd<int32_t>(1, 2);
	TDEBUG("FuncTools::SafeAdd<int32_t>(1, 2) result:" << r);
	auto r2 = FuncTools::SafeAdd<int32_t>(10, -1);
	TDEBUG("FuncTools::SafeAdd<int32_t>(10, -1); result:" << r2);
	TDEBUG("std::numeric_limits<int32_t>::min() = " << std::numeric_limits<int32_t>::min());

	auto r3 = FuncTools::SafeAdd<int32_t>(-1, 10);
	TDEBUG("FuncTools::SafeAdd<int32_t>(-1, 10); result:" << r2);
	auto r4 = FuncTools::SafeAdd<int32_t>(-10000, std::numeric_limits<int32_t>::min());
	TDEBUG("FuncTools::SafeAdd<int32_t>(-10000, num_min); result:" << r4);

	auto r5 = FuncTools::SafeAdd<int32_t>(std::numeric_limits<int32_t>::min(), -10000);
	TDEBUG("FuncTools::SafeAdd<int32_t>(num_min,  -10000); result:" << r5);

}

int main(int argc, char* argv[])
{
 	std::cout << "hello test" << std::endl;
	int64_t a = 10;
	auto test_fun = [b{a}](){
	  std::cout << "b = " << b << std::endl;
	};
	TDEBUG("hello tr servers test");
	test_cycle_buffer();
	test_socket();
	test_objpool();
	test_chrono();
	test_func_tools();
	return 0;
}
