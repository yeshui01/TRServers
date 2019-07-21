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
}

EMsgHandleResult RootFrameHandler::OnRegisterServerInfo(TConnection *session_pt, const NetMessage * messag_pt)
{
    TINFO("OnRegisterServerInfo");
    REQMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) req;
    if (!STRING_TO_PBMSG(messag_pt->GetContent(), req))
    {
        REPMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) rep;
        rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_PB_PARSE_ERROR));
        TERROR("parse pbmsg failed");
        rep.SerializeToString(&rep_content_);
        return EMsgHandleResult::E_MSG_HANDLE_RETURN_CONTENT;
    }
    TINFO("req:" << req.ShortDebugString());
    REPMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) rep;
    rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_CORRECT));
    
    g_ServerManager.AddServerInfo(dynamic_cast<ServerSession*>(session_pt), EServerRouteNodeType(req.server_node().node_type()), 
        EServerType(req.server_node().server_type()), 
        req.server_node().server_index(),
        req.server_node().zone_id());
    RETURN_REP_CONTENT(rep);
}

EMsgHandleResult RootFrameHandler::OnQueryServerNodeList(TConnection *session_pt, const NetMessage * messag_pt)
{
    TINFO("OnQueryServerNodeList");
    REQMSG(E_FRAME_MSG_QUERY_SERVER_NODE_LIST) req;
    if (!STRING_TO_PBMSG(messag_pt->GetContent(), req))
    {
        REPMSG(E_FRAME_MSG_QUERY_SERVER_NODE_LIST) rep;
        rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_PB_PARSE_ERROR));
        TERROR("parse pbmsg failed");
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

EMsgHandleResult RootFrameHandler::OnServerWaitOtherStart(TConnection *session_pt, const NetMessage * messag_pt)
{
    TINFO("OnServerWaitOtherStart");
    REQMSG(E_FRAME_MSG_XS_TO_ROOT_WAIT_OTHERS) req;
    if (!STRING_TO_PBMSG(messag_pt->GetContent(), req))
    {
        REPMSG(E_FRAME_MSG_XS_TO_ROOT_WAIT_OTHERS) rep;
        rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_PB_PARSE_ERROR));
        TERROR("parse pbmsg failed");
        rep.SerializeToString(&rep_content_);
        return EMsgHandleResult::E_MSG_HANDLE_RETURN_CONTENT;
    }
    
    REPMSG(E_FRAME_MSG_XS_TO_ROOT_WAIT_OTHERS) rep;
    rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_CORRECT));
    RootServer * server_pt = dynamic_cast<RootServer*>(session_pt->GetServer());
    if (server_pt)
    {
        server_pt->AddWaitStart(EServerRouteNodeType(req.node_type()));
        if (server_pt->ChcekAllWaitStart())
        {
            REQMSG(E_FRAME_MSG_ROOT_TO_XS_START_RUN) push;
            // TODO:通知其他服务器,可以开始运行了
            // test
            PBMSG_TO_STR(push, str);
            g_MsgHelper.ForwardMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
             INT_FRAMEMSG(E_FRAME_MSG_ROOT_TO_XS_START_RUN),
             str, EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA,
             0);

            g_MsgHelper.ForwardMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
             INT_FRAMEMSG(E_FRAME_MSG_ROOT_TO_XS_START_RUN),
             str, EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER,
             0);

            g_MsgHelper.ForwardMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
             INT_FRAMEMSG(E_FRAME_MSG_ROOT_TO_XS_START_RUN),
             str, EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC,
             0);
            g_MsgHelper.ForwardMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
             INT_FRAMEMSG(E_FRAME_MSG_ROOT_TO_XS_START_RUN),
             str, EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC,
             1);

            g_MsgHelper.ForwardMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
            INT_FRAMEMSG(E_FRAME_MSG_ROOT_TO_XS_START_RUN),
            str, EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE,
            0);
            g_MsgHelper.ForwardMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
            INT_FRAMEMSG(E_FRAME_MSG_ROOT_TO_XS_START_RUN),
            str, EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE,
            1);

            TINFO("tell other server to start");
        }
    }
    else
    {
        TERROR("server is nullptr");
    }
    
    RETURN_REP_CONTENT(rep);
}