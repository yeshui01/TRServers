/*=======================================================
# Author       : mknight
# Last modified: 2019-11-02 15:29
# Email        : 824338670@qq.com
# Filename     : root_login_handle.cpp
# Description  : 
# =======================================================*/

#include "root_login_handle.h"

#include "protocol_class.h"
#include "protocol_login.h"
#include "protcl_login.pb.h"

#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
#include "server_common/game_msg_helper.h"
#include "server_common/server_config.h"
#include "protocol_error_code.h"
#include "server_common/client_net_node.h"
#include "root_server/root_global.h"
RootLoginHandler::RootLoginHandler()
{

}

RootLoginHandler::~RootLoginHandler()
{

}

void RootLoginHandler::BindMsgHandle()
{
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_GG2ROOT_LOGIN), RootLoginHandler, OnGateClientLogin);
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_GG2ROOT_CREATE_ROLE), RootLoginHandler, OnGateCreateRole);
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_CENTER2ROOT_ENTER_GAME), RootLoginHandler, OnRootEnterGame);
}

EMsgHandleResult RootLoginHandler::OnGateClientLogin(TConnection *session_pt, const NetMessage * message_pt)
{
    TINFO("OnClientLogin");
    REQMSG(E_LOGIN_MSG_GG2ROOT_LOGIN) req;
    REPMSG(E_LOGIN_MSG_GG2ROOT_LOGIN) rep;
    if (!STRING_TO_PBMSG(message_pt->GetContent(), req))
    {
        TERROR("parse pbmsg failed");
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_PB_PARSE_ERROR, rep);
    }
    TINFO("req_E_LOGIN_MSG_GG2ROOT_LOGIN:" << req.ShortDebugString());
    // 账号登录状态判断,不能同时登录两个
    auto cur_usr_satus = g_ClientNetNodeMgr.GetUserStatus(req.account_id());
    if (cur_usr_satus != EUserStatus::E_CLIENT_USER_STATUS_NONE)
    {
        TERROR("cant login from user status = " << static_cast<int32_t>(cur_usr_satus) << ", user_id:" << req.account_id());
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_ACCOUNT_LOGINGED, rep);
    }
    g_ClientNetNodeMgr.UpdateUserStatus(req.account_id(), EUserStatus::E_CLIENT_USER_STATUS_LOGIN_ING);
    // 登录状态正常,校验一下密码
    REQMSG(E_LOGIN_MSG_ROOT2LOGIN_ACCT_CHECK) login_req;
    login_req.set_account_name(req.account_name());
    login_req.set_pswd(req.pswd());
    login_req.set_account_id(req.account_id());
    // login_req.set_zone_id(g_ServerConfig.GetZoneId());
    TR_BEGIN_ASYNC_MSG_WITH_PARAM(E_PROTOCOL_CLASS_LOGIN, E_LOGIN_MSG_ROOT2LOGIN_ACCT_CHECK, login_req,accid{req.account_id()})
    {
        REPMSG(E_LOGIN_MSG_GG2ROOT_LOGIN) ret;
        ret.set_isok(cb_rep.isok());
        if (cb_rep.isok() == INT_PROTOERR(E_PROTOCOL_ERR_CORRECT))
        {
            // 正常
            g_ClientNetNodeMgr.UpdateUserStatus(accid, EUserStatus::E_CLIENT_USER_STATUS_LOGINED);
            // 设置gate节点
            ServerSession* session_pt = dynamic_cast<ServerSession*>(cb_param.session_pt);
            if (session_pt)
            {
                g_ClientNetNodeMgr.UpdateUserNode(accid, session_pt->GetChannelInfo().node_type, session_pt->GetChannelInfo().server_index);
                // 通知网关?不用，网关自己根据回调处理
            }
            else
            {
                TERROR("cast to server_session failed, may be loginc error");
                ret.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_INNER_ERROR));
                g_ClientNetNodeMgr.DeleteUser(accid);
                // g_MsgHelper.SendAsyncRepMsg(ret, cb_param);
                TINFO("rep async rep_E_LOGIN_MSG_GG2ROOT_LOGIN:" << ret.ShortDebugString());
                return;
            }
        }
        else
        {
            g_ClientNetNodeMgr.DeleteUser(accid);
            // g_MsgHelper.SendAsyncRepMsg(ret, cb_param);
            TINFO("rep async rep_E_LOGIN_MSG_GG2ROOT_LOGIN:" << ret.ShortDebugString());
            return;
        }
        // 获取角色列表
        REQMSG(E_LOGIN_MSG_ROOT2DATA_FETCH_ROLE_SNAPSHOT) data_req;
        data_req.set_acc_id(accid);
        TR_BEGIN_ASYNC_MSG(E_PROTOCOL_CLASS_LOGIN, E_LOGIN_MSG_ROOT2DATA_FETCH_ROLE_SNAPSHOT, data_req, data_rep,async_env(cb_param))
        {
            REPMSG(E_LOGIN_MSG_GG2ROOT_LOGIN) ret;
            ret.set_isok(data_rep.isok());
            ret.mutable_role_list()->CopyFrom(data_rep.role_list());
            g_MsgHelper.SendAsyncRepMsg(ret, async_env);
        }
        TR_END_ASYNC_MSG(E_SERVER_ROUTE_NODE_DATA, 0)
        // g_MsgHelper.SendAsyncRepMsg(ret, cb_param);
        TINFO("rep async rep_E_LOGIN_MSG_GG2ROOT_LOGIN:" << ret.ShortDebugString());
    }
    TR_END_ASYNC_MSG_WITH_PARAM(E_SERVER_ROUTE_NODE_LOGIN, 0)

    RETURN_NO_HANDLE;
}

