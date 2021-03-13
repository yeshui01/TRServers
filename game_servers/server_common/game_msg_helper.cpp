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
#include "json/json.h"

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
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC_CENTER, {}},
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
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOG, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC_CENTER, {}},
        }
    },
    {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC_CENTER, 
        {
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOG, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, {}},
        }
    },
    {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, 
        {
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOG, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC_CENTER, {}},
        }
    },
    {EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA, 
        {
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC_CENTER, {}},
        }
    },
    {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOG, 
        {
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC_CENTER, {}},
        }
    },
    {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, 
        {
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIN, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOG, {}},
            {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC_CENTER, {}},
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
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE,
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOG,
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

void GameMsgHelper::SendAsyncRepMsgJson2(::google::protobuf::Message & pb_msg, const AsyncMsgParam & async_param, int32_t isok)
{
    std::string str;
    // str = pb_msg.DebugString();
    // PbMsgToJsonStr(pb_msg, str);
    Json::Value jv;
    PbMsg2Json(pb_msg, jv);
    jv.removeMember("isok");
    // str = jv.toJsonStringFast();
    Json::Value jv_rep;
    jv_rep["isok"] = Json::Int(isok);
    //] = async_param.msg_class * 1000 + async_param.msg_type;
    jv_rep["msg_data"] = jv;
    std::string final_str = jv_rep.toStyledString();
    auto rep_net_msg = g_MsgTools.MakeNetMessage(async_param.msg_class,
                                                 async_param.msg_type, 
                                                 final_str);
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

void GameMsgHelper::SendAsyncRepMsgJson(::google::protobuf::Message & pb_msg, const AsyncMsgParam & async_param)
{
    std::string str;
    // str = pb_msg.DebugString();
    PbMsgToJsonStr(pb_msg, str);
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

 
bool GameMsgHelper::Json2PbMsg(const Json::Value& src, ProtobufMsg& dst, bool str2enum)
{
    const ProtobufDescriptor* descriptor = dst.GetDescriptor();
    const ProtobufReflection* reflection = dst.GetReflection();
    if (nullptr == descriptor || nullptr == reflection)
    {
        return false;
    }
    
    int32_t count = descriptor->field_count();
    for (int32_t i = 0; i < count; ++i) 
    {
        const ProtobufFieldDescriptor* field = descriptor->field(i);
        if (nullptr == field) continue;
 
        if(!src.isMember(field->name()))
        {
            continue;
        }
        const Json::Value& value = src[field->name()];
 
        if (field->is_repeated()) 
        {
            if (!value.isArray())
            {
                TERROR("pb error");
                return false;
            } 
            else 
            {
                Json2RepeatedMessage(value, dst, field, reflection, str2enum);
                continue;
            }
        }
        switch (field->type()) 
        {
            case ProtobufFieldDescriptor::TYPE_BOOL: 
            {
                if (value.isBool())
                {
                    reflection->SetBool(&dst, field, value.asBool());
                }
 
                else if (value.isInt())
                {
                    reflection->SetBool(&dst, field, value.isInt());
                }
 
                else if (value.isString())
                {
                    if (value.asString() == "true")
                        reflection->SetBool(&dst, field, true);
                    else if (value.asString() == "false")
                        reflection->SetBool(&dst, field, false);
                }
                break;
            } 
 
            case ProtobufFieldDescriptor::TYPE_INT32:
            case ProtobufFieldDescriptor::TYPE_SINT32:
            case ProtobufFieldDescriptor::TYPE_SFIXED32: 
            {
                if (value.isInt()) reflection->SetInt32(&dst, field, value.asInt());
                break;
            }
 
            case ProtobufFieldDescriptor::TYPE_UINT32:
            case ProtobufFieldDescriptor::TYPE_FIXED32: 
            {
                if (value.isUInt()) reflection->SetUInt32(&dst, field, value.asUInt());
                break;
            }  
 
            case ProtobufFieldDescriptor::TYPE_INT64:
            case ProtobufFieldDescriptor::TYPE_SINT64:
            case ProtobufFieldDescriptor::TYPE_SFIXED64: 
            {
                if (value.isInt64() || value.isInt() || value.isNumeric()) 
                {
                    reflection->SetInt64(&dst, field, value.asInt64());
                }
                else if (value.isUInt64())
                {
                    reflection->SetInt64(&dst, field, value.asUInt64());
                }
                break;
            } 
            case ProtobufFieldDescriptor::TYPE_UINT64:
            case ProtobufFieldDescriptor::TYPE_FIXED64: 
            {
                if (value.isUInt64() || value.isUInt() || value.isNumeric()) reflection->SetUInt64(&dst, field, value.asUInt64());
                break;
            } 
 
            case ProtobufFieldDescriptor::TYPE_FLOAT: 
            {
                if (value.isDouble()) reflection->SetFloat(&dst, field, value.asDouble());
                break;
            }
 
            case ProtobufFieldDescriptor::TYPE_DOUBLE: 
            {
                if (value.isDouble()) reflection->SetDouble(&dst, field, value.asDouble());
                break;
            } 
 
            case ProtobufFieldDescriptor::TYPE_STRING:
            case ProtobufFieldDescriptor::TYPE_BYTES: 
            {
                if (value.isString()) reflection->SetString(&dst, field, value.asString());
                break;
            } 
 
            case ProtobufFieldDescriptor::TYPE_MESSAGE: 
            {
                if (value.isObject()) Json2PbMsg(value, *reflection->MutableMessage(&dst, field));
                break;
            } 
            default:
            {
                TERROR("unkonw type handle,field_name:" << field->name() << ", json value type:" << value.type());
                break;
            }
        }
    }
    return true;
}
// pb 与 json 互转
bool GameMsgHelper::JsonStr2PbMsg(const std::string& src, ProtobufMsg& dst, bool str2enum) 
{
    Json::Value value;
    Json::Reader reader(Json::Features::strictMode());
    if (!reader.parse(src, value))
    {
        TERROR("parse json string is fail,str=" << src);
        return false;
    }
    if(true != Json2PbMsg(value, dst, str2enum))
    {
        TERROR("pb convert error");
        return false;
    }
    return true;
}

void GameMsgHelper::RepeatedMessage2Json(const ProtobufMsg& message, 
                                        const ProtobufFieldDescriptor* field,
                                        const ProtobufReflection* reflection, 
                                        Json::Value& json, bool enum2str) 
{
    if (nullptr == field || nullptr == reflection) 
    {
        PbMsg2Json(message, json);
    }
 
    for (int32_t i = 0; i < reflection->FieldSize(message, field); ++i) 
    {
        Json::Value tmp_json;
        switch (field->type()) 
        {
            case ProtobufFieldDescriptor::TYPE_MESSAGE: 
            {
                const ProtobufMsg& tmp_message = reflection->GetRepeatedMessage(message, field, i);
                if (0 != tmp_message.ByteSize()) 
                {
                    PbMsg2Json(tmp_message, tmp_json);
                }
                break;
            } 
 
            case ProtobufFieldDescriptor::TYPE_BOOL:
                tmp_json[field->name()] = reflection->GetRepeatedBool(message, field, i) ? true : false;
                break;
 
            case ProtobufFieldDescriptor::TYPE_ENUM: 
            {
                const ::google::protobuf::EnumValueDescriptor* enum_value_desc = reflection->GetRepeatedEnum(message, field, i);
                if (enum2str) 
                {
                    tmp_json = enum_value_desc->name();
                } 
                else 
                {
                    tmp_json = enum_value_desc->number();
                }
                break;
            } 
 
            case ProtobufFieldDescriptor::TYPE_INT32:
            case ProtobufFieldDescriptor::TYPE_SINT32:
            case ProtobufFieldDescriptor::TYPE_SFIXED32:
                // tmp_json[field->name()] = reflection->GetRepeatedInt32(message, field, i);
                // json.append(Json::Int(reflection->GetRepeatedInt32(message, field, i)));
                tmp_json = Json::Int(reflection->GetRepeatedInt32(message, field, i));
                break;
 
            case ProtobufFieldDescriptor::TYPE_UINT32:
            case ProtobufFieldDescriptor::TYPE_FIXED32:
                // tmp_json[field->name()] = reflection->GetRepeatedUInt32(message, field, i);
                // json.append(Json::UInt(reflection->GetRepeatedInt32(message, field, i)));
                tmp_json = Json::UInt(reflection->GetRepeatedInt32(message, field, i));
                break;
 
            case ProtobufFieldDescriptor::TYPE_INT64:
            case ProtobufFieldDescriptor::TYPE_SINT64:
            case ProtobufFieldDescriptor::TYPE_SFIXED64:
                // tmp_json[field->name()] = (Json::Int64)reflection->GetRepeatedInt64(message, field, i);
                // json.append(Json::Int64(reflection->GetRepeatedInt64(message, field, i)));
                tmp_json = Json::Int64(reflection->GetRepeatedInt64(message, field, i));
                break;
 
            case ProtobufFieldDescriptor::TYPE_UINT64:
            case ProtobufFieldDescriptor::TYPE_FIXED64:
                // tmp_json[field->name()] = Json::UInt64(reflection->GetRepeatedUInt64(message, field, i));
                // json.append(Json::UInt64(reflection->GetRepeatedUInt64(message, field, i)));
                tmp_json = Json::UInt64(reflection->GetRepeatedUInt64(message, field, i));
                break;
 
            case ProtobufFieldDescriptor::TYPE_FLOAT:
                tmp_json[field->name()] = reflection->GetRepeatedFloat(message, field, i);
                break;
 
            case ProtobufFieldDescriptor::TYPE_STRING:
            case ProtobufFieldDescriptor::TYPE_BYTES:
                tmp_json[field->name()] = reflection->GetRepeatedString(message, field, i);
                break;
 
            default:
                break;
        }
        json.append(tmp_json);
    }
}

void GameMsgHelper::PbMsg2Json(const ProtobufMsg& src, Json::Value& dst, bool enum2str) 
{
    const ProtobufDescriptor* descriptor = src.GetDescriptor();
    const ProtobufReflection* reflection = src.GetReflection();
    if (nullptr == descriptor || nullptr == descriptor)
    {
        return;
    }
 
    int32_t count = descriptor->field_count();
 
    for (int32_t i = 0; i < count; ++i) 
    {
        const ProtobufFieldDescriptor* field = descriptor->field(i);
        
        if (field->is_repeated()) 
        {
            dst[field->name()] = Json::Value(Json::ValueType::arrayValue);
            
            if (reflection->FieldSize(src, field) > 0)
                RepeatedMessage2Json(src, field, reflection, dst[field->name()], enum2str);
            continue;
        }
 
 
        // if (!reflection->HasField(src, field)) 
        // {
        //     continue;
        // }
 
        switch (field->type()) 
        {
            case ProtobufFieldDescriptor::TYPE_MESSAGE: 
            {
                const ProtobufMsg& tmp_message = reflection->GetMessage(src, field);
                if (0 != tmp_message.ByteSize())
                {
                    PbMsg2Json(tmp_message, dst[field->name()]);
                }
                else
                {
                    dst[field->name()] = Json::Value(Json::ValueType::objectValue);
                }
                break;
            } 
 
            case ProtobufFieldDescriptor::TYPE_BOOL:
            {
                dst[field->name()] = reflection->GetBool(src, field) ? true : false;
                break;
            }
            case ProtobufFieldDescriptor::TYPE_ENUM: 
            {
                const ::google::protobuf::EnumValueDescriptor* enum_value_desc = reflection->GetEnum(src, field);
                if (enum2str) {
                    dst[field->name()] = enum_value_desc->name();
                } else {
                    dst[field->name()] = enum_value_desc->number();
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_INT32:
            case ProtobufFieldDescriptor::TYPE_SINT32:
            case ProtobufFieldDescriptor::TYPE_SFIXED32:
            {
                if (!reflection->HasField(src, field))
                {
                    dst[field->name()] = Json::Int(0);
                }
                else
                {
                    dst[field->name()] = Json::Int(reflection->GetInt32(src, field));
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_UINT32:
            case ProtobufFieldDescriptor::TYPE_FIXED32:
            {
                if (!reflection->HasField(src, field))
                {
                    dst[field->name()] = Json::UInt(0);
                }
                else
                {
                    dst[field->name()] = Json::UInt(reflection->GetUInt32(src, field));
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_INT64:
            case ProtobufFieldDescriptor::TYPE_SINT64:
            case ProtobufFieldDescriptor::TYPE_SFIXED64:
            {
                if (!reflection->HasField(src, field))
                {
                    dst[field->name()] = Json::Int64(0);
                }
                else
                {
                    dst[field->name()] = Json::Int64(reflection->GetInt64(src, field));
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_UINT64:
            case ProtobufFieldDescriptor::TYPE_FIXED64:
            {
                if (!reflection->HasField(src, field))
                {
                    dst[field->name()] = Json::UInt64(0);
                }
                else
                {
                    dst[field->name()] = Json::UInt64(reflection->GetUInt64(src, field));
                }
                break;
            }
                
 
            case ProtobufFieldDescriptor::TYPE_FLOAT:
            {
                if (!reflection->HasField(src, field))
                {
                    dst[field->name()] = reflection->GetFloat(src, field);
                }
                else
                {
                    dst[field->name()] = 0;
                }
                break;
            }
                
 
            case ProtobufFieldDescriptor::TYPE_STRING:
            case ProtobufFieldDescriptor::TYPE_BYTES:
            {
                if (!reflection->HasField(src, field))
                {
                    dst[field->name()] = "x";
                }
                else
                {
                    dst[field->name()] = reflection->GetString(src, field);
                }
                
                break;
            }
            default:
                break;
        }
    }
}

void GameMsgHelper::PbMsgToJsonStr(const ::google::protobuf::Message& src, std::string& dst, bool enum2str)
{
    Json::Value value;
    PbMsg2Json(src, value, enum2str);
    Json::FastWriter writer;
    dst = writer.write(value);
}

bool GameMsgHelper::Json2RepeatedMessage(const Json::Value& json, ProtobufMsg& message, 
                                const ProtobufFieldDescriptor* field,
                                const ProtobufReflection* reflection,
                                bool str2enum) 
{
    int32_t count = json.size();
    for (int32_t j = 0; j < count; ++j) 
    {
        switch (field->type()) 
        {
            case ProtobufFieldDescriptor::TYPE_BOOL: 
            {
                if (json.isBool())
                {
                    reflection->AddBool(&message, field, json[j].asBool());
                }
                else if (json[j].isInt())
                {
                    reflection->AddBool(&message, field, json[j].asInt());
                }
                else if (json[j].isString()) 
                {
                    if (json[j].asString() == "true")
                    {
                        reflection->AddBool(&message, field, true);
                    }
                    else if (json[j].asString() == "false")
                    {
                        reflection->AddBool(&message, field, false);
                    }
                }
                break;
            } 
 
            case ProtobufFieldDescriptor::TYPE_ENUM: 
            {
                const ::google::protobuf::EnumDescriptor *pedesc = field->enum_type();
                const ::google::protobuf::EnumValueDescriptor* pevdesc = nullptr;
                if (str2enum) 
                {
                    pevdesc = pedesc->FindValueByName(json[j].asString());
 
                } 
                else 
                {
                    pevdesc = pedesc->FindValueByNumber(json[j].asInt());
                }
                if (nullptr != pevdesc) 
                {
                    reflection->AddEnum(&message, field, pevdesc);
                }
                break;
            } 
 
            case ProtobufFieldDescriptor::TYPE_INT32:
            case ProtobufFieldDescriptor::TYPE_SINT32:
            case ProtobufFieldDescriptor::TYPE_SFIXED32: 
            {
                if (json[j].isInt()) reflection->AddInt32(&message, field, json[j].asInt());
            } break;
 
            case ProtobufFieldDescriptor::TYPE_UINT32:
            case ProtobufFieldDescriptor::TYPE_FIXED32: 
            {
                if (json[j].isUInt()) reflection->AddUInt32(&message, field, json[j].asUInt());
            } break;
 
            case ProtobufFieldDescriptor::TYPE_INT64:
            case ProtobufFieldDescriptor::TYPE_SINT64:
            case ProtobufFieldDescriptor::TYPE_SFIXED64: 
            {
                if (json[j].isInt()) reflection->AddInt64(&message, field, json[j].asInt64());
            } break;
 
            case ProtobufFieldDescriptor::TYPE_UINT64:
            case ProtobufFieldDescriptor::TYPE_FIXED64: 
            {
                if (json[j].isUInt()) reflection->AddUInt64(&message, field, json[j].asUInt64());
            } break;
 
            case ProtobufFieldDescriptor::TYPE_FLOAT: 
            {
                if (json[j].isDouble()) reflection->AddFloat(&message, field, json[j].asDouble());
            } break;
 
            case ProtobufFieldDescriptor::TYPE_DOUBLE: 
            {
                if (json[j].isDouble()) reflection->AddDouble(&message, field, json[j].asDouble());
            } break;
 
            case ProtobufFieldDescriptor::TYPE_MESSAGE: 
            {
                if (json[j].isObject()) Json2PbMsg(json[j], *reflection->AddMessage(&message, field));
            } break;
 
            case ProtobufFieldDescriptor::TYPE_STRING:
            case ProtobufFieldDescriptor::TYPE_BYTES: 
            {
                if (json[j].isString()) reflection->AddString(&message, field, json[j].asString());
            } break;
 
            default:
            {
                break;
            }
        }
    }
    return true;
}