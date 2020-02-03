/*=======================================================
# Author       : mknight
# Last modified: 2019-07-20 14:32
# Email        : 824338670@qq.com
# Filename     : root_frame_handle.cpp
# Description  : 
# =======================================================*/

#include "root_frame_handle.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "protcl_frame.pb.h"
#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
#include "protocol_error_code.h"
#include "root_server/root_server.h"
#include "server_common/game_msg_helper.h"
#include "server_common/server_config.h"
#include "server_common/client_net_node.h"
#include "server_common/server_describe.h"
RootFrameHandler::RootFrameHandler()
{

}

RootFrameHandler::~RootFrameHandler()
{

}

void RootFrameHandler::BindMsgHandle()
{
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_REGISTER_SERVER_INFO), RootFrameHandler, OnRegisterServerInfo);
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_QUERY_SERVER_NODE_LIST), RootFrameHandler, OnQueryServerNodeList);
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_XS_TO_ROOT_WAIT_OTHERS), RootFrameHandler, OnServerWaitOtherStart);
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_FORWARD_MESSAGE), RootFrameHandler, OnForwardMessage);
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_GG2ROOT_CLIENT_OFFLINE), RootFrameHandler, OnGateClientOffline);
}

EMsgHandleResult RootFrameHandler::OnRegisterServerInfo(TConnection *session_pt, const NetMessage * message_pt)
{
    TINFO("OnRegisterServerInfo");
    REQMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) req;
    if (!STRING_TO_PBMSG(message_pt->GetContent(), req))
    {
        REPMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) rep;
        rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_PB_PARSE_ERROR));
        TERROR("parse pbmsg failed, msg_class:" << message_pt->GetMsgClass() << ", msg_type:" << message_pt->GetMsgType());
        rep.SerializeToString(&rep_content_);
        return EMsgHandleResult::E_MSG_HANDLE_RETURN_CONTENT;
    }
    TINFO("req:" << req.ShortDebugString());
    REPMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) rep;
    rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_CORRECT));
    TINFO("recv server register info:" << g_ServerDes.GetServerNodeName(req.server_node().node_type()) 
        << " index:" << req.server_node().server_index()
        << ", zone_id:" << req.server_node().zone_id());

    g_ServerManager.AddServerInfo(dynamic_cast<ServerSession*>(session_pt), EServerRouteNodeType(req.server_node().node_type()), 
        EServerType(req.server_node().server_type()), 
        req.server_node().server_index(),
        req.server_node().zone_id());
    
    RETURN_REP_CONTENT(rep);
}

EMsgHandleResult RootFrameHandler::OnQueryServerNodeList(TConnection *session_pt, const NetMessage * message_pt)
{
    TINFO("OnQueryServerNodeList");
    REQMSG(E_FRAME_MSG_QUERY_SERVER_NODE_LIST) req;
    if (!STRING_TO_PBMSG(message_pt->GetContent(), req))
    {
        REPMSG(E_FRAME_MSG_QUERY_SERVER_NODE_LIST) rep;
        rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_PB_PARSE_ERROR));
        TERROR("parse pbmsg failed, msg_class:" << message_pt->GetMsgClass() << ", msg_type:" << message_pt->GetMsgType());
        rep.SerializeToString(&rep_content_);
        return EMsgHandleResult::E_MSG_HANDLE_RETURN_CONTENT;
    }
    
    REPMSG(E_FRAME_MSG_QUERY_SERVER_NODE_LIST) rep;
    rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_CORRECT));
    const std::vector<ServerInfo*> * node_list = g_ServerManager.GetRouteTypeServers(EServerRouteNodeType(req.node_type()));
    if (node_list)
    {
        for (auto && v : *node_list)
        {
            auto pb_node = rep.add_server_list();
            if (!pb_node)
                continue;
            pb_node->set_node_type(int32_t(v->node_type));
            pb_node->set_server_type(int32_t(v->server_type));
            pb_node->set_server_index(v->server_index);
            pb_node->set_zone_id(v->zone_id);
        }
    }
    RETURN_REP_CONTENT(rep);
}

EMsgHandleResult RootFrameHandler::OnServerWaitOtherStart(TConnection *session_pt, const NetMessage * message_pt)
{
    TINFO("OnServerWaitOtherStart");
    REQMSG(E_FRAME_MSG_XS_TO_ROOT_WAIT_OTHERS) req;
    if (!STRING_TO_PBMSG(message_pt->GetContent(), req))
    {
        REPMSG(E_FRAME_MSG_XS_TO_ROOT_WAIT_OTHERS) rep;
        rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_PB_PARSE_ERROR));
        TERROR("parse pbmsg failed, msg_class:" << message_pt->GetMsgClass() << ", msg_type:" << message_pt->GetMsgType());
        rep.SerializeToString(&rep_content_);
        return EMsgHandleResult::E_MSG_HANDLE_RETURN_CONTENT;
    }
    
    REPMSG(E_FRAME_MSG_XS_TO_ROOT_WAIT_OTHERS) rep;
    rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_CORRECT));
    RootServer * server_pt = dynamic_cast<RootServer*>(session_pt->GetServer());
    if (server_pt)
    {
        server_pt->AddWaitStart(EServerRouteNodeType(req.node_type()));
        if (server_pt->CheckAllWaitStart())
        {
            // 通知其他服务器,可以开始运行了
            REQMSG(E_FRAME_MSG_ROOT_TO_XS_START_RUN) push;
            auto & wait_start_nodes = server_pt->GetWaitStartNodes();
            PBMSG_TO_STR(push, str);
            for (auto it = wait_start_nodes.begin(); it != wait_start_nodes.end(); ++it)
            {
                for (int32_t index = 0; index < it->second; ++index)
                {
                    g_MsgHelper.ForwardMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
                                               INT_FRAMEMSG(E_FRAME_MSG_ROOT_TO_XS_START_RUN),
                                               str, it->first,
                                               index);
                    
                    TINFO("root tell server start, server_name:" << server_pt->GetServerNameByNodeType(it->first).second
                        << ", index:" << index);
                }
                
            }
            TINFO("tell all other server to start");
        }
    }
    else
    {
        TERROR("server is nullptr");
    }
    
    RETURN_REP_CONTENT(rep);
}

