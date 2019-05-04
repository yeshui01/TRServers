/********************************************************
# Author       : mknight
# Last modified: 2019-05-02 08:43
# Email        : 824338670@qq.com
# Filename     : net_epoll.cpp
# Description  : 
# *******************************************************/
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <memory.h>
#include "net_socket.h"
#include "log_module.h"
#include "net_epoll.h"

Epoll::Epoll()
{
}

Epoll::~Epoll()
{
}

void Epoll::Create(int32_t size)
{
    max_size_ = size;
    ep_fd_ = epoll_create(size);
    v_events_.resize(size);
    TDEBUG("epoll create, size:" << size << ", ep_fd:" << ep_fd_);
}

int32_t Epoll::GetFd()
{
    return ep_fd_;
}

 // 注册套接字事件
ESocketOpCode Epoll::RegSockEvent(TSocket *sock_pt, int32_t event_flag)
{
    if (INVALID_SOCKET_FD == ep_fd_)
    {
        TERROR("invalid ep_fd:" << ep_fd_ << ", cant reg sock event");
        return ESocketOpCode::E_SOCKET_OP_EPOLL_REG_FAILE;
    }
    if (register_sockets_.size() >= max_size_)
    {
        TERROR("ep socket events size is large max_size, cant register event again");
        return ESocketOpCode::E_SOCKET_OP_EPOLL_REG_FAILE;
    }
    if (!sock_pt)
    {
        return ESocketOpCode::E_SOCKET_OP_EPOLL_REG_FAILE;
    }
    if (INVALID_SOCKET_FD == sock_pt->GetFd())
    {
        TERROR("invalid socket fd, cant register event");
        return ESocketOpCode::E_SOCKET_OP_EPOLL_REG_FAILE;
    }
    // 防止重复注册
    if (register_sockets_.find(sock_pt->GetFd()) != register_sockets_.end())
    {
        TERROR("cant repeated register socket event");
        return ESocketOpCode::E_SOCKET_OP_EPOLL_REG_FAILE;
    }
    epoll_event evt;
    evt.events = event_flag;
    evt.data.ptr = sock_pt;
    if (-1 == epoll_ctl(ep_fd_, EPOLL_CTL_ADD, sock_pt->GetFd(), &evt))
    {
        return ESocketOpCode::E_SOCKET_OP_EPOLL_REG_FAILE;
    }
    register_sockets_[sock_pt->GetFd()] = sock_pt;
    sock_pt->AttachEpoll(this);
    TDEBUG("RegSockEvent, sock_fd:" << sock_pt->GetFd() << ", event_flag:" << event_flag);
    return ESocketOpCode::E_SOCKET_OP_CODE_CORRECT;
}
// 取消套接字事件
ESocketOpCode Epoll::CancleSockEvent(TSocket *sock_pt)
{
    if (INVALID_SOCKET_FD == ep_fd_)
    {
        TERROR("invalid ep_fd");
        return ESocketOpCode::E_SOCKET_OP_EPOLL_UNREG_FAILE;
    }
    if (INVALID_SOCKET_FD == sock_pt->GetFd())
    {
        TERROR("invalid socket fd, cant cancle event");
        return ESocketOpCode::E_SOCKET_OP_EPOLL_UNREG_FAILE;
    }
    if (-1 == epoll_ctl(ep_fd_, EPOLL_CTL_DEL, sock_pt->GetFd(), nullptr))
    {
        return ESocketOpCode::E_SOCKET_OP_EPOLL_UNREG_FAILE;
    }
    register_sockets_.erase(sock_pt->GetFd());
    TDEBUG("CancleSockEvent, fd:" << sock_pt->GetFd());
    sock_pt->AttachEpoll(nullptr);
    return ESocketOpCode::E_SOCKET_OP_CODE_CORRECT;
}

int32_t Epoll::EventsWatch(int32_t timeout/* = 0*/)
{
    if (INVALID_SOCKET_FD == ep_fd_)
    {
        TERROR("ep sockeet is invalid");
        return -1;
    }
    auto code = epoll_wait(ep_fd_, &v_events_[0], max_size_, timeout);
    if (code == -1)
    {
        TERROR("epoll wait error");
        return code;
    }
    for (int32_t i = 0; i < code; ++i)
    {
        TSocket * socket_pt = reinterpret_cast<TSocket *>(v_events_[i].data.ptr);
        if (!socket_pt)
            continue;
        
        if (v_events_[i].events & EPOLLIN)
        {
            // 可以读取
            socket_pt->OnReadEvent();
        }
        if (v_events_[i].events & EPOLLOUT)
        {
            // 可以写数据了
             socket_pt->OnWriteEvent();
        }
        if (v_events_[i].events & EPOLLERR)
        {
            // 出错了
            // 取消监听
            TERROR("error events, fd:");
            CancleSockEvent(socket_pt);
            socket_pt->OnErrorEvent();
        }
    }
    return code;
}

int32_t Epoll::ResFdSpace()
{
    return max_size_ - register_sockets_.size();
}