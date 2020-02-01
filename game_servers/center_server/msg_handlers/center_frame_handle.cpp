/*=======================================================
# Author       : mknight
# Last modified: 2019-07-20 14:34
# Email        : 824338670@qq.com
# Filename     : center_frame_handle.cpp
# Description  : 
# =======================================================*/


#include "center_frame_handle.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "protcl_frame.pb.h"
#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
#include "center_server/center_server.h"
#include "protocol_error_code.h"
#include "server_common/client_net_node.h"

CenterFrameHandler::CenterFrameHandler()
{

}

CenterFrameHandler::~CenterFrameHandler()
{

}

void CenterFrameHandler::BindMsgHandle()
{
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_REGISTER_SERVER_INFO), CenterFrameHandler, OnRegisterServerInfo);
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_ROOT_TO_XS_START_RUN), CenterFrameHandler, OnRecvRootStartCmd);
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_ROOT_XS_CLIENT_OFFLINE), CenterFrameHandler, OnCenterUserOffline);
}

EMsgHandleResult CenterFrameHandler::OnRegisterServerInfo(TConnection *session_pt, const NetMessage * message_pt)
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


EMsgHandleResult CenterFrameHandler::OnRecvRootStartCmd(TConnection *session_pt, const NetMessage * message_pt)
{
    TINFO("OnRecvRootStartCmd");
    CenterServer * server_pt = dynamic_cast<CenterServer*>(session_pt->GetServer());
    if(server_pt)
    {
        server_pt->SetWaitOtherServers(false);
    }
    RETURN_NO_HANDLE;
}


TR_BEGIN_HANDLE_MSG(CenterFrameHandler, OnCenterUserOffline, E_FRAME_MSG_ROOT_XS_CLIENT_OFFLINE)
{
    auto user_id = req.user_id();
    {
        auto user_info  = g_ClientNetNodeMgr.HoldClientUser(user_id);
        if (user_info)
        {
            // role 下线TODO:
            
        }
    }


    g_ClientNetNodeMgr.DeleteUser(user_id);
}
TR_END_HANDLE_MSG_NO_RETURN_MSG