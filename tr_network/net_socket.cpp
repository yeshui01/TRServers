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
#include <sys/un.h>
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
        return ESocketOpCode::E_SOCKET_OP_SOCK_CREATE_FAIL;
    }
    bool reuse_addr = true;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse_addr, sizeof(bool));
    bool reuse_port = true;
    setsockopt(fd_,SOL_SOCKET, SO_REUSEPORT,(const char*)&reuse_port,sizeof(bool));

    struct linger so_linger;
    so_linger.l_onoff = true;
    so_linger.l_linger = 0;
    setsockopt(fd_, SOL_SOCKET, SO_LINGER, &so_linger,sizeof(so_linger));

    SetNoblocking();
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
    e_trans_mode_ = E_SOCKET_TRANS_MODE_TCP;
    return ESocketOpCode::E_SOCKET_OP_CODE_CORRECT;
}

ESocketOpCode TSocket::Listen()
{
    auto code = listen(fd_, SOMAXCONN);
    if (-1 == code)
    {
        return ESocketOpCode::E_SOCKET_OP_LISTEN_ERROR;
    }
    status_ = ESocketStatus::E_SOCKET_STATUS_LISTEN;
    // bool reuse_addr = true;
    // setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse_addr, sizeof(bool));
    return ESocketOpCode::E_SOCKET_OP_CODE_CORRECT;
}

ESocketOpCode TSocket::Connect(std::string ip, int32_t port)
{
    sockaddr_in server_addr;
    fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET_FD == fd_)
    {
        return ESocketOpCode::E_SOCKET_OP_SOCK_CREATE_FAIL;
    }
    memset(&server_addr, 0, sizeof(sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(port);
    
    int32_t code = connect(fd_, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr));
    if (-1 == code)
    {
        ::close(fd_);
        fd_ = INVALID_SOCKET_FD;

        TERROR("connect server failed, op:" << ip << ", port:" << port);
        return ESocketOpCode::E_SOCKET_OP_CODE_CONNECT_FAIL;
    }
    status_ = ESocketStatus::E_SOCKET_STATUS_CONNECTED;
    // 记录连接的地址
    connect_addr_.ip = ip;
    connect_addr_.port = port;
    e_trans_mode_ = E_SOCKET_TRANS_MODE_TCP;
    return ESocketOpCode::E_SOCKET_OP_CODE_CORRECT;
}

