/*=======================================================
# Author       : mknight
# Last modified: 2019-10-02 14:45
# Email        : 824338670@qq.com
# Filename     : database_helper.cpp
# Description  : 
# =======================================================*/

#include "database_helper.h"
#include "tr_log/log_module.h"
#include "common_define.h"

TRAsyncDBEvent::TRAsyncDBEvent()
{

}

TRAsyncDBEvent::~TRAsyncDBEvent()
{

}

void TRAsyncDBEvent::AttatchConnect(sql::Connection * conn, TRDBPrepareStmtMgr * pre_stmt_mgr)
{
    conn_ = conn;
    pre_stmt_mgr_ = pre_stmt_mgr;
}

int32_t TRAsyncDBEvent::Run(std::vector<DataTableItem> * &local_result_pt)
{
    // DO NOTHING
    return 0;
}


////////////////////////////////////////////////////////////[数据库操作类]//////////////////////////////////////////////////////////////////
TRDBPrepareStmtMgr::TRDBPrepareStmtMgr()
{

}

TRDBPrepareStmtMgr::~TRDBPrepareStmtMgr()
{

}

sql::PreparedStatement * TRDBPrepareStmtMgr::HoldStatement(int32_t id)
{
    auto it = pre_stmts_.find(id);
    if (it != pre_stmts_.end())
    {
        return it->second.get();
    }
    
    return nullptr;
}
void TRDBPrepareStmtMgr::AddPreStmt(int32_t id, std::shared_ptr<sql::PreparedStatement> pre_stmt)
{
    auto it = pre_stmts_.find(id);
    if (it == pre_stmts_.end())
    {
        pre_stmts_.insert({id, pre_stmt});
    }
}

TRAsyncDBTask::TRAsyncDBTask(TRAsyncDBEvent * evnt_pt)
{
    evnt_pt_ = evnt_pt;
}

TRAsyncDBTask::~TRAsyncDBTask()
{
}

TRDataBase::TRDataBase()
{

}

TRDataBase::~TRDataBase()
{
    WaitWorkers();
    for (auto it = db_event_queue_.begin(); it != db_event_queue_.end(); ++it)
    {
        SAFE_DELETE_PTR(*it);
    }
    db_event_queue_.clear();
    callback_task_queue_.clear();
}

bool TRDataBase::Init(std::string host, std::string user_name, std::string pwd, std::string db_name, int32_t worker_size)
{
    db_name_ = db_name;
    user_ = user_name;
    pswd_ = pwd;
    host_ = host;
    if (worker_size < 0)
    {
        worker_size = 0;
    }
    TINFO("database init, host:" << host << ", user_name:" << user_name << ", pswd:" << pswd_ << ", db_name:" << db_name_ << ", worker_size:" << worker_size);
    auto db_driver = get_driver_instance();
    for (int32_t i = 0; i < worker_size + 1; ++i)
    {
        // 设计上一个worker使用一个连接器,至少要一个主连接器,因此这里使用  worker_size + 1
        auto db_conn = db_driver->connect(host_, user_, pswd_);
        if (!db_conn)
        {
            TERROR("connect data base failed");
            return false;;
        }
        db_conn->setSchema(db_name_);
        conns_.push_back(std::shared_ptr<sql::Connection>(db_conn));
        std::shared_ptr<TRDBPrepareStmtMgr> prestmt_mgr(new TRDBPrepareStmtMgr());
        conn_prestmt_mgrs_.push_back(prestmt_mgr);
    }
    // if (worker_size > 0)
    // {
    //     db_driver->threadInit();
    // }
    return true;
}

