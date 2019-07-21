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

using loop_func_t = std::function<void(time_t cur_mtime)>;

class TBaseServer : public TSocket
{
public:
    TBaseServer();
    ~TBaseServer();
    using ParentClass = TSocket;

public:
    // 初始化
    virtual bool Init() = 0;
    // 运行服务
    void RunService();
    // 停服
    void Stop();
    // 每帧循环
    virtual void LoopRun();
    // 分配一个连接
    virtual TConnection * AllocateConnect();
    // 新的连接建立了
    virtual void OnNewConnectComeIn(TConnection * new_connection);
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
    // 读取数据后的处理
    void AfterReadData(int32_t read_size);
    // 设置连接队列大小
    void SetConnectEventLimitNum(int32_t max_sock_num);
    // 设置运行阶段
    void SetRunStep(EServerRunStep e_step);
    // 获取运行阶段
    EServerRunStep GetRunStep() const;
    EServerRunStep GetRunStep();
    
public:
    // 接受连接处理
    virtual void HandleAccept();
    // 运行前的检查
    virtual bool RunStepCheck();
    // 即将运行
    virtual bool RunStepWillRun();
    // 正常运行
    virtual bool RunStepRunning(); 
    // 等待启动
    virtual bool RunStepWaiting();
    // 等待其他服务器
    virtual bool RunStepWaitOtherServers();
    // 运行前一刻
    virtual bool RunStepPreRun();
protected:
    // 回收连接
    void RecycleConnections();
protected:
    bool stop_ = false;
    std::vector<loop_func_t> v_loop_func_;
    std::unique_ptr<Epoll> epoll_ptr_;
    TObjPool<TConnection> connection_pool_;
    std::vector<TConnection*> wait_recycle_connect_;  // 等待回收的连接
    int32_t max_connect_num_ = 0;
    EServerRunStep run_step_ = EServerRunStep::E_SERVER_RUN_STEP_NONE;
};

#endif  // __BASE_SERVER_H__