void TSocket::Close()
{
    if (INVALID_SOCKET_FD != fd_)
    {
        if (epoll_)
        {
            TWARN("TSocket::Close, epoll unreg");
            epoll_->CancelSockEvent(this);
            epoll_ = nullptr;
        }
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
    char buffer[16384]; // 16K
    int32_t read_ret = 0;
    do 
    {
        read_ret = recv(fd_, buffer, 16384, 0);
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
        static char buffer[16384];      // 16K
        //static int32_t buffer_default_size = 4096;
        int32_t send_size = 0;
        do
        {
            int32_t s = send_buffer_.ReadData(buffer, 16384);
            send_size = InnerSend(buffer, s, true);
            if (send_size < 1)
                break;

        } while (send_buffer_.PeekDataSize() > 0 && send_size > 0);

        if (send_buffer_.PeekDataSize() < 1)
        {
            // 取消写事件的监听
            TWARN("cancel write event,fd:" << fd_);
            if (epoll_ && is_write_event_)
            {
                epoll_->ChangeSockEvent(this, EPOLLIN | EPOLLERR);
                is_write_event_ = false;
            }
        }
    }
    else if (epoll_ && is_write_event_)
    {
        TWARN("cancel write event2,fd:" << fd_);
        if (epoll_ && is_write_event_)
        {
            epoll_->ChangeSockEvent(this, EPOLLIN | EPOLLERR);
            is_write_event_ = false;
        }
    }
    
}

void TSocket::HandleError()
{
    if (epoll_)
    {
        TWARN("TSocket::HandleError");
        epoll_->CancelSockEvent(this);
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
    return InnerSend(buffer, buffer_size, false);
}

 int32_t TSocket::InnerSend(const char * buffer, int32_t buffer_size, bool fail_to_head)
 {
    if (!buffer)
    {
        TERROR("buffer is nullptr");
        return 0;
    }
    if (buffer_size < 1)
    {
        return 0;
    }
    if (fd_ == INVALID_SOCKET_FD)
    {
        return 0;
    }
    if (is_write_event_ && !fail_to_head /*&& send_buffer_.GetRestSpace() > 0*/)
    {
        // 说明有待发送的数据未发送完，那么直接放到发送缓冲区
        return send_buffer_.WriteData(buffer, buffer_size);
    }
    int32_t send_size = send(fd_, buffer, buffer_size, 0);
    int32_t cur_error_no = errno;
    TDEBUG("TSocket::send, ret:" << send_size << ", cur_error_no:" << cur_error_no << ",fd:" << fd_ << ",buffer_size:" << buffer_size);
    if (send_size > 0)
    {
        if (send_size < buffer_size)
        {
            // 放到缓冲区等下次发送
            TWARN("send data be write to send buffer,fd:" << fd_);
            send_buffer_.WriteDataToHead(buffer + send_size, buffer_size - send_size);
            if (epoll_ && !is_write_event_)
            {
                // RegSockEvent(this, EPOLLOUT);
                epoll_->ChangeSockEvent(this, EPOLLIN | EPOLLOUT | EPOLLERR);
                is_write_event_ = true;
            }
        }
    }
    else 
    {
        if (fail_to_head)
        {
            send_size = send_buffer_.WriteDataToHead(buffer, buffer_size);
        }
        else
        {
            send_size = send_buffer_.WriteData(buffer, buffer_size);
        }
        if (epoll_ && !is_write_event_)
        {
            // RegSockEvent(this, EPOLLOUT);
            epoll_->ChangeSockEvent(this, EPOLLIN | EPOLLOUT | EPOLLERR);
            is_write_event_ = true;
        }
        if (send_size < buffer_size)
        {
            TERROR("send buffer full");
        }
        if (EAGAIN == cur_error_no)
        {
            return 0;
        }
        else if (cur_error_no > 0)
        {
            TERROR("send error:" << cur_error_no);
            HandleSendError();
        }
    }
    return send_size;
 }

void TSocket::HandleOpClose()
{
    if (epoll_)
    {
        TWARN("TSocket::HandleOpClose,error_no:" << errno);
        epoll_->CancelSockEvent(this);
        epoll_ = nullptr;
        Close();
    }
}

void TSocket::HandleRecvError()
{
    if (epoll_)
    {
        TERROR("TSocket::HandleRecvError");
        epoll_->CancelSockEvent(this);
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
        epoll_->CancelSockEvent(this);
        epoll_ = nullptr;
        Close();
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
    int32_t ret = fcntl(fd_, F_SETFL, flag | O_NONBLOCK);
    TDEBUG("SetNoblocking:" << fd_ << ", ret:" << ret);
 }

 void TSocket::OnClose()
 {
 }

 int32_t TSocket::WaitRecv(char * buffer, int32_t buffer_size)
 {
     return recv(fd_, buffer, buffer_size, 0);
 }

 void TSocket::SetSockRecvBufferSize(int32_t buffer_size)
 {
    if (buffer_size < 1)
    {
        return;
    }
    if (fd_ == INVALID_SOCKET_FD)
    {
        return;
    }
    //设置socket发送、接收缓冲区大小为64k，默认为8k，最大为64k
    int32_t set_size = buffer_size;
    int32_t set_code = 0;
    int32_t result = 0;
    socklen_t opt_len = 4;
    set_code = ::setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, (char*)&set_size, sizeof(set_size));

    set_code = ::getsockopt(fd_,SOL_SOCKET, SO_RCVBUF, (char*)&result, &opt_len);
    TDEBUG("set socket recv buffer:" << result);
 }

void TSocket::SetSockWriteBufferSize(int32_t buffer_size)
{
    //设置socket发送、接收缓冲区大小为64k，默认为8k，最大为64k
    int32_t set_size = buffer_size;
    int32_t set_code = 0;
    int32_t result = 0;
    socklen_t opt_len = 4;
    set_code = ::getsockopt(fd_,SOL_SOCKET, SO_SNDBUF, (char*)&result, &opt_len);
    TDEBUG("get socket send buffer:" << result << "," << set_size);
    set_code = ::setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, (char*)&set_size, sizeof(set_size));

    set_code = ::getsockopt(fd_,SOL_SOCKET, SO_SNDBUF, (char*)&result, &opt_len);
    TDEBUG("get socket send buffer:" << result << "," << set_size);
}

ESocketOpCode TSocket::BindLocalFile(std::string sock_file)
{
    unlink(sock_file.c_str());
    fd_ = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (INVALID_SOCKET_FD == fd_)
    {
        return ESocketOpCode::E_SOCKET_OP_SOCK_CREATE_FAIL;
    }
    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    memcpy(server_addr.sun_path, sock_file.c_str(), sock_file.size() + 1);
    bool reuse_addr = true;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse_addr, sizeof(bool));
    // int32_t size = offsetof(struct sockaddr_un, sun_path) + sock_file.length();
    // int32_t server_len = sizeof(server_address)
    struct linger so_linger;
    so_linger.l_onoff = true;
    so_linger.l_linger = 0;
    setsockopt(fd_, SOL_SOCKET, SO_LINGER, &so_linger,sizeof(so_linger));
    SetNoblocking();
    auto code = bind(fd_, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr));
    if (-1 == code)
    {
        return ESocketOpCode::E_SOCKET_OP_BIND_ERROR;
    }
    // bind success
    bind_addr_.unix_sock_file = sock_file;
    e_trans_mode_ = E_SOCKET_TRANS_MODE_UNIX_SOCK_FILE;
    return ESocketOpCode::E_SOCKET_OP_CODE_CORRECT;
}
ESocketOpCode TSocket::ConnectLocalFile(std::string sock_file)
{
    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    fd_ = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (INVALID_SOCKET_FD == fd_)
    {
        return ESocketOpCode::E_SOCKET_OP_SOCK_CREATE_FAIL;
    }
    memset(&server_addr, 0, sizeof(sockaddr_in));
    server_addr.sun_family = AF_UNIX;
    memcpy(server_addr.sun_path, sock_file.c_str(), sock_file.size() + 1);
    int32_t code = connect(fd_, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr));
    if (-1 == code)
    {
        ::close(fd_);
        fd_ = INVALID_SOCKET_FD;
        TERROR("connect server faile:" << sock_file);
        return ESocketOpCode::E_SOCKET_OP_CODE_CONNECT_FAIL;
    }
    status_ = ESocketStatus::E_SOCKET_STATUS_CONNECTED;
    // 记录连接的地址
    connect_addr_.unix_sock_file = sock_file;
    e_trans_mode_ = E_SOCKET_TRANS_MODE_UNIX_SOCK_FILE;
    return ESocketOpCode::E_SOCKET_OP_CODE_CORRECT;
}
ESocketTransMode TSocket::GetTransMode()
{
    return e_trans_mode_;
}