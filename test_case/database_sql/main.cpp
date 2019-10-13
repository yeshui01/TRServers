/*=======================================================
# Author       : mknight
# Last modified: 2019-09-30 12:25
# Email        : 824338670@qq.com
# Filename     : main.cpp
# Description  : 
# =======================================================*/

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

// database
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/datatype.h>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/loggingmacros.h>

#include "tr_database/data_table_item.h"
#include "data_pb_common.pb.h"
#include "data_pb_tools.h"
#include "tr_database/database_helper.h"
#define TEST_BASE_PSWD "1q2w3e4r"

///////////////////////////////////////////////

class TestDBEvent : public TRAsyncDBEvent
{
public:
    TestDBEvent(int64_t id) : TRAsyncDBEvent()
    {
        data_id_ = id;
    };
    ~TestDBEvent()
    {
        TDEBUG("test db event destruct, worker_id:" << worker_id_);
    }

    int32_t Run(std::vector<DataTableItem> * &local_result_pt) override;
public:
    int64_t data_id_ = 0;
};
int32_t TestDBEvent::Run(std::vector<DataTableItem> * &local_result_pt)
{
    try
    {
        {
            std::shared_ptr<sql::PreparedStatement> pstmt(conn_->prepareStatement("select * from test_tb where id = ?"));
            pstmt->setInt64(1, data_id_);
            
            TDEBUG("TestDBEvent::Run, dataid:" << data_id_
                                               << ", pstmtptr:" << pstmt.get()
                                               << ", worker_id:" << worker_id_);
            std::shared_ptr<sql::ResultSet> results(pstmt->executeQuery());
            // result_ptr_.reset();
            // result_ptr_ = pstmt->executeQuery();
            local_result_pt = new std::vector<DataTableItem>;
            TRDataBase::ResultSetConvt(results.get(), *local_result_pt);
        }
    }
    catch (sql::SQLException &e)
    {
        TERROR("mysql err_code:" << e.getErrorCode() << ", err:" << e.what() << ", sql_state:" << e.getSQLState());
        return e.getErrorCode();
    }
    return 0;
}

void test_single_run()
{
    TRDataBase one_database;
    
    if (!one_database.Init("localhost", "root", TEST_BASE_PSWD, "test_db", 0))
    {
        TERROR("init data base failed");
        return;
    }
    TDEBUG("------------ begin single dbtest ------------");
    std::stringstream sql;
    std::vector<DataTableItem> v_local_set;
    for (int32_t i = 1; i <= 1; ++i)
    {
        sql << "select * from test_tb where id = " << i;
        one_database.Query(sql.str(), v_local_set);
        // auto results = one_database.Query(sql.str());
        // while (results->next())
        // {
        //     int64_t id = results->getInt64(1);
        //     TINFO("result, id:" << id << ", str_data:" << results->getString(2));
        // }

        for (auto && v : v_local_set)
        {
            TINFO("single_test_result, id:" << v.GetFieldBigIntValue(0) << ", str_data:" << v.GetFieldStringValue(1));
        }
        sql.str("");
        sql.clear();
        v_local_set.clear();
    }

    TDEBUG("------------ end single dbtest ------------");
}