bool TRDataBase::Init2(std::string host, int32_t port, std::string user_name, std::string pswd, std::string db_name, int32_t worker_size)
{
    // "tcp://192.168.16.19:3306", "root", "1q2w3e4R@ttt"
    db_name_ = db_name;
    user_ = user_name;
    pswd_ = pswd;
    host_ = host;
    if (worker_size < 0)
    {
        worker_size = 0;
    }
    std::stringstream ss_connect;
    ss_connect << "tcp://" << host << ":" << port;
    TINFO("database init, host:" << host << ", user_name:" << user_name << ", pswd:" << pswd_ << ", db_name:" << db_name_ << ", worker_size:" << worker_size);
    auto db_driver = get_driver_instance();
    for (int32_t i = 0; i < worker_size + 1; ++i)
    {
        // 设计上一个worker使用一个连接器,至少要一个主连接器,因此这里使用  worker_size + 1
        // auto db_conn = db_driver->connect(host_, user_, pswd_);
        auto db_conn = db_driver->connect(ss_connect.str(), user_name, pswd);
        if (!db_conn)
        {
            TERROR("connect data base failed");
            return false;;
        }
        db_conn->setSchema(db_name_);
        conns_.push_back(std::shared_ptr<sql::Connection>(db_conn));
        std::shared_ptr<TRDBPrepareStmtMgr> prestmt_mgr(new TRDBPrepareStmtMgr());
        conn_prestmt_mgrs_.push_back(prestmt_mgr);
    }
    // if (worker_size > 0)
    // {
    //     db_driver->threadInit();
    // }
    return true;
}

sql::Connection * TRDataBase::HoldConnect(int32_t index)
{
    if (index >= 0 && index < int32_t(conns_.size()))
    {
        return conns_[index].get();
    }
    return nullptr;
}

TRDBPrepareStmtMgr *TRDataBase::HoldPreStmtMgr(int32_t index)
{
    if (index >= 0 && index < int32_t(conn_prestmt_mgrs_.size()))
    {
        return conn_prestmt_mgrs_[index].get();
    }
    return nullptr;
}

void TRDataBase::WorkerReady(int32_t worker_id)
{
    std::unique_lock<std::mutex> lck(worker_mutex_);
    ++ready_workers_;
    worker_cond_var_.notify_all();
    TDEBUG("WorkerReady, worker_id:" << worker_id);
}
void TRDataBase::WorkerRun(int32_t worker_id)
{
    TRDBPrepareStmtMgr local_prestmt_mgr;
    std::unique_lock<std::mutex> queue_lock(evt_queue_mutex_);
    TDEBUG("db worker start run, worker_id:" << worker_id);
    auto db_driver = get_driver_instance();
    db_driver->threadInit();
    WorkerReady(worker_id);
    while (!worker_stop_)
    {
        evt_queue_con_var_.wait(queue_lock);
        queue_lock.unlock();
        while (!worker_stop_)
        {
            queue_lock.lock();
            if (db_event_queue_.empty())
            {
                break;
            }
            auto db_task_pt = db_event_queue_.front();
            TDEBUG("db worker fetch one task, worker_id:" << worker_id << ", task_pt:" << reinterpret_cast<int64_t>(db_task_pt));
            db_event_queue_.pop_front();
            queue_lock.unlock();

            auto conn_pt = HoldConnect(worker_id);
            if (conn_pt)
            {
                // 检查连接是否超时TODO:
                if (db_task_pt->evnt_pt_)
                {
                    db_task_pt->evnt_pt_->worker_id_ = worker_id;
                    auto prestmt_mgr = HoldPreStmtMgr(worker_id);
                    if (prestmt_mgr)
                    {
                        db_task_pt->evnt_pt_->AttatchConnect(conn_pt, &local_prestmt_mgr);
                    }
                    
                    auto r = db_task_pt->evnt_pt_->Run(db_task_pt->cb_pt_->result_ptr);

                    db_task_pt->evnt_prom_.set_value(r);
                }
            }
            else
            {
                TERROR("worker not found connector");
            }
            
            SAFE_DELETE_PTR(db_task_pt);
        }
    }
    db_driver->threadEnd();
}

