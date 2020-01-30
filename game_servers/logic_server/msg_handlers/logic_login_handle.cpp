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