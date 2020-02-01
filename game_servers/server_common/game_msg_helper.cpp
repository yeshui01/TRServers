/*=======================================================
# Author       : mknight
# Last modified: 2019-07-13 12:29
# Email        : 824338670@qq.com
# Filename     : game_msg_helper.cpp
# Description  : 
# =======================================================*/
#include "protocol_class.h"
#include "protocol_frame.h"
#include "game_msg_helper.h"
#include "tr_msg_packet/msg_tools.h"
#include "server_info_manager.h"
#include "tr_log/log_module.h"
#include "server_session.h"
#include "server_common/server_config.h"
#include "protcl_frame.pb.h"
#include "server_common/server_define.h"
#include "server_common/server_describe.h"

GameMsgHelper::GameMsgHelper()
{
    
}

GameMsgHelper::~GameMsgHelper()
{
    if (msg_buffer_)
    {
        delete [] msg_buffer_;
        msg_buffer_ = nullptr;
    }
}

std::map<EServerRouteNodeType, std::map<EServerRouteNodeType, std::vector<EServerRouteNodeType> > > GameMsgHelper::s_route_path_ =
{
    {EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, 
        {
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIN, {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA, {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT}},
        }
    },
    {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, 
        {
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, {}},
        }
    },
    {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, 
        {
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, {}},
        }
    },
    {EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA, 
        {
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, {}},
        }
    },
    {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, 
        {
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIN, {}},
        }
    },
};
bool GameMsgHelper::ForwardMessage(int32_t msg_class, 
    int32_t msg_type, 
    const std::string & content, 
    EServerRouteNodeType node_type, 
    int32_t node_index,
    int32_t zone_id/*= -1*/)
{
    
    if (-1 == zone_id)
    {
        zone_id = g_ServerConfig.GetZoneId();
    }
    if (g_MsgHelper.IsGlobalServerNode(node_type))
    {
        zone_id = g_ServerConfig.GetGlobalZoneId();
    }
    else if (g_MsgHelper.IsWorldServerNode(node_type))
    {
        zone_id = g_ServerConfig.GetWorldZoneId();
    }
    
    
    auto it_routes = s_route_path_.find(g_ServerManager.GetCurrentNodeType());
    if (it_routes == s_route_path_.end())
    {
        // 没有定义路由
        TERROR("not define start route node:" << int32_t(g_ServerManager.GetCurrentNodeType()));
        return false;
    }
    auto it_route_nodes = it_routes->second.find(node_type);
    if (it_route_nodes == it_routes->second.end())
    {
        // 没有定义路由
        TERROR("not define end route node:" << int32_t(node_type) << ", start_node:" << int32_t(g_ServerManager.GetCurrentNodeType()));
        return false;
    }

    std::vector<EServerRouteNodeType> &forward_nodes = it_route_nodes->second;

    if (forward_nodes.empty())
    {
        auto server_node = g_ServerManager.GetRouteNodeInfo(node_type, node_index, zone_id);
        if (!server_node)
        {
            TERROR("forwardmessage failed, not found server_node, node_type:" << int32_t(node_type)
                                                                              << ", node_index:" << node_index
                                                                              << ", msg_class:" << msg_class 
                                                                              << ", msg_type:" << msg_type);

            return false;
        }
        if (!server_node->session)
        {
            TERROR("server node, seesion is nullptr, node_type:" << int32_t(node_type)
                                                                 << ", node_index:" << node_index
                                                                 << ", msg_class:" << msg_class 
                                                                 << ", msg_type:" << msg_type);
            return false;
        }
        auto net_msg = g_MsgTools.MakeNetMessage(msg_class, msg_type, content);
        auto session_pt = server_node->session;
        TINFO("ForwardMessage, session_pt:" << (int64_t)session_pt);
        int32_t msg_size = net_msg.SerializeByteNum();
        EnsureMsgBuffer(msg_size);
        net_msg.Serialize(msg_buffer_, buffer_size_);
        TINFO("ForwardMessage no cross, req_id:" << net_msg.GetReqNo()
            << ", msg_class:" << net_msg.GetMsgClass() << "(" << g_ServerDes.GetMsgClassName(net_msg.GetMsgClass()) << ")"
            << ", msg_type:" << net_msg.GetMsgType() << "(" << g_ServerDes.GetMsgTypeName(net_msg.GetMsgClass(), net_msg.GetMsgType()) << ")"
            << ", msg_size:" << msg_size
            << ", node_type:" << int32_t(node_type) << "(" << g_ServerDes.GetServerNodeName(int32_t(node_type)) << ")"
            << ", node_index:" << node_index);

        if (msg_buffer_ && msg_size > 0)
            session_pt->Send(msg_buffer_, msg_size);
    }
    else 
    {
        // 转发到下一个转发节点
        REQMSG(E_FRAME_MSG_FORWARD_MESSAGE) forward_req;
        forward_req.set_des_zone_id(zone_id);
        forward_req.set_des_node_type(int32_t(node_type));
        forward_req.set_des_node_index(node_index);
        forward_req.set_msg_class(msg_class);
        forward_req.set_msg_type(msg_type);
        forward_req.set_msg_content(content);
        std::string forward_content;
        forward_req.SerializeToString(&forward_content);

        auto next_node_type = forward_nodes[0];
        auto next_node_index = 0;   // 转发节点必须是唯一的
        auto net_msg = g_MsgTools.MakeNetMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME), INT_FRAMEMSG(E_FRAME_MSG_FORWARD_MESSAGE), forward_content);
        auto next_node_zone_id = zone_id;
        // // 如果是世界节点
        if (IsWorldServerNode(next_node_type))
        {
            next_node_zone_id = g_ServerConfig.GetWorldZoneId();
        }
        else if (IsGlobalServerNode(next_node_type))
        {
            next_node_zone_id = g_ServerConfig.GetGlobalZoneId();
        }
        else if (IsZoneServerNode(next_node_type) && IsZoneServerNode(g_ServerManager.GetCurrentNodeType()))
        {
            next_node_zone_id = g_ServerConfig.GetZoneId();
        }
        else
        {
            TERROR("unknown node zone type");
            return false;
        }
        
        auto server_node = g_ServerManager.GetRouteNodeInfo(next_node_type, next_node_index, next_node_zone_id);
        if (!server_node)
        {
            TERROR("ForwardAsyncMessage failed, not found server_node, node_type:" << int32_t(next_node_type)
                                                                                   << ", node_index:" << next_node_index
                                                                                   << ", zone_id:" << next_node_zone_id
                                                                                   << ", msg_class:" << msg_class
                                                                                   << ", msg_type:" << msg_type);

            return false;
        }
        if (!server_node->session)
        {
            TERROR("server node, seesion is nullptr, nextnode_type:" << int32_t(next_node_type)
                                                                 << ", nextnode_index:" << next_node_index
                                                                 << ", zone_id:" << next_node_zone_id
                                                                 << ", msg_class:" << msg_class
                                                                 << ", msg_type:" << msg_type);
            return false;
        }

        auto session_pt = server_node->session;
        TINFO("ForwardMessage, session_pt:" << (int64_t)session_pt);

        int32_t msg_size = net_msg.SerializeByteNum();
        EnsureMsgBuffer(msg_size);
        net_msg.Serialize(msg_buffer_, buffer_size_);

        TINFO("ForwardMessage need cross"
              << ", msg_class:" << net_msg.GetMsgClass() << "(" << g_ServerDes.GetMsgClassName(net_msg.GetMsgClass()) << ")"
              << ", msg_type:" << net_msg.GetMsgType() << "(" << g_ServerDes.GetMsgTypeName(net_msg.GetMsgClass(), net_msg.GetMsgType()) << ")"
              << ", msg_size:" << msg_size
              << ", node_type:" << int32_t(node_type) << "(" << g_ServerDes.GetServerNodeName(int32_t(node_type)) << ")"
              << ", node_index:" << node_index
              << ", fd_:" << session_pt->GetFd());
        if (msg_buffer_ && msg_size > 0)
            session_pt->Send(msg_buffer_, msg_size);
    }
    return true;
}

