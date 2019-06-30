/********************************************************
# Author       : mknight
# Last modified: 2019-05-01 11:33
# Email        : 824338670@qq.com
# Filename     : net_socket.cpp
# Description  : 
# *******************************************************/
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
TSocket::TSocket()
{
}
TSocket::~TSocket()
{
    if (INVALID_SOCKET_FD != fd_)
    {
        Close();
    }
}

socket_fd_t TSocket::GetFd()
{
    return fd_;
}

void TSocket::SetFd(socket_fd_t fd)
{
    fd_ = fd;
}

ESocketStatus TSocket::GetStatus()
{
    return status_;
}

void TSocket::SetStatus(ESocketStatus status)
{
    status_ = status;
}

ESocketOpCode TSocket::Bind(std::string ip, int32_t port)
{
    sockaddr_in server_addr;
    fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET_FD == fd_)
    {
        return ESocketOpCode::E_SOCKET_OP_SOCK_CREATE_FAILE;
    }
    memset(&server_addr, 0, sizeof(sockaddr_in));
    server_addr.sin_family = AF_INET;
    auto code = inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);
    if (0 == code)
    {
        return ESocketOpCode::E_SOCKET_OP_IP_ADDR_ERROR;
    }
    server_addr.sin_port = htons(port);
    code = bind(fd_, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr));
    if (-1 == code)
    {
        return ESocketOpCode::E_SOCKET_OP_BIND_ERROR;
    }
    // bind success
    bind_addr_.ip = ip;
    bind_addr_.port = port;
    return ESocketOpCode::E_SOCKET_OP_CODE_CORRECT;
}

ESocketOpCode TSocket::Listen()
{
    auto code = listen(fd_, 100);
    if (-1 == code)
    {
        return ESocketOpCode::E_SOCKET_OP_LISTEN_ERROR;
    }
    status_ = ESocketStatus::E_SOCKET_STATUS_LISTEN;
    return ESocketOpCode::E_SOCKET_OP_CODE_CORRECT;
}

ESocketOpCode TSocket::Connect(std::string ip, int32_t port)
{
    sockaddr_in server_addr;
    fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET_FD == fd_)
    {
        return ESocketOpCode::E_SOCKET_OP_SOCK_CREATE_FAILE;
    }
    memset(&server_addr, 0, sizeof(sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(port);
    
    int32_t code = connect(fd_, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr));
    if (-1 == code)
    {
        TERROR("connect server failed, op:" << ip << ", port:" << port);
        return ESocketOpCode::E_SOCKET_OP_CODE_CONNECT_FAILE;
    }
    status_ = ESocketStatus::E_SOCKET_STATUS_CONNECTED;
    // 记录连接的地址
    connect_addr_.ip = ip;
    connect_addr_.port = port;
    return ESocketOpCode::E_SOCKET_OP_CODE_CORRECT;
}

void TSocket::Close()
{
    if (INVALID_SOCKET_FD != fd_)
    {
        ::close(fd_);
        OnClose();
        status_ = ESocketStatus::E_SOCKET_STATUS_CLOSED;
        fd_ = INVALID_SOCKET_FD;
    }
}

// 获取读缓冲区
socket_buffer_t & TSocket::GetRecvBuffer()
{
    return recv_buffer_;
}
const socket_buffer_t & TSocket::GetRecvBuffer() const
{
    return recv_buffer_;
}
// 获取写缓冲区
socket_buffer_t & TSocket::GetWriteBuffer()
{
    return send_buffer_;
}
const socket_buffer_t & TSocket::GetWriteBuffer() const
{
    return send_buffer_;
}

 // 可读事件
void TSocket::OnReadEvent()
{
    if (ESocketStatus::E_SOCKET_STATUS_LISTEN == GetStatus())
    {
        TDEBUG("OnReadEvent: listen socket:fd" << fd_);
        HandleAccept();
    }
    else 
    {
        TDEBUG("OnReadEvent:fd" << fd_);
        HandleRecv();
    }
}
// 可写事件
void TSocket::OnWriteEvent()
{
    // TDEBUG("OnWriteEvent:fd" << fd_);
    HandleWrite();
}
// 出错事件
void TSocket::OnErrorEvent()
{
    HandleError();
}

// 可以接受连接
void TSocket::HandleAccept()
{
    // TODO:
}
    // 可以收数据
