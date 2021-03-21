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
#include <functional>
class TBaseServer;
class TConnection : public TSocket
{
public:
    TConnection();
    ~TConnection();
    // 关联一个server
    void AttachServer(TBaseServer * server);
    // 获取关联的server
    TBaseServer * GetServer();
    // 获取连接对象标识Id
    int32_t GetConnId();
    // 重连
    void Reconnect();
    bool CloseReconnect() {return reconnect_;}
    // 设置是否重连
    void SetNeedReconnect(bool connect);
    // 设置重连时候的操作
    void SetReConnectAction(std::function<void (TConnection *connect_pt)> && reconnect_func);
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
    
protected:
    static int32_t connection_id_index_;
protected:
    TBaseServer * server_ = nullptr;
    int32_t conn_id_ = 0;

    // 关闭后是否重连
    bool reconnect_ = false;
    std::function<void (TConnection *connect_pt)> reconnect_func_ = nullptr;
};
#endif  // __NET_CONNECTION_H__
