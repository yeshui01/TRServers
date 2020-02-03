/*=======================================================
# Author       : mknight
# Last modified: 2020-02-03 14:22
# Email        : 824338670@qq.com
# Filename     : logic_cs_frame_handle.cpp
# Description  : 
# =======================================================*/

#include "logic_cs_frame_handle.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "protcl_frame.pb.h"
#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
#include "logic_center_server/logic_center_server.h"
#include "protocol_error_code.h"
#include "server_common/client_net_node.h"
LogicCenterFrameHandler::LogicCenterFrameHandler()
{

}

LogicCenterFrameHandler::~LogicCenterFrameHandler()
{

}

void LogicCenterFrameHandler::BindMsgHandle()
{
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_REGISTER_SERVER_INFO), LogicCenterFrameHandler, OnRegisterServerInfo);
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_ROOT_TO_XS_START_RUN), LogicCenterFrameHandler, OnRecvRootStartCmd);
}

EMsgHandleResult LogicCenterFrameHandler::OnRegisterServerInfo(TConnection *session_pt, const NetMessage * message_pt)
{
    TINFO("OnRegisterServerInfo");
    REQMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) req;
    if (!STRING_TO_PBMSG(message_pt->GetContent(), req))
    {
        REPMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) rep;
        rep.set_isok(-1);
        TERROR("parse pbmsg failed");
        rep.SerializeToString(&rep_content_);
        return EMsgHandleResult::E_MSG_HANDLE_RETURN_CONTENT;
    }
    REPMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) rep;
    rep.set_isok(1);
    
    g_ServerManager.AddServerInfo(dynamic_cast<ServerSession*>(session_pt), 
        EServerRouteNodeType(req.server_node().node_type()), 
        EServerType(req.server_node().server_type()), 
        req.server_node().server_index(),
        req.server_node().zone_id());
    RETURN_REP_CONTENT(rep);
}


EMsgHandleResult LogicCenterFrameHandler::OnRecvRootStartCmd(TConnection *session_pt, const NetMessage * message_pt)
{
    TINFO("OnRecvRootStartCmd");
    LogicCenterServer * server_pt = dynamic_cast<LogicCenterServer*>(session_pt->GetServer());
    if(server_pt)
    {
        server_pt->SetWaitOtherServers(false);
    }
    RETURN_NO_HANDLE;
}
