/********************************************************
# Author       : mknight
# Last modified: 2019-05-02 11:19
# Email        : 824338670@qq.com
# Filename     : net_connection.h
# Description  : 
# *******************************************************/

#ifndef __NET_CONNECTION_H__
#define __NET_CONNECTION_H__

#include "net_socket.h"

class TConnection : public TSocket
{
public:
    TConnection();
    ~TConnection();
protected:
    // 接受连接
    virtual void HandleAccept();
    // 收数据
    virtual void HandleRecv();
    // 写数据
    virtual void HandleWrite();
    // 出错处理
    virtual void HandleError();
    virtual void OnClose();
    // 读到数据的处理
    virtual void AfterReadData(int32_t read_size);
};
#endif  // __NET_CONNECTION_H__
