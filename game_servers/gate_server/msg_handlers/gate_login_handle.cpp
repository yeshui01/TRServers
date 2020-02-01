/*=======================================================
# Author       : mknight
# Last modified: 2019-11-02 12:30
# Email        : 824338670@qq.com
# Filename     : gate_login_handle.cpp
# Description  : 
# =======================================================*/

#include "gate_login_handle.h"

#include "protocol_class.h"
#include "protocol_frame.h"
#include "protocol_login.h"
#include "protcl_login.pb.h"
#include "protcl_frame.pb.h"

#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
#include "gate_server/gate_server.h"
#include "server_common/game_msg_helper.h"
#include "server_common/server_config.h"
#include "protocol_error_code.h"
#include "server_common/client_net_node.h"
#include "gate_server/gate_server.h"

GateLoginHandler::GateLoginHandler()
{

}

GateLoginHandler::~GateLoginHandler()
{

}

void GateLoginHandler::BindMsgHandle()
{
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_C2S_LOGIN), GateLoginHandler, OnClientLogin);
    
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_C2S_CREATE_ROLE), GateLoginHandler, OnCreateRole);
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_C2S_ENTER_GAME), GateLoginHandler, OnGateEnterGame);
}

EMsgHandleResult GateLoginHandler::OnClientLogin(TConnection *session_pt, const NetMessage * message_pt)
{
    TINFO("OnClientLogin");
    REQMSG(E_LOGIN_MSG_C2S_LOGIN) req;
    REPMSG(E_LOGIN_MSG_C2S_LOGIN) rep;
    if (!STRING_TO_PBMSG(message_pt->GetContent(), req))
    {
        rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_PB_PARSE_ERROR));
        TERROR("parse pbmsg failed");
        rep.SerializeToString(&rep_content_);
        return EMsgHandleResult::E_MSG_HANDLE_RETURN_CONTENT;
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_PB_PARSE_ERROR, rep);
    }
    TINFO("req_E_LOGIN_MSG_C2S_LOGIN:" << req.ShortDebugString());
    REQMSG(E_LOGIN_MSG_GG2ROOT_LOGIN) root_req;
    root_req.set_account_name(req.account_name());
    root_req.set_pswd(req.pswd());
    root_req.set_account_id(req.account_id());
    root_req.set_zone_id(g_ServerConfig.GetZoneId());
    
    TR_BEGIN_ASYNC_MSG_WITH_PARAM(E_PROTOCOL_CLASS_LOGIN, E_LOGIN_MSG_GG2ROOT_LOGIN, root_req,accid{req.account_id()})
    {
        REPMSG(E_LOGIN_MSG_C2S_LOGIN) ret;
        ret.set_isok(cb_rep.isok());
        if (cb_rep.isok() == INT_PROTOERR(E_PROTOCOL_ERR_CORRECT))
        {
            // 正常
            ret.mutable_role_list()->CopyFrom(cb_rep.role_list());
            // 设置session 类型
            ServerSession* session_pt = dynamic_cast<ServerSession*>(cb_param.session_pt);
            if (session_pt)
            {
                session_pt->SetChannelType(ESessionChannelType::E_CHANNEL_CLIENT_TO_SERVER);
                session_pt->SetChannelUserId(accid);
                session_pt->AddCloseCallback([accid](ServerSession * session)
                {
                    TINFO("user offline in gate, user_id:" << accid);
                    // 客户端下线
                    REQMSG(E_FRAME_MSG_GG2ROOT_CLIENT_OFFLINE) root_req;
                    root_req.set_user_id(accid);
                    g_MsgHelper.ForwardPbMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
                        INT_FRAMEMSG(E_FRAME_MSG_GG2ROOT_CLIENT_OFFLINE), 
                        root_req,
                        EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT,
                        0);

                });
                TINFO("login sucess,accid:" << accid << ", set session as client2server channel_type" << ", session_id:" << session_pt->GetConnId());
            }
            else
            {
                TERROR("cast to server_session failed, may be loginc error");
            }
        }
        g_MsgHelper.SendAsyncRepMsg(ret, cb_param);
        TINFO("rep async rep_E_LOGIN_MSG_C2S_LOGIN:" << ret.ShortDebugString());
    }
    TR_END_ASYNC_MSG_WITH_PARAM(E_SERVER_ROUTE_NODE_ROOT, 0)

    // g_MsgHelper.ForwardAsyncPbMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_LOGIN),
    //         INT_LOGINMSG(E_LOGIN_MSG_GG2ROOT_LOGIN), root_req,
    //         [](const NetMessage *rep_msg, const AsyncMsgParam &cb_param) {
    //             REPMSG(E_LOGIN_MSG_GG2ROOT_LOGIN) cb_rep;
    //             STRING_TO_PBMSG(rep_msg->GetContent(), cb_rep);
    //             TDEBUG("asyncmsg callback:rep_E_LOGIN_MSG_GG2ROOT_LOGIN:" << cb_rep.ShortDebugString());
                
    //             REPMSG(E_LOGIN_MSG_C2S_LOGIN) ret;
    //             ret.set_isok(cb_rep.isok());
    //             if (cb_rep.isok() == INT_PROTOERR(E_PROTOCOL_ERR_CORRECT))
    //             {
    //                 // 正常
    //                 ret.mutable_role_list()->CopyFrom(cb_rep.role_list());
    //             }
    //             // std::string str;
    //             // ret.SerializeToString(&str);
    //             // auto rep_net_msg = g_MsgTools.MakeNetMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_LOGIN),
    //             //     INT_LOGINMSG(E_LOGIN_MSG_C2S_LOGIN), str);
    //             // int32_t packet_size = rep_net_msg.SerializeByteNum();
    //             // std::vector<char> buffer(packet_size, '\0');
    //             // rep_net_msg.Serialize(buffer.data(), packet_size);
    //             // cb_param.session_pt->Send(buffer.data(), packet_size);
    //             g_MsgHelper.SendAsyncRepMsg(ret, cb_param);
    //             TINFO("rep async rep_E_LOGIN_MSG_C2S_LOGIN:" << ret.ShortDebugString());
    //         }, g_MsgHelper.GenAsyncMsgEnvParam(session_pt, message_pt), 
    //         EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, 0);
    
    RETURN_NO_HANDLE;
}

