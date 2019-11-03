/*=======================================================
# Author       : mknight
# Last modified: 2019-06-01 12:17
# Email        : 824338670@qq.com
# Filename     : server_session.h
# Description  : 
# =======================================================*/
#ifndef __TR_SERVER_SESSION_H__
#define __TR_SERVER_SESSION_H__

#include "net_connection.h"
#include "server_define.h"
#include <functional>
#include <vector>
// 会话的通道类型
enum class ESessionChannelType : int32_t
{
    E_CHANNEL_NONE = 0,             // 未知会话类型
    E_CHANNEL_SERVER_TO_SERVER = 1, // 服务器之间的会话
    E_CHANNEL_CLIENT_TO_SERVER = 2, // 客户端和服务器的会话
};

// 通道数据
struct SessionChannelInfo
{
    void Reset()
    {
        node_type = EServerRouteNodeType::E_SERVER_ROUTE_NODE_NONE;
        user_id = 0;
        server_index = 0;
        server_id = 0;
    }
    // 对应的服务器类型
    EServerRouteNodeType node_type = EServerRouteNodeType::E_SERVER_ROUTE_NODE_NONE;
    // 服务器的idex标号
    int32_t server_index = 0;
    // 服务器id
    int32_t server_id = 0;
    // 对应的客户端标识
    int64_t user_id = 0;
    // 
};

class ServerSession;
using session_close_callback_t = std::function<void (ServerSession* session)>;

class ServerSession : public TConnection
{
public:
    ServerSession();
    ~ServerSession();

    void AfterReadData(int32_t read_size) override;
	void OnClose() override;

public:
    // 获取会话通道类型
    ESessionChannelType GetChannelType();
    // 设置会话通道类型
    void SetChannelType(ESessionChannelType e_type);
    // 获取通道信息
    const SessionChannelInfo & GetChannleInfo();
    // 设置通道服务器信息
    void SetChannelServerInfo(EServerRouteNodeType route_type, int32_t index_id, int32_t server_id);
    // 设置通道客户端标识
    void SetChannelUserId(int64_t user_id);
    // 计算服务器id
    static int32_t CalcServerId(EServerRouteNodeType route_type, int32_t index_id);
    // 注册关闭回调
    void AddCloseCallback(session_close_callback_t && close_cb);
protected:
    // 通道类型
    ESessionChannelType channel_type_ = ESessionChannelType::E_CHANNEL_NONE;
    // 会话数据
    SessionChannelInfo channel_info_;
    // 连接关闭回调
    std::vector<session_close_callback_t> close_callbacks_;
};
#endif // __TR_SERVER_SESSION_H__
