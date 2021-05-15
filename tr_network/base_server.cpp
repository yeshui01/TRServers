/********************************************************
# Author       : mknight
# Last modified: 2019-05-02 13:08
# Email        : 824338670@qq.com
# Filename     : base_server.cpp
# Description  : 
# *******************************************************/
#include "base_server.h"
#include "singleton.h"
#include "net_epoll.h"
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/un.h>
#include <fcntl.h>
#include <memory.h>
#include <error.h>
#include "net_socket.h"
#include "log_module.h"
#include "net_epoll.h"
#include "net_common.h"
#include "connect_manager.h"
#include "net_epoll.h"
#include "server_time.h"
#include <thread>
#include <chrono>
#include <algorithm>
TBaseServer::TBaseServer()
{
    epoll_ptr_.reset(new Epoll());
}
TBaseServer::~TBaseServer()
{
}

TConnection * TBaseServer::AllocateConnect()
{
    return connection_pool_.Pop();
}
// 接受连接
void TBaseServer::HandleAccept()
{
    if (epoll_->RestFdSpace() <= 0)
    {
        TERROR("rest epoll fd scpace is 0");
        return;
    }
    if (INVALID_SOCKET_FD != fd_)
    {
        // 有效套接字
        TConnection * connect_pt = AllocateConnect();
        if (connect_pt)
        {
            if (GetTransMode() == E_SOCKET_TRANS_MODE_TCP)
            {
                sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);
                socket_fd_t client_sock = accept(fd_, (struct sockaddr*)&client_addr, &addr_len);
                if (INVALID_SOCKET_FD == client_sock)
                {
                    TERROR("client socket is invalid");
                    return;
                }
                connect_pt->SetStatus(ESocketStatus::E_SOCKET_STATUS_BE_CONNECT);
                connect_pt->SetFd(client_sock);
                // 记录对方的地址数据
                std::string ip = inet_ntoa(client_addr.sin_addr);
                TDEBUG("accept new connect, fd:" << client_sock << ", ip:" << ip << ", port:" << client_addr.sin_port);
            }
            else
            {
                struct sockaddr_un client_addr;
                socklen_t addr_len = sizeof(client_addr);
                socket_fd_t client_sock = accept(fd_, (struct sockaddr*)&client_addr, &addr_len);
                if (INVALID_SOCKET_FD == client_sock)
                {
                    TERROR("client socket is invalid");
                    return;
                }
                connect_pt->SetStatus(ESocketStatus::E_SOCKET_STATUS_BE_CONNECT);
                connect_pt->SetFd(client_sock);
                // 记录对方的地址数据
                TDEBUG("accept new connect on unix_sock_file, fd:" << client_sock);
            }
            
            OnNewConnectComeIn(connect_pt);
        }
        else
        {
            TERROR("alocate connection failed");
        }
        
    }
}

bool TBaseServer::Init()
{
    return true;
}

void TBaseServer::LoopRun()
{
    // Nothing
    for (auto &v : v_loop_func_)
    {
        // v(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
        v(g_ServerTime.NowTimeMs());
    }
}