TR_BEGIN_HANDLE_MSG(RootLoginHandler, OnGateCreateRole, E_LOGIN_MSG_GG2ROOT_CREATE_ROLE)
{
    int64_t acc_id = req.acc_id();
    auto user_status = g_ClientNetNodeMgr.GetUserStatus(acc_id);
    if (EUserStatus::E_CLIENT_USER_STATUS_LOGINED != user_status)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_ACCOUNT_STATUS, rep);
    }
    REQMSG(E_LOGIN_MSG_ROOT2LOGIN_CREATE_ROLE) login_req;
    login_req.set_acc_id(acc_id);
    login_req.set_nickname(req.nickname());
    TR_BEGIN_ASYNC_MSG_WITH_PARAM(E_PROTOCOL_CLASS_LOGIN, E_LOGIN_MSG_ROOT2LOGIN_CREATE_ROLE, login_req,acc_id)
    {
        if (cb_rep.isok() != INT_PROTOERR(E_PROTOCOL_ERR_CORRECT))
        {
            TWARN("step 1 create role failed, acc_id:" << acc_id);
            REPMSG(E_LOGIN_MSG_GG2ROOT_CREATE_ROLE) ret;
            ret.set_isok(cb_rep.isok());
            g_MsgHelper.SendAsyncRepMsg(ret, cb_param);
            return;
        }
        
        REQMSG(E_LOGIN_MSG_ROOT2DATA_CREATE_ROLE) data_req;
        data_req.set_role_id(cb_rep.role_id());
        data_req.set_acc_id(cb_rep.acc_id());
        data_req.set_nickname(cb_rep.nickname());
        TINFO("data_req:" << data_req.ShortDebugString());
        TR_BEGIN_ASYNC_MSG(E_PROTOCOL_CLASS_LOGIN, E_LOGIN_MSG_ROOT2DATA_CREATE_ROLE, data_req, data_rep, \
            async_env{cb_param},acc_id,role_id{cb_rep.role_id()}, nickname(cb_rep.nickname()))
        {
            if (data_rep.isok() != INT_PROTOERR(E_PROTOCOL_ERR_CORRECT))
            {
                TWARN("step 2 create role failed, acc_id:" << acc_id);
                REPMSG(E_LOGIN_MSG_GG2ROOT_CREATE_ROLE) ret;
                ret.set_isok(data_rep.isok());
                g_MsgHelper.SendAsyncRepMsg(ret, async_env);
                return;
            }
            // to logic server init role data
            // 动态分配一个logic
            int32_t logic_index = g_RootGlobal.AlocateLogicServerIndex(role_id);
            REQMSG(E_LOGIN_MSG_ROOT2LOGIC_CREATE_ROLE) logic_req;
            logic_req.set_role_id(role_id);
            logic_req.set_acc_id(acc_id);
            logic_req.set_nickname(nickname);
            TR_BEGIN_ASYNC_MSG(E_PROTOCOL_CLASS_LOGIN, E_LOGIN_MSG_ROOT2LOGIC_CREATE_ROLE, logic_req, logic_rep, async_env)
            {
                TINFO("create role finish");
                REPMSG(E_LOGIN_MSG_GG2ROOT_CREATE_ROLE) ret;
                ret.set_isok(logic_rep.isok());
                ret.mutable_snapshot()->CopyFrom(logic_rep.snapshot());
                g_MsgHelper.SendAsyncRepMsg(ret, async_env);
            }
            TR_END_ASYNC_MSG(E_SERVER_ROUTE_NODE_LOGIC, logic_index)
        }
        TR_END_ASYNC_MSG(E_SERVER_ROUTE_NODE_DATA, 0)
    }
    TR_END_ASYNC_MSG_WITH_PARAM(E_SERVER_ROUTE_NODE_LOGIN, 0)
}
TR_END_HANDLE_MSG_NO_RETURN_MSG

