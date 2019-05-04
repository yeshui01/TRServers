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
    // 创建epoll
    void Create(int32_t size);
    // 获取剩余能够监听的套接字最大数量
    int32_t ResFdSpace();
protected:
    int32_t ep_fd_ = INVALID_SOCKET_FD;     // epoll套接字
    int32_t max_size_ = 1;                  // 最大监听数量
    std::vector<epoll_event> v_events_;     // 事件集合
    std::map<socket_fd_t, TSocket *> register_sockets_; // 监视的套接字记录
};

#endif  // __NET_EPOLL_H__