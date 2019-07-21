/*=======================================================
# Author       : mknight
# Last modified: 2019-07-13 12:29
# Email        : 824338670@qq.com
# Filename     : game_msg_helper.cpp
# Description  : 
# =======================================================*/

#include "game_msg_helper.h"
#include "tr_msg_packet/msg_tools.h"
#include "server_info_manager.h"
#include "tr_log/log_module.h"
#include "server_session.h"
#include "server_common/server_config.h"
GameMsgHelper::GameMsgHelper()
{
    
}

GameMsgHelper::~GameMsgHelper()
{

}

bool GameMsgHelper::ForwardMessage(int32_t msg_class, 
    int32_t msg_type, 
    const std::string & content, 
    EServerRouteNodeType node_type, 
    int32_t node_index)
{
    auto net_msg = g_MsgTools.MakeNetMessage(msg_class, msg_type, content);
    // 找到对应的服务器信息(目前是不跨服的,跨服node的待续)
    int32_t zone_id = g_ServerConfig.GetZoneId();
    auto server_node = g_ServerManager.GetRouteNodeInfo(node_type, node_index, zone_id);
    if (!server_node)
    {
        TERROR("forwardmessage failed, not found server_node, node_type:" << int32_t(node_type)
            << ", node_index:" << node_index);

        return false;
    }
    if (!server_node->session)
    {
        TERROR("server node, seesion is nullptr, node_type:" << int32_t(node_type)
            << ", node_index:" << node_index);
        return false;
    }
    auto session_pt = server_node->session;
    TINFO("ForwardMessage, session_pt:" << (int64_t)session_pt);
    int32_t msg_size = net_msg.SerializeByteNum();
    EnsureMsgBuffer(msg_size);
    net_msg.Serialize(msg_buffer_, buffer_size_);
    TINFO("ForwardAsyncMessage, req_id:" << net_msg.GetReqNo()
        << ", msg_class:" << net_msg.GetMsgClass()
        << ", msg_type:" << net_msg.GetMsgType()
        << ", msg_size:" << msg_size
        << ", node_type:" << int32_t(node_type)
         << ", node_index:" << node_index);

    if (msg_buffer_ && msg_size > 0)
        session_pt->Send(msg_buffer_, msg_size);
    return true;
}

bool GameMsgHelper::ForwardAsyncMessage(int32_t msg_class, int32_t msg_type, 
        const std::string & content, asyncmsg_callback_t && callback, 
        AsyncMsgParam && cb_param,
        EServerRouteNodeType node_type, 
        int32_t node_index)
{
    auto net_msg = g_MsgTools.MakeNetMessage(msg_class, msg_type, content);

    // 找到对应的服务器信息(目前是不跨服的,跨服node的待续)
    int32_t zone_id = g_ServerConfig.GetZoneId();
    auto server_node = g_ServerManager.GetRouteNodeInfo(node_type, node_index, zone_id);
    if (!server_node)
    {
        TERROR("ForwardAsyncMessage failed, not found server_node, node_type:" << int32_t(node_type)
            << ", node_index:" << node_index);

        return false;
    }
    if (!server_node->session)
    {
        TERROR("server node, seesion is nullptr, node_type:" << int32_t(node_type)
            << ", node_index:" << node_index);
        return false;
    }
    
    if (!g_MsgTools.BindAsyncCallback(&net_msg, std::move(callback), std::move(cb_param)))
        return false;
    auto session_pt = server_node->session;
    TINFO("ForwardMessage, session_pt:" << (int64_t)session_pt);

    int32_t msg_size = net_msg.SerializeByteNum();
    EnsureMsgBuffer(msg_size);
    net_msg.Serialize(msg_buffer_, buffer_size_);

    TINFO("ForwardAsyncMessage, req_id:" << net_msg.GetReqNo() 
        << ", msg_class:" << net_msg.GetMsgClass()
        << ", msg_type:" << net_msg.GetMsgType()
        << ", msg_size:" << msg_size
        << ", node_type:" << int32_t(node_type)
        << ", node_index:" << node_index
        << ", fd_:" << session_pt->GetFd());
    if (msg_buffer_ && msg_size > 0)
        session_pt->Send(msg_buffer_, msg_size);
    
    return true;
}

void GameMsgHelper::EnsureMsgBuffer(int32_t buffer_size)
{
    if (buffer_size_ < buffer_size)
    {
        if (msg_buffer_)
        {
            SAFE_DELETE_PTR(msg_buffer_);
            buffer_size_ = 0;
        }
        msg_buffer_ = new char[buffer_size];
        buffer_size_ = buffer_size;
    }
}

bool GameMsgHelper::ForwardAsyncPbMessage(int32_t msg_class, int32_t msg_type, 
        ::google::protobuf::Message & pb_msg, asyncmsg_callback_t && callback, 
        AsyncMsgParam && cb_param, 
        EServerRouteNodeType node_type, 
        int32_t node_index)
{
    std::string str_content;
    pb_msg.SerializeToString(&str_content);
    return ForwardAsyncMessage(msg_class, msg_type, str_content, std::move(callback),
        std::move(cb_param),
        node_type,
        node_index);
}