TR_BEGIN_HANDLE_MSG(RootLoginHandler, OnRootEnterGame, E_LOGIN_MSG_CENTER2ROOT_ENTER_GAME)
{
    int64_t role_id = req.role_id();
    int64_t acc_id = req.acc_id();
    int32_t gate_index = req.gate_index();
    auto user_status = g_ClientNetNodeMgr.GetUserStatus(acc_id);
    if (EUserStatus::E_CLIENT_USER_STATUS_LOGINED != user_status)
    {
        TERROR("account status not match, acc_id:" << acc_id << ", status:" << int32_t(user_status));
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_ACCOUNT_STATUS, rep);
    }

    int32_t logic_index = g_RootGlobal.AlocateLogicServerIndex(role_id);
    TINFO("role alocate logic index, roleid:" << role_id << ", logic_index:" << logic_index);
    REQMSG(E_LOGIN_MSG_ROOT2LOGIC_ENTER_GAME) logic_req;
    logic_req.set_role_id(role_id);
    logic_req.set_acc_id(acc_id);
    logic_req.set_gate_index(gate_index);
    TR_BEGIN_ASYNC_MSG_WITH_PARAM(E_PROTOCOL_CLASS_LOGIN, E_LOGIN_MSG_ROOT2LOGIC_ENTER_GAME, logic_req, role_id,logic_index,acc_id, gate_index)
    {
        REPMSG(E_LOGIN_MSG_CENTER2ROOT_ENTER_GAME) ret;
        ret.set_isok(cb_rep.isok());
        ret.set_logic_index(logic_index);
        g_MsgHelper.SendAsyncRepMsg(ret, cb_param);
        if (cb_rep.isok() != INT_PROTOERR(E_PROTOCOL_ERR_CORRECT))
        {
            return;
        }
        TINFO("role enter game, finish enter logic_server, role_id:" << role_id 
            << ", logic_index:" << logic_index 
            << ", gate_index:" << gate_index);

        g_ClientNetNodeMgr.UpdateUserStatus(acc_id, EUserStatus::E_CLIENT_USER_STATUS_NORMAL);
        g_ClientNetNodeMgr.AttachUserRole(acc_id, role_id);
        g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, gate_index);
        g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, 0);
        g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, logic_index);
        g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, 0);
        g_MsgHelper.SendAsyncRepMsg(ret, cb_param);
    }
    TR_END_ASYNC_MSG_WITH_PARAM(E_SERVER_ROUTE_NODE_LOGIC, logic_index)
}
TR_END_HANDLE_MSG_NO_RETURN_MSG