bool GameMsgHelper::ForwardAsyncMessage(int32_t msg_class, int32_t msg_type, 
        const std::string & content, asyncmsg_callback_t && callback, 
        AsyncMsgParam && cb_param,
        EServerRouteNodeType node_type, 
        int32_t node_index,
        int32_t zone_id/*= -1*/,
        MsgAddonParam * addon_param/* = nullptr*/)
{
    if (-1 == zone_id)
    {
        zone_id = g_ServerConfig.GetZoneId();
    }
    if (g_MsgHelper.IsGlobalServerNode(node_type))
    {
        zone_id = g_ServerConfig.GetGlobalZoneId();
    }
    else if (g_MsgHelper.IsWorldServerNode(node_type))
    {
        zone_id = g_ServerConfig.GetWorldZoneId();
    }
    
    auto it_routes = s_route_path_.find(g_ServerManager.GetCurrentNodeType());
    if (it_routes == s_route_path_.end())
    {
        // 没有定义路由
        TERROR("not define start route node:" << int32_t(g_ServerManager.GetCurrentNodeType())
            << ", msg_class:" << msg_class << ", msg_type:" << msg_type);
        return false;
    }
    auto it_route_nodes = it_routes->second.find(node_type);
    if (it_route_nodes == it_routes->second.end())
    {
        // 没有定义路由
        TERROR("not define end route node:" << int32_t(node_type) << ", start_node:" << int32_t(g_ServerManager.GetCurrentNodeType())
            << ", msg_class:" << msg_class << ", msg_type:" << msg_type);
        return false;
    }

    std::vector<EServerRouteNodeType> &forward_nodes = it_route_nodes->second;

    if (forward_nodes.empty())
    {
        auto server_node = g_ServerManager.GetRouteNodeInfo(node_type, node_index, zone_id);
        if (!server_node)
        {
            TERROR("ForwardAsyncMessage failed, not found server_node, node_type:" << int32_t(node_type)
                                                                                   << ", node_index:" << node_index
                                                                                   << ", zone_id:" << zone_id
                                                                                   << ", msg_class:" << msg_class
                                                                                   << ", msg_type:" << msg_type);

            return false;
        }
        if (!server_node->session)
        {
            TERROR("server node, seesion is nullptr, node_type:" << int32_t(node_type)
                                                                 << ", node_index:" << node_index
                                                                 << ", zone_id:" << zone_id
                                                                 << ", msg_class:" << msg_class
                                                                 << ", msg_type:" << msg_type);
            return false;
        }
        auto net_msg = g_MsgTools.MakeNetMessage(msg_class, msg_type, content);
        if (addon_param)
        {
            if (addon_param->role_id > 0)
            {
                net_msg.SetParam(addon_param->role_id);
            }
        }
        if (!g_MsgTools.BindAsyncCallback(&net_msg, std::move(callback), std::move(cb_param)))
            return false;
        auto session_pt = server_node->session;
        TINFO("ForwardMessage, session_pt:" << (int64_t)session_pt);

        int32_t msg_size = net_msg.SerializeByteNum();
        EnsureMsgBuffer(msg_size);
        net_msg.Serialize(msg_buffer_, buffer_size_);

        TINFO("ForwardAsyncMessage no cross, req_no:" << net_msg.GetReqNo()
            << ", msg_class:" << net_msg.GetMsgClass() << "(" << g_ServerDes.GetMsgClassName(net_msg.GetMsgClass()) << ")"
            << ", msg_type:" << net_msg.GetMsgType() << "(" << g_ServerDes.GetMsgTypeName(net_msg.GetMsgClass(), net_msg.GetMsgType()) << ")"
            << ", msg_size:" << msg_size
            << ", node_type:" << int32_t(node_type) << "(" << g_ServerDes.GetServerNodeName(int32_t(node_type)) << ")"
            << ", node_index:" << node_index
            << ", fd_:" << session_pt->GetFd());

        if (msg_buffer_ && msg_size > 0)
            session_pt->Send(msg_buffer_, msg_size);

        return true;
    }
    else 
    {
        // 转发到下一个转发节点
        REQMSG(E_FRAME_MSG_FORWARD_MESSAGE) forward_req;
        forward_req.set_des_zone_id(zone_id);
        forward_req.set_des_node_type(int32_t(node_type));
        forward_req.set_des_node_index(node_index);
        forward_req.set_msg_class(msg_class);
        forward_req.set_msg_type(msg_type);
        forward_req.set_msg_content(content);
        std::string forward_content;
        forward_req.SerializeToString(&forward_content);

        auto next_node_type = forward_nodes[0];
        auto next_node_index = 0;   // 转发节点必须是唯一的
        auto net_msg = g_MsgTools.MakeNetMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME), INT_FRAMEMSG(E_FRAME_MSG_FORWARD_MESSAGE), forward_content);
        auto next_node_zone_id = zone_id;
        // // 如果是世界节点
        if (IsWorldServerNode(next_node_type))
        {
            next_node_zone_id = g_ServerConfig.GetWorldZoneId();
        }
        else if (IsGlobalServerNode(next_node_type))
        {
            next_node_zone_id = g_ServerConfig.GetGlobalZoneId();
        }
        else if (IsZoneServerNode(next_node_type) && IsZoneServerNode(g_ServerManager.GetCurrentNodeType()))
        {
            next_node_zone_id = g_ServerConfig.GetZoneId();
        }
        else
        {
            TERROR("unknown node zone type");
            return false;
        }
        auto server_node = g_ServerManager.GetRouteNodeInfo(next_node_type, next_node_index, next_node_zone_id);
        if (!server_node)
        {
            TERROR("ForwardAsyncMessage failed, not found server_node, node_type:" << int32_t(next_node_type)
                    << ", node_index:" << next_node_index
                    << ", zone_id:" << next_node_zone_id
                    << ", msg_class:" << msg_class << "(" << g_ServerDes.GetMsgClassName(msg_class) << ")"
                    << ", msg_type:" << msg_type << "(" << g_ServerDes.GetMsgTypeName(msg_class, msg_type) << ")"
                    );

            return false;
        }
        if (!server_node->session)
        {
            TERROR("server node, seesion is nullptr, nextnode_type:" << int32_t(next_node_type)
                                                                 << ", nextnode_index:" << next_node_index
                                                                 << ", zone_id:" << next_node_zone_id
                                                                 << ", msg_class:" << msg_class
                                                                 << ", msg_type:" << msg_type);
            return false;
        }

        if (!g_MsgTools.BindAsyncCallback(&net_msg, std::move(callback), std::move(cb_param)))
            return false;
        auto session_pt = server_node->session;
        TINFO("ForwardMessage, session_pt:" << (int64_t)session_pt);

        int32_t msg_size = net_msg.SerializeByteNum();
        EnsureMsgBuffer(msg_size);
        net_msg.Serialize(msg_buffer_, buffer_size_);

        TINFO("ForwardAsyncMessage need cross req_no:" << net_msg.GetReqNo()
            << ", msg_class:" << net_msg.GetMsgClass() << "(" << g_ServerDes.GetMsgClassName(msg_class) << ")"
            << ", msg_type:" << net_msg.GetMsgType() << "(" << g_ServerDes.GetMsgTypeName(msg_class, msg_type) << ")"
            << ", msg_size:" << msg_size
            << ", node_type:" << int32_t(node_type) << "(" << g_ServerDes.GetServerNodeName(int32_t(node_type)) << ")"
            << ", node_index:" << node_index
            << ", fd_:" << session_pt->GetFd());

        if (msg_buffer_ && msg_size > 0)
            session_pt->Send(msg_buffer_, msg_size);
    }
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
        int32_t node_index,
        int32_t zone_id/*= -1*/)
{
    std::string str_content;
    pb_msg.SerializeToString(&str_content);
    return ForwardAsyncMessage(msg_class, msg_type, str_content, std::move(callback),
        std::move(cb_param),
        node_type,
        node_index,
        zone_id);
}

