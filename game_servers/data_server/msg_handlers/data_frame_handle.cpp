/*=======================================================
# Author       : mknight
# Last modified: 2019-07-20 14:33
# Email        : 824338670@qq.com
# Filename     : data_frame_handle.cpp
# Description  : 
# =======================================================*/

#include "data_frame_handle.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "protcl_frame.pb.h"
#include "protocol_error_code.h"
#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
#include "data_server/data_server.h"
#include "data_server/data_global.h"
#include "data_server/data_player.h"
#include "data_server/data_table_service.h"

DataFrameHandler::DataFrameHandler()
{

}

DataFrameHandler::~DataFrameHandler()
{

}

void DataFrameHandler::BindMsgHandle()
{
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_REGISTER_SERVER_INFO), DataFrameHandler, OnRegisterServerInfo);
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_ROOT_TO_XS_START_RUN), DataFrameHandler, OnRecvRootStartCmd);
    MSG_BIND_HANDLER(INT_FRAMEMSG(E_FRAME_MSG_XS_TO_DATA_SAVE_PLAYER_TABLES), DataFrameHandler, OnSaveDataPlayer);
}

EMsgHandleResult DataFrameHandler::OnRegisterServerInfo(TConnection *session_pt, const NetMessage * message_pt)
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

EMsgHandleResult DataFrameHandler::OnRecvRootStartCmd(TConnection *session_pt, const NetMessage * message_pt)
{
    TINFO("OnRecvRootStartCmd");
    DataServer * server_pt = dynamic_cast<DataServer*>(session_pt->GetServer());
    if(server_pt)
    {
        server_pt->SetWaitOtherServers(false);
    }
    RETURN_NO_HANDLE;
}

TR_BEGIN_HANDLE_MSG(DataFrameHandler, OnSaveDataPlayer, E_FRAME_MSG_XS_TO_DATA_SAVE_PLAYER_TABLES)
{
    int64_t role_id = req.role_id();
    auto p = g_DataGlobal.GetDataPlayer(role_id);
    if (!p)
    {
        TERROR("save dataplayer faile, not found data_player, role_id:" << role_id);
        RETURN_NO_HANDLE
    }
    for (int32_t i = 0; i < req.table_list_size(); ++i)
    {
        g_DataTableService.UpdatePlayerTablesFromPbTables(p, req.mutable_table_list(i));
    }
    g_DataTableService.SaveDataPlayer(p);
}
TR_END_HANDLE_MSG_NO_RETURN_MSG