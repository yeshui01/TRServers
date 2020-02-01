/*=======================================================
# Author       : mknight
# Last modified: 2020-01-31 11:10
# Email        : 824338670@qq.com
# Filename     : center_login_handle.cpp
# Description  : 
# =======================================================*/
#include "center_login_handle.h"

#include "protocol_class.h"
#include "protocol_frame.h"
#include "protocol_login.h"

#include "protcl_frame.pb.h"
#include "protcl_login.pb.h"

#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
#include "center_server/center_server.h"
#include "msg_handler.h"
#include "protocol_error_code.h"
#include "center_server/center_global.h"
#include "center_server/center_player.h"
#include "tr_timer/server_time.h"
#include "server_common/server_config.h"
#include "center_server/center_table_service.h"
#include "server_common/client_net_node.h"
#include "server_common/game_msg_helper.h"

CenterLoginHandler::CenterLoginHandler()
{

}

CenterLoginHandler::~CenterLoginHandler()
{

}

// 绑定消息处理接口
void CenterLoginHandler::BindMsgHandle()
{
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_GG2CENTER_ENTER_GAME), CenterLoginHandler, OnCSEnterGame);
}

TR_BEGIN_HANDLE_MSG(CenterLoginHandler, OnCSEnterGame, E_LOGIN_MSG_GG2CENTER_ENTER_GAME)
{
    int64_t role_id = req.role_id();
    int64_t acc_id = req.acc_id();
    {
        auto center_p = g_CenterGlobal.GetCenterPlayer(role_id);
        if (center_p)
        {
            // 旧的角色,校验TODO;
        }
    }
    
    auto gate_session = dynamic_cast<ServerSession*>(session_pt);
    if (!gate_session)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_INNER_ERROR, rep);
    }
    
    int32_t gate_index = gate_session->GetChannelInfo().server_index;
    REQMSG(E_LOGIN_MSG_CENTER2ROOT_ENTER_GAME) root_req;
    root_req.set_acc_id(acc_id);
    root_req.set_role_id(role_id);
    root_req.set_gate_index(gate_index);
    TINFO("req_E_LOGIN_MSG_CENTER2ROOT_ENTER_GAME:" << root_req.gate_index());
    TR_BEGIN_ASYNC_MSG_WITH_PARAM(E_PROTOCOL_CLASS_LOGIN, E_LOGIN_MSG_CENTER2ROOT_ENTER_GAME, root_req,acc_id, role_id, gate_index)
    {
        REPMSG(E_LOGIN_MSG_GG2CENTER_ENTER_GAME) ret;
        ret.set_isok(cb_rep.isok());
        ret.set_logic_index(cb_rep.logic_index());
        g_MsgHelper.SendAsyncRepMsg(ret, cb_param);
        if (cb_rep.isok() != INT_PROTOERR(E_PROTOCOL_ERR_CORRECT))
        {
            return;
        }
        g_ClientNetNodeMgr.UpdateUserStatus(acc_id, EUserStatus::E_CLIENT_USER_STATUS_NORMAL);
        g_ClientNetNodeMgr.AttachUserRole(acc_id, role_id);
        g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, gate_index);
        g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, 0);
        g_ClientNetNodeMgr.UpdateUserNode(acc_id, EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, cb_rep.logic_index());
    }
    TR_END_ASYNC_MSG_WITH_PARAM(E_SERVER_ROUTE_NODE_ROOT, 0)
}
TR_END_HANDLE_MSG_NO_RETURN_MSG