bool GameMsgHelper::ForwardPbMessage(int32_t msg_class, int32_t msg_type, 
        ::google::protobuf::Message & pb_msg, 
        EServerRouteNodeType node_type, 
        int32_t node_index, int32_t zone_id/* = -1*/)
{
    std::string str_content;
    pb_msg.SerializeToString(&str_content);
    return ForwardMessage(msg_class, msg_type, str_content, node_type, node_index, zone_id);
}

bool GameMsgHelper::IsWorldServerNode(EServerRouteNodeType node_type)
{
    static std::vector<EServerRouteNodeType> s_world_nodes = {
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_WORLD,
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_WORLD_CENTER,
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_VIEW_MANAGER,
    };
    return std::find(s_world_nodes.begin(), s_world_nodes.end(), node_type) != s_world_nodes.end();
}

bool GameMsgHelper::IsGlobalServerNode(EServerRouteNodeType node_type)
{
    static std::vector<EServerRouteNodeType> s_global_nodes = {
        // EServerRouteNodeType::E_SERVER_ROUTE_NODE_WORLD,
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIN
    };

    return std::find(s_global_nodes.begin(), s_global_nodes.end(), node_type) != s_global_nodes.end();
}

bool GameMsgHelper::IsZoneServerNode(EServerRouteNodeType node_type)
{
    static std::vector<EServerRouteNodeType> s_zone_nodes = {
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT,
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC,
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER,
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA,
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE
    };

    return std::find(s_zone_nodes.begin(), s_zone_nodes.end(), node_type) != s_zone_nodes.end();
}