TR_BEGIN_HANDLE_MSG(GateLoginHandler, OnCreateRole, E_LOGIN_MSG_C2S_CREATE_ROLE)
{
    auto client_session = dynamic_cast<ServerSession*>(session_pt);
    if (!client_session)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_INNER_ERROR, rep);
    }
    if (client_session->GetChannelType() != ESessionChannelType::E_CHANNEL_CLIENT_TO_SERVER)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_INVALID_SESSION, rep);
    }
    if (client_session->GetChannelInfo().user_id == 0)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_ACCOUNT_UNLOGIN, rep);
    }
    if (req.nickname().length() < 5)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_PARAM_ERROR, rep);
    }
    int64_t acc_id = client_session->GetChannelInfo().user_id;
    REQMSG(E_LOGIN_MSG_GG2ROOT_CREATE_ROLE) root_req;
    root_req.set_acc_id(acc_id);
    root_req.set_nickname(req.nickname());
    TR_BEGIN_ASYNC_MSG_WITH_PARAM(E_PROTOCOL_CLASS_LOGIN, E_LOGIN_MSG_GG2ROOT_CREATE_ROLE, root_req)
    {
        REPMSG(E_LOGIN_MSG_C2S_CREATE_ROLE) ret;
        ret.set_isok(cb_rep.isok());
        ret.mutable_snapshot()->CopyFrom(cb_rep.snapshot());
        g_MsgHelper.SendAsyncRepMsg(ret, cb_param);
    }
    TR_END_ASYNC_MSG_WITH_PARAM(E_SERVER_ROUTE_NODE_ROOT, 0);
}
TR_END_HANDLE_MSG_NO_RETURN_MSG


TR_BEGIN_HANDLE_MSG(GateLoginHandler, OnGateEnterGame, E_LOGIN_MSG_C2S_ENTER_GAME)
{
    // TODO:
    auto client_session = dynamic_cast<ServerSession*>(session_pt);
    if (!client_session)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_INNER_ERROR, rep);
    }
    if (client_session->GetChannelType() != ESessionChannelType::E_CHANNEL_CLIENT_TO_SERVER)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_INVALID_SESSION, rep);
    }
    if (client_session->GetChannelInfo().user_id == 0)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_ACCOUNT_UNLOGIN, rep);
    }
    auto client_user = g_ClientNetNodeMgr.HoldClientUser(client_session->GetChannelInfo().user_id);
    if (client_user)
    {
        if (client_user->role_id > 0)
        {
            SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_ROLE_ONLINE, rep);
        }
    }
    int64_t acc_id =  client_session->GetChannelInfo().user_id;
    int64_t role_id = req.role_id();
    REQMSG(E_LOGIN_MSG_GG2CENTER_ENTER_GAME) cs_req;
    cs_req.set_role_id(role_id);
    cs_req.set_acc_id(acc_id);
    TR_BEGIN_ASYNC_MSG_WITH_PARAM(E_PROTOCOL_CLASS_LOGIN, E_LOGIN_MSG_GG2CENTER_ENTER_GAME, cs_req, acc_id,role_id)
    {
        if (INT_PROTOERR(E_PROTOCOL_ERR_CORRECT) == cb_rep.isok())
        {
            // 正常
            g_ClientNetNodeMgr.UpdateUserStatus(acc_id, EUserStatus::E_CLIENT_USER_STATUS_NORMAL);
            g_ClientNetNodeMgr.AttachUserRole(acc_id, role_id);
            g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, 0);
            g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, 0);
            // g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, 0);
            g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, cb_rep.logic_index());

            REQMSG(E_LOGIN_MSG_GATE2LOGIC_ENTER_GAME) logic_req;
            logic_req.set_role_id(role_id);
            logic_req.set_acc_id(acc_id);
            TR_BEGIN_ASYNC_MSG(E_PROTOCOL_CLASS_LOGIN, E_LOGIN_MSG_GATE2LOGIC_ENTER_GAME, logic_req, loigc_rep, async_env{cb_param})
            {
                TDEBUG("cb gate tologic entergame");
                REPMSG(E_LOGIN_MSG_C2S_ENTER_GAME) ret;
                ret.set_isok(loigc_rep.isok());
                ret.mutable_game_data()->CopyFrom(loigc_rep.game_data());
                g_MsgHelper.SendAsyncRepMsg(ret, async_env);
            }
            TR_END_ASYNC_MSG(E_SERVER_ROUTE_NODE_LOGIC, cb_rep.logic_index());
        }
        else
        {
            REPMSG(E_LOGIN_MSG_C2S_ENTER_GAME) ret;
            ret.set_isok(cb_rep.isok());
            g_MsgHelper.SendAsyncRepMsg(ret, cb_param);
        }
    }
    TR_END_ASYNC_MSG_WITH_PARAM(E_SERVER_ROUTE_NODE_CENTER, 0);
}
TR_END_HANDLE_MSG_NO_RETURN_MSG