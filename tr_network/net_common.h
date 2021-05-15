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

// 通信模式
enum ESocketTransMode
{
    E_SOCKET_TRANS_MODE_TCP = 0,
    E_SOCKET_TRANS_MODE_UNIX_SOCK_FILE = 1,
};

// 网络地址数据
struct NetAddr
{
    std::string ip;     // ip地址
    int32_t port = 0;   // 端口
    std::string unix_sock_file;
};

// socket 操作码
enum class ESocketOpCode : int32_t
{
    E_SOCKET_OP_CODE_CORRECT = 0,
    E_SOCKET_OP_SOCK_CREATE_FAIL = 1,  // socket创建失败
    E_SOCKET_OP_CODE_CONNECT_FAIL = 2, // 连接失败
    E_SOCKET_OP_IP_ADDR_ERROR = 3,      // ip地址无效
    E_SOCKET_OP_BIND_ERROR = 4,         // 绑定地址失败
    E_SOCKET_OP_LISTEN_ERROR = 5,       // 绑定地址失败
    E_SOCKET_OP_EPOLL_REG_FAIL = 6,   // epoll注册事件失败
    E_SOCKET_OP_EPOLL_UNREG_FAIL = 6,  // epoll取消事件失败
};

// 服务器运行阶段
enum class EServerRunStep : int32_t
{
    E_SERVER_RUN_STEP_NONE = 0,        // 默认初始值
    E_SERVER_RUN_STEP_CHECK = 1,       // 运行前的检测阶段
    E_SERVER_RUN_STEP_LISTEN = 2,      // 监听阶段
    E_SERVER_RUN_STEP_WILL_RUNNING = 3,  // 即将运行
    E_SERVER_RUN_STEP_WAIT_BOOTUP = 4,  // 等待启动
    E_SERVER_RUN_STEP_WAIT_OTHER_SERVERS = 5,   // 等待其他服务器启动
    E_SERVER_RUN_STEP_PRE_RUN = 6,      // 正式运行的前一刻
    E_SERVER_RUN_STEP_RUNNING = 7,      // 正常监听运行阶段
    E_SERVER_RUN_STEP_STOP = 8,        // 停服阶段
};
#endif // __NET_COMMON_H__
