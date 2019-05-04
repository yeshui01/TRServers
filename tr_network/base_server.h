/********************************************************
# Author       : mknight
# Last modified: 2019-05-02 13:08
# Email        : 824338670@qq.com
# Filename     : base_server.h
# Description  : 
# *******************************************************/

#ifndef __BASE_SERVER_H__
#define __BASE_SERVER_H__
#include "net_connection.h"
#include "obj_pool.h"
#include <functional>
#include <memory>
class Epoll;

using loop_func_t = std::function<void(time_t cur_time)>;
class TBaseServer : public TSocket
{
public:
    TBaseServer();
    ~TBaseServer();
    using ParentClass = TSocket;

public:
    virtual bool Init() = 0;
    void RunService();
    void Stop();
    virtual void LoopRun();
    virtual TConnection * AllocateConnect();
    // 添加循环执行的函数
    void AddLoopRun(loop_func_t && func);
    // 添加连接到回收列表
    void RecycleConnect(TConnection * connect_pt);
    // 初始化连接池
    template <class T>
    void InitConnectionPool(int32_t num)
    {
        connection_pool_.InitEx<T>(num);
        SetConnectEventLimitNum(num);
    }
    void AfterReadData(int32_t read_size);
    // 设置连接队列大小
    void SetConnectEventLimitNum(int32_t max_sock_num);
public:
    // 接受连接
    virtual void HandleAccept();
protected:
    bool stop_ = false;
    std::vector<loop_func_t> v_loop_func_;
    std::unique_ptr<Epoll> epoll_ptr_;
    TObjPool<TConnection> connection_pool_;
    std::vector<TConnection*> wait_recycle_connect_;  // 等待回收的连接
    int32_t max_connect_num_ = 0;

};

#endif  // __BASE_SERVER_H__

