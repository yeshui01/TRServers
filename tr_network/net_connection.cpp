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
#include "base_server.h"
TConnection::TConnection()
{
    conn_id_ = connection_id_index_;
    ++connection_id_index_;
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
    // g_ConnectMgr.DelConnection(this);
    if (server_)
    {
        server_->RecycleConnect(this);
    }
}

void TConnection::AfterReadData(int32_t read_size)
{
    // do nothing
}

// 关联一个server
void TConnection::AttachServer(TBaseServer * server)
{
    server_ = server;
}
// 获取关联的server
TBaseServer * TConnection::GetServer()
{
    return server_;
}

int32_t TConnection::GetConnId()
{
    return conn_id_;
}

// 连接标识id，从1000开始
int32_t TConnection::connection_id_index_ = 1000;