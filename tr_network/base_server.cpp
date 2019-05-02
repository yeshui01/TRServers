#include "base_server.h"
#include "singleton.h"
#include "net_epoll.h"
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <memory.h>
#include <error.h>
#include "net_socket.h"
#include "log_module.h"
#include "net_epoll.h"
#include "net_common.h"
#include "connect_manager.h"
#include <thread>
#include <chrono>
TBaseServer::TBaseServer()
{

}
TBaseServer::~TBaseServer()
{
}

TConnection * TBaseServer::AllocateConnect()
{
    return nullptr;
}
// 接受连接
void TBaseServer::HandleAccept()
{
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    socket_fd_t client_sock = accept(fd_, (struct sockaddr*)&client_addr, &addr_len);
    if (INVALID_SOCKET_FD != client_sock)
    {
        // 有效套接字
        TConnection * connect_pt = AllocateConnect();
        if (connect_pt)
        {
            connect_pt->SetStatus(ESocketStatus::E_SOCKET_STATUS_BE_CONNECT);
            connect_pt->SetFd(client_sock);
            // TODO:记录对端地址
            std::string ip = inet_ntoa(client_addr.sin_addr);
            TDEBUG("accept new connect, fd:" << client_sock << ", ip:" << ip << ", port:" << client_addr.sin_port);
            g_ConnectMgr.AddConnction(connect_pt);
            // 添加到epoll
            auto epoll_pt = GetEpoll();
            if (epoll_pt)
            {
                connect_pt->SetNoblocking();
                epoll_pt->RegSockEvent(connect_pt, EPOLLIN | EPOLLOUT | EPOLLERR);
            }
            else
            {
                TERROR("epoll_pt is null");
            }
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
        v(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
    }
}

void TBaseServer::RunService()
{
    while (!stop_)
    {
        LoopRun();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

void TBaseServer::Stop()
{
    stop_ = true;
}

void TBaseServer::AddLoopRun(loop_func_t && func)
{
    v_loop_func_.push_back(func);
}