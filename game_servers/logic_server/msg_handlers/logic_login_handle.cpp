/*=======================================================
# Author       : mknight
# Last modified: 2019-12-15 16:00
# Email        : 824338670@qq.com
# Filename     : logic_login_handle.cpp
# Description  : 
# =======================================================*/

#include "logic_login_handle.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "protocol_login.h"

#include "protcl_frame.pb.h"
#include "protcl_login.pb.h"

#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
#include "logic_server/logic_server.h"
#include "msg_handler.h"
#include "protocol_error_code.h"
#include "logic_server/logic_global.h"
#include "logic_server/logic_player.h"
#include "logic_server/func_wrappers/logic_login_wrapper.h"
#include "tr_timer/server_time.h"
#include "server_common/server_config.h"
#include "logic_server/logic_table_service.h"
#include "server_common/client_net_node.h"
#include "server_common/game_msg_helper.h"

LogicLoginHandler::LogicLoginHandler()
{

}

LogicLoginHandler::~LogicLoginHandler()
{

}

// 绑定消息处理接口
void LogicLoginHandler::BindMsgHandle()
{
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_ROOT2LOGIC_CREATE_ROLE), LogicLoginHandler, OnLogicCreateRole);
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_ROOT2LOGIC_ENTER_GAME), LogicLoginHandler, OnLogicStartEnterGame);
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_GATE2LOGIC_ENTER_GAME), LogicLoginHandler, OnClientEnterGame);
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_C2S_TEST_ROLE_MSG), LogicLoginHandler, OnRoleTestMsg);
}

TR_BEGIN_HANDLE_MSG(LogicLoginHandler, OnLogicCreateRole, E_LOGIN_MSG_ROOT2LOGIC_CREATE_ROLE)
{
    int64_t role_id = req.role_id();
    int64_t acc_id = req.acc_id();
    const std::string & nickname = req.nickname();
    LogicPlayer * logic_player = g_LogicGlobal.GetLogicPlayer(role_id);
    if (!logic_player)
    {
        auto tbs_pt = g_LogicTableService.CreatePlayerTables(role_id);
        if (tbs_pt)
        {
            logic_player = new LogicPlayer(tbs_pt);
            LcLoginWrapper login_wp(logic_player);
            auto &role_base = logic_player->data_tbs_->role_base.HoldData();
            role_base.SetRoleId(role_id);
            role_base.SetAccId(acc_id);
            role_base.SetCreateTime(g_ServerTime.NowTime());
            role_base.SetNickname(nickname);
            role_base.SetZoneId(g_ServerConfig.GetZoneId());
            role_base.SetInited(1);

            // 初始化业务数据
            login_wp.InitCreateData();

            auto pb_snapshot = rep.mutable_snapshot();
            pb_snapshot->set_role_id(role_id);
            pb_snapshot->set_nickname(nickname);
        }
    }
    else
    {
        TERROR("create role failed, logic player existed, may be logic error");
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_INNER_ERROR, rep);
    }
    
}
TR_END_HANDLE_MSG_AND_RETURN_MSG


