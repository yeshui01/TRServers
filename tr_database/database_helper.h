/*=======================================================
# Author       : mknight
# Last modified: 2019-10-02 14:45
# Email        : 824338670@qq.com
# Filename     : database_helper.h
# Description  : 
# =======================================================*/

#ifndef __TR_DATABASE_HELPER_H__
#define __TR_DATABASE_HELPER_H__

#include "data_table_item.h"

#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <future>
#include <map>
// sql connector header
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

// 数据库操作类型
enum EDababaseOperation
{
    E_DATABASE_OPER_INSERT = 1, // 插入
    E_DATABASE_OPER_UPDATE = 2, // 更新
    E_DATABASE_OPER_DELETE = 3, // 删除
    E_DATABASE_OPER_QUERY = 4,  // 查询
};

class TRDataBase;
class TRDBPrepareStmtMgr;
// 数据库异步事件基类
class TRAsyncDBEvent
{
public:
    friend class TRDataBase;
    friend class TRAsyncDBCallBackTask;
    TRAsyncDBEvent();
    virtual ~TRAsyncDBEvent();

    virtual int32_t Run(std::vector<DataTableItem> * &local_result_pt);
protected:
    void AttatchConnect(sql::Connection * conn, TRDBPrepareStmtMgr * pre_stmt_mgr);

protected:
    sql::Connection * conn_ = nullptr;              // 数据库连接
    TRDBPrepareStmtMgr * pre_stmt_mgr_;             // 预处理语句库
    int32_t worker_id_ = 0;                         // 所在的worker标识
};


//////////////////////////////////////////////////////[数据库操作]/////////////////////////////////////////////////////////////
// 预处理语句容器
class TRDBPrepareStmtMgr
{
public:
    TRDBPrepareStmtMgr();
    ~TRDBPrepareStmtMgr();

    sql::PreparedStatement * HoldStatement(int32_t id);
    void AddPreStmt(int32_t id, std::shared_ptr<sql::PreparedStatement> pre_stmt);
protected:
    std::map<int32_t, std::shared_ptr<sql::PreparedStatement> > pre_stmts_; // key:语句标识id value:预处理语句对象
};

using async_dbevnt_callback_t = std::function<void(int32_t evt_code, std::vector<DataTableItem> * results)>;
using async_dbevnt_fut_t = std::future<int32_t>;
class TRAsyncDBCallBackTask
{
public:
    TRAsyncDBCallBackTask() = delete;
    TRAsyncDBCallBackTask(const TRAsyncDBCallBackTask &) = delete;
    TRAsyncDBCallBackTask& operator=(const TRAsyncDBCallBackTask&) = delete;

    TRAsyncDBCallBackTask(async_dbevnt_fut_t && fut, std::shared_ptr<TRAsyncDBEvent> evt, async_dbevnt_callback_t &&cb_fn)
    :evt_fut(std::move(fut)),db_evt(evt), cb_evt(cb_fn)
    {
    }
    async_dbevnt_fut_t evt_fut;                          // 异步事件结果
    std::shared_ptr<TRAsyncDBEvent> db_evt;              // 异步事件
    async_dbevnt_callback_t cb_evt;                      // 回调事件
    std::vector<DataTableItem> * result_ptr = nullptr;
};

class TRAsyncDBTask
{
public:
    friend class TRDataBase;
    TRAsyncDBTask(TRAsyncDBEvent * evnt_pt);
    virtual ~TRAsyncDBTask();

protected:
    TRAsyncDBEvent * evnt_pt_ = nullptr;
    std::promise<int32_t> evnt_prom_;     // 异步事件完成状态承诺
    // TRDBPrepareStmtMgr * pre_stmt_mgr_;
    TRAsyncDBCallBackTask * cb_pt_ = nullptr;
};
// 数据库操作代理类
class TRDataBase
{
public:
    TRDataBase();
    ~TRDataBase();

    bool Init(std::string host, std::string user_name, std::string pwd, std::string db_name, int32_t worker_size);
    // 每帧事件循环
    void EventRun();
    // 添加一个异步事件
    void AddAsyncDBEvent(std::shared_ptr<TRAsyncDBEvent> db_event, async_dbevnt_callback_t && cb_fun);
    // 线程工作函数
    void WorkerRun(int32_t worker_id);
    // 开启线程运行
    void StartWorkers();
    // 等待所有线程运行
    void WaitWorkers();
    // 结束所有后台线程
    void StopWorkers();
    // 同步执行sql语句,主线程中调用
    void Query(const std::string & sql_str, std::vector<DataTableItem> & v_local_set);
    bool Execute(const std::string & sql_str);
    int32_t ExecuteUpdate(const std::string & sql_str);
    // 获取主连接器
    sql::Connection *HoldMainConnect();
    // 回调任务队列是否为空
    bool CallBackTaskEmpty();
    size_t CallBackTaskSize();
    // 设置所有连接使用utf8字符集
    void SetConnsUtf8Char();
    // 初始安装预处理语句
    void InstallPreStmts(std::function<void (sql::Connection *, TRDBPrepareStmtMgr *) > && install_func);
    // 获取主连接的预处理语句
    sql::PreparedStatement *HoldMainPreStmt(int32_t stmt_id);
public:
    // 查询结果转换到本地结果集
    static void ResultSetConvt(sql::ResultSet * meta_result, std::vector<DataTableItem> & v_local_set);
protected:
    sql::Connection * HoldConnect(int32_t index);
    void WorkerReady(int32_t worker_id);
    TRDBPrepareStmtMgr *HoldPreStmtMgr(int32_t index);
protected:
    std::string db_name_;   // 数据库名
    std::string user_;      // 用户名
    std::string pswd_;      // 密码
    std::string host_;      // 主机地址
    std::vector<std::shared_ptr<sql::Connection> > conns_;  // 连接器池
    std::vector<std::shared_ptr<TRDBPrepareStmtMgr> > conn_prestmt_mgrs_; // 连接预处理语句管理器
protected:
    std::deque<TRAsyncDBTask *> db_event_queue_;            // 数据库异步任务队列
    std::mutex evt_queue_mutex_;                            // 异步事件队列互斥锁
    std::condition_variable evt_queue_con_var_;             // 异步事件队列条件变量
    std::vector<std::thread> workers_;                      // 工作线程
    bool worker_stop_ = false;

    int32_t ready_workers_ = 0;                             // 已经就绪的worker数量
    std::condition_variable worker_cond_var_;                // 工作线程条件变量
    std::mutex worker_mutex_;                               // 工作线程的互斥量
protected:
    std::list<TRAsyncDBCallBackTask> callback_task_queue_; // 异步事件回调队列
};

// ////////////////////////////////////////////////////////[数据库管理助手]//////////////////////////////////////////////////////////
// 数据库管理器助手
class TRDataBaseHelper
{
public:
    TRDataBaseHelper();
    ~TRDataBaseHelper();
    // 添加一个数据库对象
    bool AddDataBase(int32_t database_id, std::shared_ptr<TRDataBase> database_ptr);
    // 获取一个数据库对象
    TRDataBase * HoldDataBase(int32_t database_id);
    // 每帧运行
    void FrameUpdate();
    // 停止所有数据库服务
    void Stop();
protected:
    std::map<int32_t, std::shared_ptr<TRDataBase> > databases_; // key:database_id value: database ptr
};

#endif // __TR_DATABASE_HELPER_H__