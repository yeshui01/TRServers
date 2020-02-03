/*=======================================================
# Author       : mknight
# Last modified: 2020-02-03 15:36
# Email        : 824338670@qq.com
# Filename     : view_manager_frame_handle.cpp
# Description  : 
# =======================================================*/

#include "view_manager_frame_handle.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "protcl_frame.pb.h"
#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
#include "view_manager_server/view_manager_server.h"
#include "protocol_error_code.h"
#include "server_common/client_net_node.h"
ViewManagerFrameHandler::ViewManagerFrameHandler()
{

}

ViewManagerFrameHandler::~ViewManagerFrameHandler()
{

}

void ViewManagerFrameHandler::BindMsgHandle()
{
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_REGISTER_SERVER_INFO), ViewManagerFrameHandler, OnRegisterServerInfo);
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_ROOT_TO_XS_START_RUN), ViewManagerFrameHandler, OnRecvRootStartCmd);
}

EMsgHandleResult ViewManagerFrameHandler::OnRegisterServerInfo(TConnection *session_pt, const NetMessage * message_pt)
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


EMsgHandleResult ViewManagerFrameHandler::OnRecvRootStartCmd(TConnection *session_pt, const NetMessage * message_pt)
{
    TINFO("OnRecvRootStartCmd");
    ViewManagerServer * server_pt = dynamic_cast<ViewManagerServer*>(session_pt->GetServer());
    if(server_pt)
    {
        server_pt->SetWaitOtherServers(false);
    }
    RETURN_NO_HANDLE;
}
