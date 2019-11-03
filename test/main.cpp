#include <iostream>
#include <string>
#include <cstdint>

#include "tr_timer/time_wheel.h"
#include "tr_timer/server_time.h"
#include "log_module.h"
#include "net_common.h"
#include "net_socket.h"
#include "cycle_buffer.h"
#include "obj_pool.h"
#include "singleton.h"
#include "net_connection.h"
#include "func_tools.h"
#include <chrono>
#include "net_message.h"
#include "json/json.h"

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "test.pb.h"

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/loggingmacros.h>

#include <fstream>

static void test_cycle_buffer()
{
	CycleBuffer<char> buffer(6);
	std::string str = "hello world";
	std::string str2 = "hello worl2";
	char buffer_test[1024] = "";
	int32_t readed_size = 0;
	// buffer.WriteData(str.c_str(), str.size() + 1);
	// TDEBUG("buffer.data_size:" << buffer.PeekDataSize() << ", space:" << buffer.GetSpace() << ", restspace:" << buffer.GetRestSpace());
	// int32_t readed_size = buffer.ReadData(buffer_test, 1024, true);
	// TDEBUG("PeekData buffer.data_size:" << buffer.PeekDataSize() << ", space:" << buffer.GetSpace() << ", restspace:" << buffer.GetRestSpace()
	// 	<< ",readed_size:" << readed_size);
	// // buffer.GrowSpace(10);
	// // TDEBUG("GrowSpace buffer.data_size:" << buffer.PeekDataSize() << ", space:" << buffer.GetSpace() << ", restspace:" << buffer.GetRestSpace());
	// TDEBUG("buffer_test:" << buffer_test);
	// readed_size = buffer.ReadData(buffer_test, 1024);
	// TDEBUG("readed_size222:" << readed_size);
	// TDEBUG("buffer_test222:" << buffer_test);
	// TDEBUG("ReadData buffer.data_size:" << buffer.PeekDataSize() << ", space:" << buffer.GetSpace() << ", restspace:" << buffer.GetRestSpace());
	// buffer.WriteData(str2.c_str(), str2.size() + 1);
	// readed_size = buffer.ReadData(buffer_test, 1024);
	// TDEBUG("buffer_test2:" << buffer_test);
	// TDEBUG("ReadData22 buffer.data_size:" << buffer.PeekDataSize() << ", space:" << buffer.GetSpace() << ", restspace:" << buffer.GetRestSpace());

	// std::string str3 = "6789";
	// std::string str4 = "12345";
	// CycleBuffer<char> buffer2(10);
	// buffer2.WriteData("6789", 4);
	// TDEBUG("WriteData:" << buffer2.PeekDataSize() 
	// 	<< ", space:" << buffer2.GetSpace() 
	// 	<< ", restspace:" << buffer2.GetRestSpace()
	// 	<< ", read_index:" << buffer2.GetReadIndex()
	// 	<< ", write_index:" << buffer2.GetWriteIndex());
	// buffer2.WriteDataToHead("12345", 5);
	// TDEBUG("WriteDataToHead:" << buffer2.PeekDataSize() 
	// 	<< ", space:" << buffer2.GetSpace() 
	// 	<< ", restspace:" << buffer2.GetRestSpace()
	// 	<< ", read_index:" << buffer2.GetReadIndex()
	// 	<< ", write_index:" << buffer2.GetWriteIndex());
	// readed_size = buffer2.ReadData(buffer_test, 1024);
	// buffer_test[9] = '\0';
	// TDEBUG("buffer_test2:" << buffer_test);
	// TDEBUG("ReadData4 buffer.data_size:" << buffer2.PeekDataSize() 
	// 	<< ", space:" << buffer2.GetSpace() 
	// 	<< ", restspace:" << buffer2.GetRestSpace()
	// 	<< ", read_index:" << buffer2.GetReadIndex()
	// 	<< ", write_index:" << buffer2.GetWriteIndex());

	CycleBuffer<char> buffer3(11);
	buffer3.WriteData("123456789", 9);
	readed_size = buffer3.ReadData(buffer_test, 6);
	buffer3.WriteDataToHead("123456", 6);
	
	TDEBUG("WriteDataToHead:" << buffer3.PeekDataSize() 
		<< ", space:" << buffer3.GetSpace() 
		<< ", restspace:" << buffer3.GetRestSpace()
		<< ", read_index:" << buffer3.GetReadIndex()
		<< ", write_index:" << buffer3.GetWriteIndex());

	readed_size = buffer3.ReadData(buffer_test, 1024);
	buffer_test[9] = '\0';
	TDEBUG("buffer_test4:" << buffer_test);
	TDEBUG("ReadData4 buffer.data_size:" << buffer3.PeekDataSize() 
		<< ", space:" << buffer3.GetSpace() 
		<< ", restspace:" << buffer3.GetRestSpace()
		<< ", read_index:" << buffer3.GetReadIndex()
		<< ", write_index:" << buffer3.GetWriteIndex());

	// buffer3.WriteDataToHead("0", 1);
	// readed_size = buffer3.ReadData(buffer_test, 1024);
	// buffer_test[10] = '\0';

	// TDEBUG("buffer_test5:" << buffer_test);
	// TDEBUG("ReadData5 buffer.data_size:" << buffer3.PeekDataSize() 
	// 	<< ", space:" << buffer3.GetSpace() 
	// 	<< ", restspace:" << buffer3.GetRestSpace()
	// 	<< ", read_index:" << buffer3.GetReadIndex()
	// 	<< ", write_index:" << buffer3.GetWriteIndex());

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


  	system_clock::time_point tp = system_clock::now();
  	system_clock::duration dtn = tp.time_since_epoch();

  	TDEBUG("current time since epoch, expressed in:");
  	TDEBUG("periods: " << dtn.count());
  	TDEBUG("seconds: " << dtn.count() * system_clock::period::num / system_clock::period::den / 1000);
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

static void test_server_time()
{
	g_ServerTime.UpdateTime();
	std::time_t time_tamp = g_ServerTime.NowTime();
	TDEBUG("cur_time:" << time_tamp << ", str:" << ctime(&time_tamp));
	TDEBUG("cur_mtime:" << g_ServerTime.NowTimeMs());
	TDEBUG("cur_utime:" << g_ServerTime.NowTimeUs())
}

static void test_net_message()
{
  TDEBUG("------------ msg with cycle_buffer ------------");
	char send_buffer[1024] = "";
	char recv_buffer[1024] = "";
	CycleBuffer<char> buffer2(1024);
	buffer2.SetReadIndex(1013);
	buffer2.SetWriteIndex(1013);
	// 开始写数据
	NetMessage test_msg(1, 1);
	std::string test_content = "hello req msg server!";
	test_msg.SetContent(test_content);
	test_msg.Serialize(send_buffer, sizeof(send_buffer));

	TDEBUG("msg_serialized_size:" << test_msg.SerializeByteNum())
	buffer2.WriteData(send_buffer, 53);
	TDEBUG("buffer2.readindex:" << buffer2.GetReadIndex()
		<< ", buffer2.write_index:" << buffer2.GetWriteIndex()
		<< ", buffer2.data_size:" << buffer2.PeekDataSize());

	buffer2.ReadData(recv_buffer, 53);
	NetMessage rep_msg;
	rep_msg.UnSerialize(recv_buffer, 53);
	TDEBUG("rep_msg.msg_class:" << rep_msg.GetMsgClass()
		<< ", rep_msg.msg_type:" << rep_msg.GetMsgType());
}

static void test_jsconcpp()
{
	const char* str = "{\"uploadid\": \"UP000000\",\"code\": 100,\"msg\": \"\",\"files\": \"\"}";  

    Json::Reader reader;  
    Json::Value root;  
    if (reader.parse(str, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素  
    {  
        std::string upload_id = root["uploadid"].asString();  // 访问节点，upload_id = "UP000000"  
        int code = root["code"].asInt();    // 访问节点，code = 100 
        TDEBUG("parse json string, code = " << code << ", uploadid =" << upload_id);
    }
    else 
    {
    	TERROR("parse json string failed");
    }
}

static void test_mysqlconnector()
{
	TDEBUG("test mysql connector");
	try
	{
		sql::Driver *driver;
		sql::Connection *con = nullptr;
		sql::Statement *stmt = nullptr;
		sql::ResultSet *res = nullptr;
		sql::PreparedStatement *pstmt = nullptr;

		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "1q2w3e4r");
		if (!con)
		{
			TERROR("connect database failed");
			return;
		}
		TDEBUG("connect database success");
		con->setSchema("test_db");
		stmt = con->createStatement();
		res = stmt->executeQuery("SELECT * from test_tb");
		while (res->next())
		{
			int32_t id = res->getInt64(1);
			std::string str = res->getString(2);
			TDEBUG("read one data, id=" << id << ", str=" << str);
		}
		delete stmt;
		delete res;
		delete con;
		// delete driver;
	}
	catch (sql::SQLException &e)
	{
		TDEBUG("# ERR: " << e.what());
		TDEBUG(" (MySQL error code: " << e.getErrorCode());
		TDEBUG("SQLState: " << e.getSQLState() << " )");
	}
}

void test_protos()
{
	TDEBUG("test protos");
	tprotos::user_info_t pb_user;
	pb_user.set_id(1);
	pb_user.set_name("太极");
	std::string str;
	pb_user.SerializeToString(&str);

	tprotos::user_containers_t map_users;
	auto pb_map = map_users.mutable_users();
	if (!pb_map)
	{
		TERROR("pb_map is nullptr");
	}
	else
	{
		auto &user_map = *pb_map;
		user_map[1] = pb_user;
		for (auto it = user_map.begin(); it != user_map.end(); ++it)
		{
			TDEBUG("pb_key:" << it->first << ", user.id=" << it->second.id());
		}
	}
	// 存入数据库
	try
	{
		sql::Driver *driver;
		sql::Connection *con = nullptr;
		sql::PreparedStatement *pstmt = nullptr;
		sql::PreparedStatement *p_stmp_read = nullptr;
		sql::ResultSet *res = nullptr;
		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "1q2w3e4r");
		if (!con)
		{
			TERROR("connect database failed");
			return;
		}
		TDEBUG("connect database success");
		con->setSchema("test_db");
		pstmt = con->prepareStatement("update test_tb set blob_data= ? where id = 1");
		pstmt->setString(1,str);
		pstmt->executeUpdate();
		TDEBUG("write data to db success");
		// 读出来反序列化
		p_stmp_read = con->prepareStatement("select blob_data from test_tb where id = 1");
		res = p_stmp_read->executeQuery();
		while(res->next())
		{
			std::string str_blob = res->getString(1);
			tprotos::user_info_t pb_user;
			pb_user.ParseFromString(str_blob);
			TDEBUG("unserialize blob proto_user");
			TDEBUG("pb_user_info:" << pb_user.ShortDebugString());
		}
		delete p_stmp_read;
		delete pstmt;
		delete con;
		
		// delete driver;
	}
	catch (sql::SQLException &e)
	{
		TDEBUG("# ERR: " << e.what());
		TDEBUG(" (MySQL error code: " << e.getErrorCode());
		TDEBUG("SQLState: " << e.getSQLState() << " )");
	}
}

void test_log4cplus()
{
	TDEBUG("test_log4cplus")
	// static log4cplus::Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("log"));
	log4cplus::Logger logger = log4cplus::Logger::getRoot();
	log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log_config/test_log.properties"));
	// LOG4CPLUS_TRACE_METHOD(logger, LOG4CPLUS_TEXT("::printDebug()"));
	LOG4CPLUS_DEBUG(logger, "This is a DEBUG message");
	LOG4CPLUS_INFO(logger, "This is a INFO message");
	LOG4CPLUS_WARN(logger, "This is a WARN message");
	LOG4CPLUS_ERROR(logger, "This is a ERROR message");
	LOG4CPLUS_FATAL(logger, "This is a FATAL message");

	g_ServerLog.Init("log_config/test_log.properties");
	TWARN("hello");
	// LOG4CPLUS_WARN(g_ServerLog.GetLogger(), "This is a WARN message");
}

void test_load_jsonfile()
{
	std::map<int32_t, std::string> node_type_des;  // key:node_type value:node_name
    std::map<int32_t, std::string> msg_class_des;  // key:msg_class value:msg_class_name
    std::map<int32_t, std::map<int32_t, std::string> > msg_type_des; // key1: msg_class key2:msg_type  vlaue:msg_type_name
	// server_des.json
    {
        Json::Reader reader;
        std::ifstream fs;
        Json::Value server_des_jv;
        fs.open("server_des.json", std::ios::binary);
        if (!fs.is_open())
        {
            TERROR("open server_des.json failed");
            return;
        }
        if (!reader.parse(fs, server_des_jv, false))
        {
            fs.close();
            TERROR("parse server_des.json failed");
            return;
        }
        TINFO("parce server_des.json success")
        if (server_des_jv.isMember("node_des"))
        {
            auto &node_des_jv = server_des_jv["node_des"];
            for (int32_t i = 0; i < node_des_jv.size(); ++i)
            {
                int32_t node_type = node_des_jv[i]["node_type"].asInt();
                std::string node_name = node_des_jv[i]["node_name"].asString();
                node_type_des[node_type] = node_name;
            }
        }
        fs.close();
    }
    {
        // protocol_des.json
        Json::Reader reader;
        std::ifstream fs;
        Json::Value protocol_des_jv;
        fs.open("protocol_des.json", std::ios::binary);
        if (!fs.is_open())
        {
            TERROR("protocol_des.json failed");
            return;
        }
        if (!reader.parse(fs, protocol_des_jv, false))
        {
            fs.close();
            TERROR("parse protocol_des.json failed");
            return;
        }
        TINFO("parce protocol_des.json success")
        if (protocol_des_jv.isMember("proto_class_des"))
        {
            auto &msg_class_jv = protocol_des_jv["proto_class_des"];
            for (int32_t i = 0; i < msg_class_jv.size(); ++i)
            {
                int32_t msg_class = msg_class_jv[i]["msg_class"].asInt();
                std::string msg_class_name = msg_class_jv[i]["class_des"].asString();
                msg_class_des[msg_class] = msg_class_name;
                // msg types
                auto & msg_type_jv = msg_class_jv[i]["msg_type"];
                for (int32_t j = 0; j < msg_type_jv.size(); ++j)
                {
                    int32_t msg_type = msg_type_jv[j]["type"].asInt();
                    std::string type_name = msg_type_jv[j]["des"].asString();
                    msg_type_des[msg_class][msg_type] = type_name;
                }
            }
        }
        fs.close();
    }
	return;
}

int main(int argc, char* argv[])
{
 	std::cout << "hello test" << std::endl;
	int64_t a = 10;
	auto test_fun = [b{a}](){
	  std::cout << "b = " << b << std::endl;
	};
	TDEBUG("hello tr servers test");
	test_log4cplus();

	test_cycle_buffer();
	// test_socket();
	// test_objpool();
	// test_chrono();
	// test_func_tools();
	// test_server_time();
	// test_net_message();
	// test_jsconcpp();
	// test_mysqlconnector();
	// test_protos();
	test_load_jsonfile();

	return 0;
}