TR_BEGIN_HANDLE_MSG(LogicLoginHandler, OnLogicStartEnterGame, E_LOGIN_MSG_ROOT2LOGIC_ENTER_GAME)
{
    int64_t role_id = req.role_id();
    int64_t acc_id = req.acc_id();
    int32_t gate_index = req.gate_index();
    auto player_tbs = g_LogicTableService.HoldPlayerTables(role_id);
    if (player_tbs)
    {
        LogicPlayer *p = g_LogicGlobal.GetLogicPlayer(role_id);
        if (!p)
        {
            p = new LogicPlayer(player_tbs);
            g_LogicGlobal.AddLogicPlayer(role_id, p);
        }
        g_ClientNetNodeMgr.UpdateUserStatus(acc_id, EUserStatus::E_CLIENT_USER_STATUS_NORMAL);
        g_ClientNetNodeMgr.AttachUserRole(acc_id, role_id);
        g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, gate_index);
        g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, 0);
        // g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, logic_index);
        g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, 0);
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_CORRECT, rep);
    }
    else
    {
        REQMSG(E_FRAME_MSG_XS_TO_DATA_LOAD_PLAYER_TABLES)
        data_req;
        data_req.set_role_id(role_id);
        TR_BEGIN_ASYNC_MSG_WITH_PARAM(E_PROTOCOL_CLASS_FRAME, E_FRAME_MSG_XS_TO_DATA_LOAD_PLAYER_TABLES, data_req, role_id, acc_id, gate_index)
        {
            REPMSG(E_LOGIN_MSG_ROOT2LOGIC_ENTER_GAME) ret;
            ret.set_isok(cb_rep.isok());
            
            g_MsgHelper.SendAsyncRepMsg(ret, cb_param);
            if (cb_rep.isok() != INT_PROTOERR(E_PROTOCOL_ERR_CORRECT))
            {
                return;
            }

            TINFO("load player from db success, finish enter logic_server, role_id:" << role_id << ", gate_index:" << gate_index);
            auto player_tbs = g_LogicTableService.CreatePlayerTables(role_id);
            g_LogicTableService.LoadPlayerTableFromPbMsg(*player_tbs, &cb_rep);
            auto p = g_LogicGlobal.GetLogicPlayer(role_id);
            if (!p)
            {
                p = new LogicPlayer(player_tbs);
                g_LogicGlobal.AddLogicPlayer(role_id, p);
            }

            g_ClientNetNodeMgr.UpdateUserStatus(acc_id, EUserStatus::E_CLIENT_USER_STATUS_NORMAL);
            g_ClientNetNodeMgr.AttachUserRole(acc_id, role_id);
            g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, gate_index);
            g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, 0);
            // g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, logic_index);
            g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, 0);
            g_MsgHelper.SendAsyncRepMsg(ret, cb_param);
        }
        TR_END_ASYNC_MSG_WITH_PARAM(E_SERVER_ROUTE_NODE_DATA, 0)
    }
    
}
TR_END_HANDLE_MSG_AND_RETURN_MSG

TR_BEGIN_HANDLE_MSG(LogicLoginHandler, OnClientEnterGame, E_LOGIN_MSG_GATE2LOGIC_ENTER_GAME)
{
    int64_t role_id = req.role_id();
    
    auto p = g_LogicGlobal.GetLogicPlayer(role_id);
    if (!p)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_NOT_FOUND_PLAYER, rep);
    }
    // TODO: 
    // tmp test code
    auto pb_game_data = rep.mutable_game_data();
    if (pb_game_data)
    {
        auto & role_base = p->data_tbs_->role_base.GetData();
        pb_game_data->set_role_id(role_base.GetRoleId());
        pb_game_data->set_nickname(role_base.GetNickname());
    }
}
TR_END_HANDLE_MSG_AND_RETURN_MSG


TR_BEGIN_HANDLE_MSG(LogicLoginHandler,OnRoleTestMsg, E_LOGIN_MSG_C2S_TEST_ROLE_MSG)
{
    int64_t role_id = message_pt->GetParam();
    
    auto p = g_LogicGlobal.GetLogicPlayer(role_id);
    if (!p)
    {
        TERROR("not foun player, role_id:" << role_id);
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_NOT_FOUND_PLAYER, rep);
    }
    auto user_info = g_ClientNetNodeMgr.GetRoleUser(role_id);
    if (!user_info)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_ACCOUNT_UNLOGIN, rep);
    }
    if (user_info->user_id != p->data_tbs_->role_base.GetData().GetAccId())
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_ACCOUNT_ROLE_NOT_MATCH, rep);
    }

    rep.set_echo_content(req.echo_content());
}
TR_END_HANDLE_MSG_AND_RETURN_MSG