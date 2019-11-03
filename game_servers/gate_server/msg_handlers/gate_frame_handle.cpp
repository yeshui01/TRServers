/*=======================================================
# Author       : mknight
# Last modified: 2019-07-20 14:35
# Email        : 824338670@qq.com
# Filename     : gate_frame_handle.cpp
# Description  : 
# =======================================================*/


#include "gate_frame_handle.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "protcl_frame.pb.h"
#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
#include "gate_server/gate_server.h"

GateFrameHandler::GateFrameHandler()
{

}

GateFrameHandler::~GateFrameHandler()
{

}

void GateFrameHandler::BindMsgHandle()
{
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_REGISTER_SERVER_INFO), GateFrameHandler, OnRegisterServerInfo);
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_ROOT_TO_XS_START_RUN), GateFrameHandler, OnRecvRootStartCmd);
}

EMsgHandleResult GateFrameHandler::OnRegisterServerInfo(TConnection *session_pt, const NetMessage * messag_pt)
{
    TINFO("OnRegisterServerInfo");
    REQMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) req;
    if (!STRING_TO_PBMSG(messag_pt->GetContent(), req))
    {
        REPMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) rep;
        rep.set_isok(-1);
        TERROR("parse pbmsg failed");
        rep.SerializeToString(&rep_content_);
        return EMsgHandleResult::E_MSG_HANDLE_RETURN_CONTENT;
    }
    // TODO:
    REPMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) rep;
    rep.set_isok(1);
    
    g_ServerManager.AddServerInfo(dynamic_cast<ServerSession*>(session_pt), 
        EServerRouteNodeType(req.server_node().node_type()), 
        EServerType(req.server_node().server_type()), 
        req.server_node().server_index(),
        req.server_node().zone_id());
    RETURN_REP_CONTENT(rep);
}


EMsgHandleResult GateFrameHandler::OnRecvRootStartCmd(TConnection *session_pt, const NetMessage * messag_pt)
{
    TINFO("OnRecvRootStartCmd");
    GateServer * server_pt = dynamic_cast<GateServer*>(session_pt->GetServer());
    if(server_pt)
    {
        server_pt->SetWaitOtherServers(false);
    }
    RETURN_NO_HANDLE;
}