AsyncMsgParam GameMsgHelper::GenAsyncMsgEnvParam(TConnection *session_pt, const NetMessage * message_pt)
{
    AsyncMsgParam async_env;
    async_env.session_pt = session_pt;
    if (message_pt)
    {
        async_env.msg_class = message_pt->GetMsgClass();
        async_env.msg_type = message_pt->GetMsgType();
        async_env.req_no = message_pt->GetReqNo();
        async_env.rep_no = message_pt->GetRepNo();
    }
    return async_env;
}

void GameMsgHelper::SendAsyncRepMsg(::google::protobuf::Message & pb_msg, const AsyncMsgParam & async_param)
{
    std::string str;
    pb_msg.SerializeToString(&str);

    auto rep_net_msg = g_MsgTools.MakeNetMessage(async_param.msg_class,
                                                 async_param.msg_type, 
                                                 str);
    if (async_param.req_no > 0)
    {
        rep_net_msg.SetRepNo(async_param.req_no);
    }
    TINFO("SendAsyncRepMsg rep_no:" << async_param.req_no 
        << ", msg(" << async_param.msg_class << "," << async_param.msg_type << ")"
        << ", msg_data:" << pb_msg.ShortDebugString());

    int32_t packet_size = rep_net_msg.SerializeByteNum();
    std::vector<char> buffer(packet_size, '\0');
    rep_net_msg.Serialize(buffer.data(), packet_size);
    if (async_param.session_pt)
    {
        async_param.session_pt->Send(buffer.data(), packet_size);
    }
    else
    {
        TERROR("async_param.session is nullptr, msg_class:" << async_param.msg_class << ", msg_type:" << async_param.msg_type);
    }
}

void GameMsgHelper::SendAsyncRepNetMsg(const NetMessage * rep_net_msg, const AsyncMsgParam & async_param)
{
    if (!rep_net_msg)
    {
        return;
    }
    
    TINFO("SendAsyncRepNetMsg rep_no:" << async_param.req_no
        << ", msg(" << async_param.msg_class << "," << async_param.msg_type << ")");

    int32_t packet_size = rep_net_msg->SerializeByteNum();
    std::vector<char> buffer(packet_size, '\0');
    (const_cast<NetMessage *>(rep_net_msg))->Serialize(buffer.data(), packet_size);
    if (async_param.session_pt)
    {
        async_param.session_pt->Send(buffer.data(), packet_size);
    }
    else
    {
        TERROR("async_param.session is nullptr, msg_class:" << async_param.msg_class << ", msg_type:" << async_param.msg_type);
    }
}