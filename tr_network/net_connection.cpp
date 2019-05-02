/********************************************************
# Author       : mknight
# Last modified: 2019-05-02 11:19
# Email        : 824338670@qq.com
# Filename     : net_connection.h
# Description  : 
# *******************************************************/
#include "net_connection.h"
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <memory.h>
#include "net_socket.h"
#include "log_module.h"
#include "connect_manager.h"
TConnection::TConnection()
{
}
TConnection::~TConnection()
{
}

// 接受连接
void TConnection::HandleAccept()
{
    TSocket::HandleAccept();
}
// 收数据
void TConnection::HandleRecv()
{
    TSocket::HandleRecv();
}
// 写数据
void TConnection::HandleWrite()
{
    TSocket::HandleWrite();
}
// 出错处理
void TConnection::HandleError()
{
    TSocket::HandleError();
}

void TConnection::OnClose()
{
    g_ConnectMgr.DelConnection(this);
}

void TConnection::AfterReadData(int32_t read_size)
{
    // do nothing
}