void TBaseServer::RunService()
{
    if (!Init())
    {
        TERROR("server init failed");
        return;
    }
    TINFO("run service");
    // 注册epoll监听
    SetRunStep(EServerRunStep::E_SERVER_RUN_STEP_CHECK);
    while (!stop_)
    {
        g_ServerTime.UpdateTime();
        if (run_step_ >= EServerRunStep::E_SERVER_RUN_STEP_LISTEN)
        {
            epoll_ptr_->EventsWatch(epoll_mtime_);
            LoopRun();
        }
        switch (run_step_)
        {
            case EServerRunStep::E_SERVER_RUN_STEP_CHECK:
            {
                if (RunStepCheck())
                {
                    SetRunStep(EServerRunStep::E_SERVER_RUN_STEP_LISTEN);
                }
                break;
            }
            case EServerRunStep::E_SERVER_RUN_STEP_LISTEN:
            {
                if (this->GetFd() != INVALID_SOCKET_FD)
                {
                    epoll_ptr_->RegSockEvent(this, EPOLLIN);
                }
                else
                {
                    TWARN("server no bind port!!!");
                }
                
                SetRunStep(EServerRunStep::E_SERVER_RUN_STEP_WILL_RUNNING);
                break;
            }
            case EServerRunStep::E_SERVER_RUN_STEP_WILL_RUNNING:
            {
                if (RunStepWillRun())
                {
                    SetRunStep(EServerRunStep::E_SERVER_RUN_STEP_WAIT_BOOTUP);
                }
                else
                {
                    Stop();
                }
                break;
            }
            case EServerRunStep::E_SERVER_RUN_STEP_WAIT_BOOTUP:
            {
                if (RunStepWaiting())
                {
                    SetRunStep(EServerRunStep::E_SERVER_RUN_STEP_WAIT_OTHER_SERVERS);
                }
                break;
            }
            case EServerRunStep::E_SERVER_RUN_STEP_WAIT_OTHER_SERVERS:
            {
                if (RunStepWaitOtherServers())
                {
                    SetRunStep(EServerRunStep::E_SERVER_RUN_STEP_PRE_RUN);
                }
                break;
            }
            case EServerRunStep::E_SERVER_RUN_STEP_PRE_RUN:
            {
                if (RunStepPreRun())
                {
                    SetRunStep(EServerRunStep::E_SERVER_RUN_STEP_RUNNING);
                }
                else
                {
                    Stop();
                }
                break;
            }
            case EServerRunStep::E_SERVER_RUN_STEP_RUNNING:
            {
                RunStepRunning();
                break;
            }
            case EServerRunStep::E_SERVER_RUN_STEP_STOP:
            {
                RunStepStop();
                stop_ = true;
                break;
            }
        default:
            break;
        }
        
        // 回收连接
        RecycleConnections();
        if (EServerRunStep::E_SERVER_RUN_STEP_RUNNING != run_step_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void TBaseServer::Stop()
{
    // 停服操作
    // 关闭所有连接
    g_ConnectMgr.Traversal([](TConnection * connect_pt){
        if (connect_pt)
        {
            connect_pt->Close();
        }
    });
    Close();
    SetRunStep(EServerRunStep::E_SERVER_RUN_STEP_STOP);
    // stop_ = true;
}

void TBaseServer::AddLoopRun(loop_func_t && func)
{
    v_loop_func_.push_back(func);
}

void TBaseServer::AfterReadData(int32_t read_size)
{
    // do nothing
}

void TBaseServer::RecycleConnect(TConnection * connect_pt)
{
    if (!connect_pt)
        return;
    
    if (std::find(wait_recycle_connect_.begin(), wait_recycle_connect_.end(), connect_pt)
        != wait_recycle_connect_.end())
    {
        TERROR("cant repeated recycle connect");
        return;
    }
    wait_recycle_connect_.push_back(connect_pt);
}

void TBaseServer::SetConnectEventLimitNum(int32_t max_sock_num)
{
    max_connect_num_ = max_sock_num;
    if (epoll_ptr_)
    {
        epoll_ptr_->Create(max_connect_num_ + 1);
    }
}

void TBaseServer::RecycleConnections()
{
    if (!wait_recycle_connect_.empty())
    {
        TDEBUG("recycle wait_recycle_connect_,size:" << wait_recycle_connect_.size());
        for (auto v : wait_recycle_connect_)
        {
            TDEBUG("recycle connect, ptr:" << (int64_t)(v) << ", conn_id:" << v->GetConnId());
            // TDEBUG("before idle_size:" << connection_pool_.GetIdleSize()
            //     << ", working_size:" << connection_pool_.GetWorkingSize());
            g_ConnectMgr.DelConnection(v);
            connection_pool_.Push(v);
            TDEBUG("after idle_size:" << connection_pool_.GetIdleSize()
                                      << ", working_size:" << connection_pool_.GetWorkingSize());
            v->AttachServer(nullptr);
            v->GetRecvBuffer().Reset();
            v->GetWriteBuffer().Reset();
        }
        wait_recycle_connect_.clear();
    }
}

bool TBaseServer::RunStepCheck()
{
    return true;
}

void TBaseServer::SetRunStep(EServerRunStep e_step)
{
    run_step_ = e_step;
    TINFO("SetRunStep, e_step:" << int32_t(e_step));
}

bool TBaseServer::RunStepRunning()
{
    return true;
}

EServerRunStep TBaseServer::GetRunStep() const
{
    return run_step_;
}
EServerRunStep TBaseServer::GetRunStep()
{
    return run_step_;
}

void TBaseServer::OnNewConnectComeIn(TConnection * new_connection)
{
    g_ConnectMgr.AddConnction(new_connection);
    new_connection->AttachServer(this);
    // 添加到epoll
    auto epoll_pt = GetEpoll();
    if (!epoll_pt)
    {
        epoll_pt = epoll_ptr_.get();
    }
    if (epoll_pt)
    {
        new_connection->SetNoblocking();
        // new_connection->SetSockWriteBufferSize(16*1024);
        // epoll_pt->RegSockEvent(new_connection, EPOLLIN | EPOLLOUT | EPOLLERR);
        epoll_pt->RegSockEvent(new_connection, EPOLLIN | EPOLLERR);
    }
    else
    {
        TERROR("epoll_pt is null");
    }
}

bool TBaseServer::RunStepWillRun()
{
    return true;
}

 // 等待启动
bool TBaseServer::RunStepWaiting()
{
    return true;
}
// 运行前一刻
bool TBaseServer::RunStepPreRun()
{
    return true;
}

bool TBaseServer::RunStepWaitOtherServers()
{
    return true;
}

bool TBaseServer::RunStepStop()
{
    return true;
}

void TBaseServer::SetEpollTimeout(int32_t msec)
{
    if (msec < 0)
    {
        msec = 0;
    }
    epoll_mtime_ = msec;
}