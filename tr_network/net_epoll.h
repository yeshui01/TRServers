/********************************************************
# Author       : mknight
# Last modified: 2019-05-02 08:43
# Email        : 824338670@qq.com
# Filename     : net_epoll.h
# Description  : 
# *******************************************************/
#ifndef __NET_EPOLL_H__
#define __NET_EPOLL_H__

#include <sys/epoll.h>
#include "net_common.h"
#include <vector>
#include <map>
#include "net_socket.h"
class Epoll
{
public:
    Epoll();
    ~Epoll();

    // 获取epoll套接字
    int32_t GetFd();
    // 注册套接字事件
    ESocketOpCode RegSockEvent(TSocket *sock_pt, int32_t event_flag);
    // 取消套接字事件
    ESocketOpCode CancleSockEvent(TSocket *sock_pt);
    // 事件监听
    int32_t EventsWatch(int32_t timeout = 0);
    void Create(int32_t size);
    
protected:
    int32_t ep_fd_ = INVALID_SOCKET_FD;
    int32_t max_size_ = 1;
    // 事件集合
    std::vector<epoll_event> v_events_;
    // 监视的套接字记录
    std::map<socket_fd_t, TSocket *> register_sockets_;
};

#endif  // __NET_EPOLL_H__