void TRDataBase::EventRun()
{
    for (auto it = callback_task_queue_.begin(); it != callback_task_queue_.end();)
    {
        auto fut_status = it->evt_fut.wait_for(std::chrono::microseconds(0));
        if (std::future_status::ready == fut_status)
        {
            // 执行完毕
            TDEBUG("db event task finish, do cb event");
            it->cb_evt(it->evt_fut.get(), it->result_ptr);
            if (it->result_ptr)
                SAFE_DELETE_PTR(it->result_ptr);
            
            callback_task_queue_.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

void TRDataBase::AddAsyncDBEvent(std::shared_ptr<TRAsyncDBEvent> db_event, async_dbevnt_callback_t && cb_fun)
{
    if (conns_.size() < 2)
    {
        TERROR("not found db worker");
        return;
    }
    std::unique_lock<std::mutex> queue_lock(evt_queue_mutex_);
    TRAsyncDBTask * new_task = new TRAsyncDBTask(db_event.get());
    callback_task_queue_.emplace_back(new_task->evnt_prom_.get_future(), db_event, std::forward<async_dbevnt_callback_t>(cb_fun));
    new_task->cb_pt_ = &callback_task_queue_.back();
    new_task->evnt_pt_ = db_event.get();
    db_event_queue_.push_back(new_task);
    evt_queue_con_var_.notify_all();
    // return new_task->evnt_prom_.get_future();
}

void TRDataBase::StartWorkers()
{
    for (size_t i = 1; i < conns_.size(); ++i)
    {
        workers_.push_back(std::thread(&TRDataBase::WorkerRun, this, i));
    }
    if (conns_.size() > 1)
    {
        int32_t worker_num = conns_.size() - 1;
        std::unique_lock<std::mutex> lck (worker_mutex_);
        while (true)
        {
            if (ready_workers_ >= worker_num)
            {
                break;
            }
            else
            {
                worker_cond_var_.wait(lck);
            }
        }
    }

}

void TRDataBase::WaitWorkers()
{
    for (auto && w : workers_)
    {
        w.join();
    }
}
void TRDataBase::StopWorkers()
{
    worker_stop_ = true;
    evt_queue_con_var_.notify_all();
    WaitWorkers();
    workers_.clear();
}
sql::Connection *TRDataBase::HoldMainConnect()
{
    return HoldConnect(0);
}

void TRDataBase::Query(const std::string & sql_str, std::vector<DataTableItem> & v_local_set)
{
    auto conn = HoldMainConnect();
    if (!conn)
        return;
    
    try
    {
        std::shared_ptr<sql::Statement> stmt(conn->createStatement());
        std::shared_ptr<sql::ResultSet> result(stmt->executeQuery(sql_str));
        ResultSetConvt(result.get(), v_local_set);
    }
    catch (sql::SQLException &e)
    {
        // mysql err_code:2013, err:Lost connection to MySQL server during query, sql_state:HY000
        // mysql err_code:2006, err:MySQL server has gone away, sql_state:HY000
        TERROR("mysql err_code:" << e.getErrorCode() << ", err:" << e.what() << ", sql_state:" << e.getSQLState() << ",sql:" << sql_str);
        if (e.getErrorCode() == 2013 || e.getErrorCode() == 2006)
        {
            // 重连mysql
            if (conn->reconnect())
            {
                TWARN("reconnect mysql success");

                std::shared_ptr<sql::Statement> stmt(conn->createStatement());
                std::shared_ptr<sql::ResultSet> result(stmt->executeQuery(sql_str));
                ResultSetConvt(result.get(), v_local_set);
            }
            else
            {
                TERROR("reconnect mysql fail");
            }
            
        }
    }
}

bool TRDataBase::Execute(const std::string & sql_str)
{
    auto conn = HoldMainConnect();
    if (!conn)
        return false;
    
    try
    {
        std::shared_ptr<sql::Statement> stmt(conn->createStatement());
        return stmt->execute(sql_str);
    }
    catch (sql::SQLException &e)
    {
        TERROR("mysql err_code:" << e.getErrorCode() << ", err:" << e.what() << ", sql_state:" << e.getSQLState() << ",sql:" << sql_str);
        if (e.getErrorCode() == 2013 || e.getErrorCode() == 2006)
        {
            // 重连mysql
            if (conn->reconnect())
            {
                TWARN("reconnect mysql success2");

                std::shared_ptr<sql::Statement> stmt(conn->createStatement());
                return stmt->execute(sql_str);
            }
            else
            {
                TERROR("reconnect mysql fail2");
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    return false;
}

int32_t TRDataBase::ExecuteUpdate(const std::string & sql_str)
{
    auto conn = HoldMainConnect();
    if (!conn)
        return 0;
        
    try
    {
        std::shared_ptr<sql::Statement> stmt(conn->createStatement());
        return stmt->executeUpdate(sql_str);
    }
    catch (sql::SQLException &e)
    {
        TERROR("mysql err_code:" << e.getErrorCode() << ", err:" << e.what() << ", sql_state:" << e.getSQLState() << ",sql:" << sql_str);
        if (e.getErrorCode() == 2013 || e.getErrorCode() == 2006)
        {
            // 重连mysql
            if (conn->reconnect())
            {
                TWARN("reconnect mysql success3");

                std::shared_ptr<sql::Statement> stmt(conn->createStatement());
                return stmt->executeUpdate(sql_str);
            }
            else
            {
                TERROR("reconnect mysql fail3");
                return 0;
            }
        }
        else
        {
            return 0;
        }
        return 0;
        // return e.getErrorCode();
    }
    return 0;
}


bool TRDataBase::CallBackTaskEmpty()
{
    return callback_task_queue_.empty();
}
size_t TRDataBase::CallBackTaskSize()
{
    return callback_task_queue_.size();
}

void TRDataBase::ResultSetConvt(sql::ResultSet *meta_result, std::vector<DataTableItem> &v_local_set)
{
    if (!meta_result)
        return;
    while (meta_result->next())
    {
        auto meta_data = meta_result->getMetaData();
        int32_t new_index = int32_t(v_local_set.size());
        v_local_set.push_back(DataTableItem(0));
        auto &new_data = v_local_set[new_index];
        // TDEBUG("metadata clomun count:" << meta_data->getColumnCount());
        for (int32_t i = 0; i < int32_t(meta_data->getColumnCount()); ++i)
        {
            auto data_type = meta_data->getColumnType(i + 1);
            // TDEBUG("column_type:" << data_type << ", index:" << i);
            switch (data_type)
            {
            case sql::DataType::BIGINT:
            {
                new_data.AddField(E_FIELD_VALUE_TYPE_BIG_INT, false);
                new_data.SetFieldBigIntValue(i, meta_result->getInt64(i + 1)); // mysqlconnector下标从1开始
                break;
            }
            case sql::DataType::INTEGER:
            {
                new_data.AddField(E_FIELD_VALUE_TYPE_INT, false);
                new_data.SetFieldIntValue(i, meta_result->getInt(i + 1)); // mysqlconnector下标从1开始
                break;
            }
            case sql::DataType::VARCHAR:
            case sql::DataType::CHAR:
            case sql::DataType::LONGVARCHAR:
            {
                new_data.AddField(E_FIELD_VALUE_TYPE_STRING, false);
                new_data.SetFieldStringValue(i, meta_result->getString(i + 1)); // mysqlconnector下标从1开始
                break;
            }
            case sql::DataType::VARBINARY:
            case sql::DataType::BINARY:
            case sql::DataType::LONGVARBINARY:
            {
                new_data.AddField(E_FIELD_VALUE_TYPE_STRING, false);
                new_data.SetFieldStringValue(i, meta_result->getString(i + 1)); // mysqlconnector下标从1开始
                break;
            }
            case sql::DataType::REAL:
            case sql::DataType::DOUBLE:
            {
                new_data.AddField(E_FIELD_VALUE_TYPE_FLOAT, false);
                new_data.SetFieldFloatValue(i, meta_result->getDouble(i + 1)); // mysqlconnector下标从1开始
                break;
            }
            default:
            {
                TERROR("sql not support data type:" << int32_t(data_type) << ", index:" << i);
                break;
            }
            }
        }
    }
}

// 设置所有连接使用utf8字符集
void TRDataBase::SetConnsUtf8Char()
{
    for (auto & conn : conns_)
    {
        std::shared_ptr<sql::Statement> stmt(conn->createStatement());
        stmt->execute("set NAMES 'utf8'");
    }
}

// 初始安装预处理语句
void TRDataBase::InstallPreStmts(std::function<void (sql::Connection *, TRDBPrepareStmtMgr *) > && install_func)
{
    for (size_t i = 0; i < conns_.size() && i < conn_prestmt_mgrs_.size(); ++i)
    {
        install_func(conns_[i].get(), conn_prestmt_mgrs_[i].get());
    }
}

sql::PreparedStatement *TRDataBase::HoldMainPreStmt(int32_t stmt_id)
{
    if (conn_prestmt_mgrs_.empty())
    {
        return nullptr;
    }
    return conn_prestmt_mgrs_[0]->HoldStatement(stmt_id);
}


// 执行预处理语句
bool TRDataBase::ExecutePreStmt(sql::PreparedStatement * stmt_pt)
{
    try
    {
        return stmt_pt->execute();  // 更新数据库
    }
    catch (sql::SQLException &e)
    {
        TERROR("mysql err_code:" << e.getErrorCode() << ", err:" << e.what() << ", sql_state:" << e.getSQLState());
        if (e.getErrorCode() == 2013 || e.getErrorCode() == 2006)
        {
            auto conn = stmt_pt->getConnection();
            if (conn)
            {
                // 重连mysql
                if (conn->reconnect())
                {
                    TWARN("reconnect mysql success2");
                    return stmt_pt->execute();
                }
                else
                {
                    TERROR("reconnect mysql fail2");
                    return false;
                }
            }
        }
        else
        {
            return false;
        }
    }
    return false;
}

// 执行预处理语句更新
int32_t TRDataBase::ExecutePreUpdate(sql::PreparedStatement * stmt_pt)
{
    try
    {
        return stmt_pt->executeUpdate();  // 更新数据库
    }
    catch (sql::SQLException &e)
    {
        TERROR("mysql err_code:" << e.getErrorCode() << ", err:" << e.what() << ", sql_state:" << e.getSQLState());
        if (e.getErrorCode() == 2013 || e.getErrorCode() == 2006)
        {
            auto conn = stmt_pt->getConnection();
            if (conn)
            {
                // 重连mysql
                if (conn->reconnect())
                {
                    TWARN("reconnect mysql success2");
                    return stmt_pt->executeUpdate();
                }
                else
                {
                    TERROR("reconnect mysql fail2");
                    return 0;
                }
            }
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

// 执行预处理语句查询
sql::ResultSet *TRDataBase::ExecutePreQuery(sql::PreparedStatement * stmt_pt)
{
    try
    {
        return stmt_pt->executeQuery();  // 查询数据库
    }
    catch (sql::SQLException &e)
    {
        TERROR("mysql err_code:" << e.getErrorCode() << ", err:" << e.what() << ", sql_state:" << e.getSQLState());
        if (e.getErrorCode() == 2013 || e.getErrorCode() == 2006)
        {
            auto conn = stmt_pt->getConnection();
            if (conn)
            {
                // 重连mysql
                if (conn->reconnect())
                {
                    TWARN("reconnect mysql success2");
                    return stmt_pt->executeQuery();
                }
                else
                {
                    TERROR("reconnect mysql fail2");
                    return nullptr;
                }
            }
        }
        else
        {
            return nullptr;
        }
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TRDataBaseHelper::TRDataBaseHelper()
{

}

TRDataBaseHelper::~TRDataBaseHelper()
{

}

bool TRDataBaseHelper::AddDataBase(int32_t database_id, std::shared_ptr<TRDataBase> database_ptr)
{
    auto it = databases_.find(database_id);
    if (it != databases_.end())
        return false;
    
    databases_.insert({database_id, database_ptr});
    return true;
}

TRDataBase * TRDataBaseHelper::HoldDataBase(int32_t database_id)
{
    auto it = databases_.find(database_id);
    
    return it != databases_.end() ? it->second.get() : nullptr;
}

void TRDataBaseHelper::Stop()
{
    for (auto it = databases_.begin(); it != databases_.end(); ++it)
    {
        it->second->StopWorkers();
    }
}

void TRDataBaseHelper::FrameUpdate()
{
    for (auto it = databases_.begin(); it != databases_.end(); ++it)
    {
        it->second->EventRun();
    }
}

void TRDataBaseHelper::ForEach(std::function<void(int32_t db_id, TRDataBase *)> && visitor)
{
    for (auto it = databases_.begin(); it != databases_.end(); ++it)
    {
        visitor(it->first, it->second.get());
    }
}