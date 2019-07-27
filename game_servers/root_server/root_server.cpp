/*=======================================================
# Author       : mknight
# Last modified: 2019-06-29 15:05
# Email        : 824338670@qq.com
# Filename     : root_server.cpp
# Description  : 
# =======================================================*/

#include "root_server.h"
#include "server_common/server_config.h"
#include "tr_timer/time_wheel.h"
#include "log_module.h"
#include "net_common.h"
#include "net_socket.h"
#include "cycle_buffer.h"
#include "obj_pool.h"
#include "singleton.h"
#include "net_connection.h"
#include "base_server.h"
#include "net_epoll.h"
#include "log_module.h"
#include "msg_dispatcher.h"
#include "msg_handler.h"
#include "msg_queue.h"
#include "net_message.h"
#include "common_define.h"
#include "server_common/server_session.h"
#include "server_common/server_info_manager.h"
#include "server_common/game_msg_helper.h"
#include "tr_common/common_define.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "protcl_frame.pb.h"
#include "server_common/game_msg_helper.h"

#include <string>

RootServer::RootServer(int32_t index):GameServer(index)
{
    server_type_ = EServerType::E_SERVER_TYPE_ROOT_SERVER;
    node_type_ = EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT;
    wait_other_servers_ = false;
}

RootServer::~RootServer()
{

}

bool RootServer::Init()
{
    if (!RootParentClass::Init())
    {
        return false;
    }
    // 初始化
    node_type_to_wait_num_[EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA] = 1;
    node_type_to_wait_num_[EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER] = 1;
    node_type_to_wait_num_[EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC] = g_ServerConfig.GetJsonConfig()["root_server"]["logic_server_num"].asInt();
    node_type_to_wait_num_[EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE] = g_ServerConfig.GetJsonConfig()["root_server"]["gate_server_num"].asInt();
    return true;
}

// 运行前的检查
bool RootServer::RunStepCheck()
{
    if (!StartLocalListen("root_server"))
    {
        Stop();
        return false;
    }
    
    return true;
}

// 正常运行
bool RootServer::RunStepRunning()
{
    RootParentClass::RunStepRunning();
    return true;
}

void RootServer::OnNewConnectComeIn(TConnection * new_connection)
{
    RootParentClass::OnNewConnectComeIn(new_connection);
}

// 即将运行
bool RootServer::RunStepWillRun()
{
    if (!RootParentClass::RunStepWillRun())
        return false;
    
    // 通知login_server自己的服务器信息
    
    REQMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) req;
    req.mutable_server_node()->set_node_type(int32_t(node_type_));
    req.mutable_server_node()->set_server_type(int32_t(server_type_));
    req.mutable_server_node()->set_server_index(index_);
    req.mutable_server_node()->set_zone_id(g_ServerConfig.GetZoneId());
    // PBMSG_TO_STR(req, msg_str);

    g_MsgHelper.ForwardAsyncPbMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
        INT_FRAMEMSG(E_FRAME_MSG_REGISTER_SERVER_INFO), req,
        [](const NetMessage * rep_msg, const AsyncMsgParam & cb_param){
            REPMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) rep;
            STRING_TO_PBMSG(rep_msg->GetContent(), rep);
            TDEBUG("asyncmsg callback:rep_E_FRAME_MSG_REGISTER_SERVER_INFO:" << rep.ShortDebugString());
        }, AsyncMsgParam(),
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIN,
        0);
    return true;
}

void RootServer::AddWaitStart(EServerRouteNodeType node_type)
{
    node_type_waitstart_num_[node_type] += 1;
    TINFO("AddWaitStart, node_type:" << int32_t(node_type) << ", num:" << node_type_waitstart_num_[node_type]);
}
bool RootServer::CheckAllWaitStart()
{
    bool ret = true;
    for (auto it = node_type_to_wait_num_.begin(); it != node_type_to_wait_num_.end(); ++it)
    {
        TDEBUG("node_type_waitstart_num_[it->first]:" << node_type_waitstart_num_[it->first]);
        if (node_type_waitstart_num_[it->first] < it->second)
        {
            ret = false;
            break;
        }
    }
    return ret;
}

 const std::map<EServerRouteNodeType, int32_t> & RootServer::GetWaitStartNodes()
 {
     return node_type_waitstart_num_;
 }