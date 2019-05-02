/********************************************************
# Author       : mknight
# Last modified: 2019-05-01 11:33
# Email        : 824338670@qq.com
# Filename     : net_socket.h
# Description  : 
# *******************************************************/
#ifndef __NET_SOKCET_H__
#define __NET_SOKCET_H__
#include <sys/types.h>
#include <sys/socket.h>
#include "net_common.h"
#include "cycle_buffer.h"
class Epoll;
using socket_buffer_t = CycleBuffer<char>;
// 网络套接字
class TSocket
{
public:
    TSocket();
    virtual ~TSocket();

    // socket文件描述符
    socket_fd_t GetFd();
    void SetFd(socket_fd_t fd);

    // 获取套接字状态
    ESocketStatus GetStatus();
    // 设置套接字状态
    void SetStatus(ESocketStatus status);
    // 绑定地址和端口
    ESocketOpCode Bind(std::string ip, int32_t port);
    // 开始监听
    virtual ESocketOpCode Listen();
    // 连接服务器
    ESocketOpCode Connect(std::string ip, int32_t port);
    // 关闭
    void Close();
    // 获取读缓冲区
    socket_buffer_t & GetRecvBuffer();
    const socket_buffer_t & GetRecvBuffer() const;
    // 获取写缓冲区
    socket_buffer_t & GetWriteBuffer();
    const socket_buffer_t & GetWriteBuffer() const;
    // 读取数据
    int32_t Recv(char * buffer, int32_t buffer_size);
    // 读数据直到有数据到来
    int32_t WaitRecv(char * buffer, int32_t buffer_size);
    // 发送数据
    int32_t Send(const char * buffer, int32_t buffer_size);
    // 获取关联的eoll
    Epoll * GetEpoll();
    // 关联一个epoll
    void AttachEpoll(Epoll * epoll_pt);
    // 设置非组赛模式
    void SetNoblocking();
public:
    // 可读事件
    virtual void OnReadEvent();
    // 可写事件
    virtual void OnWriteEvent();
    // 出错事件
    virtual void OnErrorEvent();
    // 关闭
    virtual void OnClose();
protected:
    // 接受连接
    virtual void HandleAccept() = 0;
    // 收数据
    virtual void HandleRecv();
    // 写数据
    virtual void HandleWrite();
    // 出错处理
    virtual void HandleError();
    // 对端关闭了
    virtual void HandleOpClose();
    // 处理意外错误
    virtual void HandleRecvError();
    // 处理写数据意外错误
    virtual void HandleSendError();
protected:
    // 读到数据的处理
    virtual void AfterReadData(int32_t read_size) = 0;
protected:
    socket_fd_t fd_ = INVALID_SOCKET_FD;    // sock fd
    ESocketStatus status_ = ESocketStatus::E_SOCKET_STATUS_NONE;

    NetAddr bind_addr_;     // 监听套接字绑定的地址数据
    NetAddr connect_addr_;  // 连接套接字连接的地址数据
    NetAddr op_addr_;       // 对应的对端的地址
    socket_buffer_t recv_buffer_ = {INIT_SOCKET_BUFFER_SIZE};
    socket_buffer_t send_buffer_ = {INIT_SOCKET_BUFFER_SIZE};
    Epoll * epoll_ = nullptr;
};

#endif // __NET_SOKCET_H__
