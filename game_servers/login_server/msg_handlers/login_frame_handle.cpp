/*=======================================================
# Author       : mknight
# Last modified: 2019-07-13 15:42
# Email        : 824338670@qq.com
# Filename     : login_frame_handle.cpp
# Description  : 
# =======================================================*/

#include "login_frame_handle.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "protcl_frame.pb.h"
#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
LoginFrameHandler::LoginFrameHandler()
{

}

LoginFrameHandler::~LoginFrameHandler()
{

}

void LoginFrameHandler::BindMsgHandle()
{
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_REGISTER_SERVER_INFO), LoginFrameHandler, OnRegisterServerInfo);
}

EMsgHandleResult LoginFrameHandler::OnRegisterServerInfo(TConnection *session_pt, const NetMessage * messag_pt)
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
    
    REPMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) rep;
    rep.set_isok(1);
    
    g_ServerManager.AddServerInfo(dynamic_cast<ServerSession*>(session_pt), 
        EServerRouteNodeType(req.server_node().node_type()), 
        EServerType(req.server_node().server_type()), 
        req.server_node().server_index(), 
        req.server_node().zone_id());
    RETURN_REP_CONTENT(rep);
}