int32_t test_thread_db()
{
    TINFO("HELLO database sql");
    TRDataBase one_database;
    if (!one_database.Init("localhost", "root", TEST_BASE_PSWD, "test_db", 2))
    {
        TERROR("init data base failed");
        return 0;
    }
    one_database.StartWorkers();
    // std::this_thread::sleep_for(std::chrono::seconds(2));
    std::vector<DataTableItem> v_result;
    int64_t max_id = 0;
    one_database.Query("select MAX(id) from test_tb", v_result);
    
    for (auto && v : v_result)
    {
        int64_t id = v.GetFieldBigIntValue(0);
        if (max_id < id)
            max_id = id;
    }

    // auto results_pt = one_database.Query("select * from test_tb");
    // if (!results_pt)
    // {
    //     TERROR("result set is nullptr");
    //     return 0;
    // }
    // int64_t max_id = 0;
    // while(results_pt->next())
    // {
    //     int64_t id = results_pt->getInt64(1);
    //     TINFO("result, id:" << id << ", str_data:" << results_pt->getString(2));
    //     if (id > max_id)
    //     {
    //         max_id = id;
    //     }
    // }
    int32_t test_max_data_size = 200;
    // 添加测试100条数据
    std::stringstream ss_sql;
    while (max_id < test_max_data_size)
    {
        ++max_id;
        ss_sql << "insert into test_tb(id, str_data, blob_data) values(" << max_id << "," << "\'teststr" << max_id << "\'," << "\'xx\');";
        TDEBUG("ss_sql:" << ss_sql.str());
        one_database.Execute(ss_sql.str());
        ss_sql.str("");
        ss_sql.clear();
    }

    // test add async db event
    TDEBUG("------------ begin dbtest ------------");
    for (int32_t i = 1; i <= test_max_data_size; ++i)
    {
        std::shared_ptr<TestDBEvent> test_evt(new TestDBEvent(i));
        one_database.AddAsyncDBEvent(test_evt, [](int32_t evt_code, std::vector<DataTableItem> *results) {
            if (results)
            {
                // while (results->next())
                // {
                //     int64_t id = results->getInt64(1);
                //     TINFO("result, id:" << id << ", str_data:" << results->getString(2));
                // }

                for (auto && one_row : *results)
                {
                    // TDEBUG("GetFieldsSize:" << one_row.GetFieldsSize());
                    TINFO("threadresult, id:" << one_row.GetFieldBigIntValue(0) << ", str_data:" << one_row.GetFieldStringValue(1));
                }
            }
            TDEBUG("db evt cbfinished, evt_code:" << evt_code << ", results:" << reinterpret_cast<int64_t>(results));
        });
    }
    
    while (!one_database.CallBackTaskEmpty())
    {
        one_database.EventRun();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        TDEBUG("one_database.EventRun, cbtask_size:" << one_database.CallBackTaskSize());
    }
    TDEBUG("------------ end dbtest ------------");
    one_database.StopWorkers();
    return 0;
}
void test_datatable_item()
{
    DataTableItem test_item(1);
    int32_t index0 = test_item.AddField(E_FIELD_VALUE_TYPE_BOOL, false);
    test_item.SetFieldBoolValue(index0, true);
    db_protos::pb_table_item pb_item;
    DbProtoTools::DumpToOnePbTbItem(&test_item, &pb_item);
    TDEBUG("xxxx0:" << pb_item.ShortDebugString());

    int32_t index1 = test_item.AddField(E_FIELD_VALUE_TYPE_BIG_INT, false);
    db_protos::pb_table_item pb_item1;
    test_item.SetFieldBigIntValue(index1, 13456789);
    DbProtoTools::DumpToOnePbTbItem(&test_item, &pb_item1);
    TDEBUG("xxxx1:" << pb_item1.ShortDebugString());


    int32_t index2 = test_item.AddField(E_FIELD_VALUE_TYPE_STRING, false);
    std::string str = "hello test pb dbdata";
    test_item.SetFieldStringValue(index2, str.c_str(), str.size());
    db_protos::pb_table_item pb_item2;
    DbProtoTools::DumpToOnePbTbItem(&test_item, &pb_item2);
    TDEBUG("xxxx2:" << pb_item2.ShortDebugString());

    TDEBUG("-----------------------------------------------------------------------");
    {
        DataTableItemMgr<DataTableItem, int32_t, 1> test_tb_item_mgr;
        auto one_item = test_tb_item_mgr.AddItem(1);
        if (one_item)
        {
            int32_t index0 = one_item->AddField(E_FIELD_VALUE_TYPE_BOOL, false);
            one_item->SetFieldBoolValue(index0, true);
            db_protos::pb_table_item pb_item;
            DbProtoTools::DumpToOnePbTbItem(one_item, &pb_item);
            TDEBUG("xxxx0:" << pb_item.ShortDebugString());

            int32_t index1 = one_item->AddField(E_FIELD_VALUE_TYPE_BIG_INT, false);
            one_item->SetFieldBigIntValue(index1, 13456789);
            db_protos::pb_table_item pb_item1;
            DbProtoTools::DumpToOnePbTbItem(one_item, &pb_item1);
            TDEBUG("xxxx1:" << pb_item1.ShortDebugString());

            std::string str2 = "hello test pb dbdata";

            int32_t index2 = one_item->AddField(E_FIELD_VALUE_TYPE_STRING, false);
            one_item->SetFieldStringValue(index2, str2.c_str(), str2.size());
            db_protos::pb_table_item pb_item2;
            DbProtoTools::DumpToOnePbTbItem(one_item, &pb_item2);
            TDEBUG("xxxx2:" << pb_item2.ShortDebugString());

            std::string str3 = "hsdlfjsdlkjf";
            int32_t index3 = one_item->AddField(E_FIELD_VALUE_TYPE_STRING, false);
            one_item->SetFieldStringValue(index3, str3.c_str(), str3.size());
            db_protos::pb_table_item pb_item3;
            DbProtoTools::DumpToOnePbTbItem(one_item, &pb_item3);
            TDEBUG("xxxx3:" << pb_item3.ShortDebugString());

            db_protos::pb_data_module pb_module;
            DbProtoTools::DumpToPbTableItems(test_tb_item_mgr, &pb_module);
            TINFO("pb_data_module:" << pb_module.ShortDebugString());

            db_protos::pb_table_item pb_item_final;
            DbProtoTools::DumpToOnePbTbItem(one_item, &pb_item_final);
            TDEBUG("testpbstri_final:" << pb_item_final.ShortDebugString());
        }
    }
}

void error_test()
{
    DataTableItem new_data2(1);
    new_data2.AddField(E_FIELD_VALUE_TYPE_BIG_INT, false);
    new_data2.SetFieldBigIntValue(0, 1); // mysqlconnector下标从1开始

    new_data2.AddField(E_FIELD_VALUE_TYPE_STRING, false);
    new_data2.SetFieldStringValue(1, "hello world"); // mysqlconnector下标从1开始

    new_data2.AddField(E_FIELD_VALUE_TYPE_STRING, false);
    new_data2.SetFieldStringValue(2, "xxxxx"); // mysqlconnector下标从1开始
    TDEBUG("field1:" << new_data2.GetFieldBigIntValue(0));
    TDEBUG("field2:" << new_data2.GetFieldStringValue(1));
}

int main(int argc, char* argv[])
{
    g_ServerLog.Init("log_config/test_case_log.properties");
    test_thread_db();
    test_single_run();
    // error_test();
    return 0;
}