void TSocket::HandleRecv()
{
    // 接收数据到缓冲区
    char buffer[1024];
    int32_t read_ret = 0;
    do 
    {
        read_ret = recv(fd_, buffer, 1024, 0);
        if (0 == read_ret)
        {
            // 对端socket关闭了
            HandleOpClose();
            break;
        }
        else if (read_ret > 0)
        {
            // 数据收入缓冲区
            int32_t s = recv_buffer_.WriteData(buffer, read_ret);
            if (s < read_ret)
            {
                // 缓冲区不够了
                TERROR("recv buffer full");
            }
            AfterReadData(s);
        }
        else 
        {
            if (EAGAIN == errno || EWOULDBLOCK == errno || EINTR == errno)
            {
                // 数据读完了
                break;
            }
            else 
            {
                // 发生了意外错误，需要关闭
                HandleRecvError();
            }
        }
    } while (read_ret > 0);
}
void TSocket::HandleWrite()
{
    // 可以写数据了，如果有需要发送的数据，发送数据
    if (send_buffer_.PeekDataSize() > 0)
    {
        TDEBUG("ready to send data, peek data size:" << send_buffer_.PeekDataSize());
        char buffer[1024];
        int32_t send_size = 0;
        do
        {
            int32_t s = send_buffer_.ReadData(buffer, 1024, true);
            send_size = send(fd_, buffer, s, 0);
            if (send_size > 0)
            {
                // 发送出去了send_size字节,缓冲区清理
                send_buffer_.ReadData(buffer, send_size);
            }
            else if (send_size == 0)
            {
                // 对端关闭
                HandleOpClose();
                break;
            }
            else if (send_size < 0)
            {
                if (EAGAIN != errno)
                {
                    // 意外错误
                    HandleSendError();
                }
                break;
            }
        } while (send_buffer_.GetRestSpace() > 0 && send_size > 0);
    }
}

void TSocket::HandleError()
{
    if (epoll_)
    {
        TERROR("TSocket::HandleError");
        epoll_->CancleSockEvent(this);
        epoll_ = nullptr;
        Close();
    }
}

// 读取数据
int32_t TSocket::Recv(char * buffer, int32_t buffer_size)
{
    // 从接收缓冲区都数据
    return recv_buffer_.ReadData(buffer, buffer_size);
}
// 发送数据
int32_t TSocket::Send(const char * buffer, int32_t buffer_size)
{
    int32_t send_size = send(fd_, buffer, buffer_size, 0);
    TDEBUG("TSocket::send, ret:" << send_size);
    if (send_size > 0)
    {
        if (send_size < buffer_size)
        {
            // 放到缓冲区等下次发送
            TDEBUG("send data be write to send buffer");
            send_buffer_.WriteDataToHead(buffer + send_size, buffer_size - send_size);
        }
    }
    else 
    {
        if (EAGAIN == errno)
        {
            send_size = send_buffer_.WriteData(buffer, buffer_size);
            if (send_size < buffer_size)
            {
                TERROR("send buffer full");
            }
        }
        else 
        {
            TERROR("send error:" << errno);
            HandleSendError();
        }
    }
    return send_size;
}

void TSocket::HandleOpClose()
{
    if (epoll_)
    {
        TERROR("TSocket::HandleOpClose");
        epoll_->CancleSockEvent(this);
        epoll_ = nullptr;
        Close();
    }
}

void TSocket::HandleRecvError()
{
    if (epoll_)
    {
        TERROR("TSocket::HandleRecvError");
        epoll_->CancleSockEvent(this);
        epoll_ = nullptr;
        Close();
    }
}

void TSocket::AfterReadData(int32_t read_size)
{
}
void TSocket::HandleSendError()
{
    if (epoll_)
    {
        TERROR("HandleSendError");
        epoll_->CancleSockEvent(this);
        epoll_ = nullptr;
    }
}

 Epoll * TSocket::GetEpoll()
 {
     return epoll_;
 }

 void TSocket::AttachEpoll(Epoll * epoll_pt)
 {
     epoll_ = epoll_pt;
 }

 void TSocket::SetNoblocking()
 {
     int32_t flag = fcntl(fd_, F_GETFL, 0);
     fcntl(fd_, F_SETFL, flag | O_NONBLOCK);
 }

 void TSocket::OnClose()
 {
 }

 int32_t TSocket::WaitRecv(char * buffer, int32_t buffer_size)
 {
     return recv(fd_, buffer, buffer_size, 0);
 }