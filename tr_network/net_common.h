/********************************************************
# Author       : mknight
# Last modified: 2019-05-01 11:38
# Email        : 824338670@qq.com
# Filename     : net_common.h
# Description  : 
# *******************************************************/
#ifndef __NET_COMMON_H__
#define __NET_COMMON_H__
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdint>
#include <string>

// 宏定义
#define INVALID_SOCKET_FD (-1)
#define INIT_SOCKET_BUFFER_SIZE (1024)
#define MAX_SOCKET_BUFFER_SIZE (4096)

#define socket_fd_t int32_t

// 套接字状态枚举
enum class ESocketStatus : int32_t
{
    E_SOCKET_STATUS_NONE = 0,       // 无效状态
    E_SOCKET_STATUS_LISTEN = 1,     // 监听状态
    E_SOCKET_STATUS_CONNECTED = 2,  // 连接状态
    E_SOCKET_STATUS_BE_CONNECT = 3, // 被连接状态
    E_SOCKET_STATUS_TIMEOUT = 4,    // 超时状态
    E_SOCKET_STATUS_CLOSED = 5,     // 关闭状态
};

// 网络地址数据
struct NetAddr
{
    std::string ip;     // ip地址
    int32_t port = 0;   // 端口
};

// socket 操作码
enum class ESocketOpCode
{
    E_SOCKET_OP_CODE_CORRECT = 0,
    E_SOCKET_OP_SOCK_CREATE_FAILE = 1,  // socket创建失败
    E_SOCKET_OP_CODE_CONNECT_FAILE = 2, // 连接失败
    E_SOCKET_OP_IP_ADDR_ERROR = 3,      // ip地址无效
    E_SOCKET_OP_BIND_ERROR = 4,         // 绑定地址失败
    E_SOCKET_OP_LISTEN_ERROR = 5,       // 绑定地址失败
    E_SOCKET_OP_EPOLL_REG_FAILE = 6,   // epoll注册事件失败
    E_SOCKET_OP_EPOLL_UNREG_FAILE = 6,  // epoll取消事件失败
};

#endif // __NET_COMMON_H__
