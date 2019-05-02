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
#include <functional>

using loop_func_t = std::function<void(time_t cur_time)>;
class TBaseServer : public TConnection
{
public:
    TBaseServer();
    ~TBaseServer();
    using ParentClass = TConnection;

public:
    bool Init();
    void RunService();
    void Stop();
    virtual void LoopRun();
    virtual TConnection * AllocateConnect() = 0;
    // 添加循环执行的函数
    void AddLoopRun(loop_func_t && func);
public:
    // 接受连接
    virtual void HandleAccept();
protected:
    bool stop_ = false;
    std::vector<loop_func_t> v_loop_func_;
};

#endif  // __BASE_SERVER_H__