EMsgHandleResult RootFrameHandler::OnForwardMessage(TConnection *session_pt, const NetMessage * message_pt)
{
    TINFO("OnForwardMessage");
    REQMSG(E_FRAME_MSG_FORWARD_MESSAGE) req;
    if (!STRING_TO_PBMSG(message_pt->GetContent(), req))
    {
        REPMSG(E_FRAME_MSG_FORWARD_MESSAGE) rep;
        rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_PB_PARSE_ERROR));
        TERROR("parse pbmsg failed, msg_class:" << message_pt->GetMsgClass() << ", msg_type:" << message_pt->GetMsgType());
        if (message_pt->GetReqNo() > 0)
        {
            // rep.SerializeToString(&rep_content_);
            // return EMsgHandleResult::E_MSG_HANDLE_RETURN_CONTENT;
            RETURN_REP_CONTENT(rep)
        }
        RETURN_NO_HANDLE;
    }

    if (message_pt->GetReqNo() > 0)
    {
        auto req_no = message_pt->GetReqNo();
        AsyncMsgParam async_env;
        async_env.session_pt = session_pt;
        g_MsgHelper.ForwardAsyncMessage(req.msg_class(), 
                                        req.msg_type(),
                                        req.msg_content(),
        [req_no](const NetMessage *rep_msg, const AsyncMsgParam &cb_param)->void {
            // 消息转发回去
            if (cb_param.session_pt)
            {
                auto rep_net_msg = g_MsgTools.MakeNetMessage(rep_msg->GetMsgClass(),
                    rep_msg->GetMsgType(),
                    rep_msg->GetContent());

                rep_net_msg.SetRepNo(req_no);
                
                // char * buffer = new char [rep_net_msg.SerializeByteNum()];
                // rep_net_msg.Serialize(buffer, rep_net_msg.SerializeByteNum());
                // cb_param.session_pt->Send(buffer, rep_net_msg.SerializeByteNum());
                // delete [] buffer;
                int32_t packet_size = rep_net_msg.SerializeByteNum();
                std::vector<char> buffer(packet_size, '\0');
                rep_net_msg.Serialize(buffer.data(), packet_size);
                cb_param.session_pt->Send(buffer.data(), packet_size);
            }
            else 
            {
                TWARN("async forward message ,not found session to reply");
            }
        }, std::move(async_env), EServerRouteNodeType(req.des_node_type()),
        req.des_node_index(), req.des_zone_id());
    }
    else 
    {
        g_MsgHelper.ForwardMessage(req.msg_class(), 
                                   req.msg_type(),
                                   req.msg_content(),
                                   EServerRouteNodeType(req.des_node_type()),
                                   req.des_node_index(), 
                                   req.des_zone_id());
    }
    RETURN_NO_HANDLE;
}

EMsgHandleResult RootFrameHandler::OnGateClientOffline(TConnection *session_pt, const NetMessage * message_pt)
{
    REQMSG(E_FRAME_MSG_GG2ROOT_CLIENT_OFFLINE) req;
    // REPMSG(E_FRAME_MSG_GG2ROOT_CLIENT_OFFLINE) rep;
    if (!STRING_TO_PBMSG(message_pt->GetContent(), req))
    {
        TERROR("parse pbmsg failed");
        // SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_PB_PARSE_ERROR, rep);
        RETURN_NO_HANDLE;
    }
    TINFO("req_E_FRAME_MSG_GG2ROOT_CLIENT_OFFLINE:" << req.ShortDebugString());

    auto user_id = req.user_id();
    // 通知其他服务器,客户端下线
    auto user_pt = g_ClientNetNodeMgr.HoldClientUser(user_id);
    if (!user_pt)
    {
        TERROR("not found user_id, may be logic error, user_id:" << user_id);
        RETURN_NO_HANDLE;
    }
    // TODO:通知其他相关服务器,客户端下线
    REQMSG(E_FRAME_MSG_ROOT_XS_CLIENT_OFFLINE) offline_req;
    offline_req.set_user_id(user_id);
    offline_req.set_reason(E_USER_OFFLINE_REASON_GATE_OFFLINE);
    // tmp code ,这里需要做个群组异步处理，等其他服务器处理完再处理自己的
    for (auto it_node = user_pt->net_nodes_.begin(); it_node != user_pt->net_nodes_.end(); ++it_node)
    {
        g_MsgHelper.ForwardPbMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
            E_FRAME_MSG_ROOT_XS_CLIENT_OFFLINE,
            offline_req,
            it_node->first,
            it_node->second.node_index,
            it_node->second.zone_id);
    }
    g_ClientNetNodeMgr.DeleteUser(user_id);
    RETURN_NO_HANDLE;
}