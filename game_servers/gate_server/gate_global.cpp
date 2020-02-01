/*=======================================================
# Author       : mknight
# Last modified: 2020-01-31 17:03
# Email        : 824338670@qq.com
# Filename     : gate_global.cpp
# Description  : 
# =======================================================*/
#include "gate_global.h"
#include "tr_log/log_module.h"
#include "tr_common/common_define.h"
#include "net_message.h"
#include "server_common/game_msg_helper.h"
#include "server_common/server_config.h"
#include "server_common/client_net_node.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "protocol_login.h"
#include "server_common/server_session.h"
#include "tr_msg_packet/msg_tools.h"
#include "tr_msg_packet/msg_dispatcher.h"
GateGlobal::GateGlobal()
{

}

GateGlobal::~GateGlobal()
{

}

bool GateGlobal::Init()
{
    g_MsgDispatcher.RegDispatchHook([](NetMessage * net_message)->bool
    {
        return g_GateGlobal.ForwardClientMsg(net_message);
    });
    return true;
}

void GateGlobal::Stop()
{

}

bool GateGlobal::ForwardClientMsg(NetMessage * msg_pt)
{
    if (!msg_pt)
    {
        return false;
    }
    
    // 客户端消息转发表 test_code,key:msg_class * 100000 + msg_type
    static int32_t key_factor = 100000;
    static std::function<int64_t (int32_t msg_class, int32_t msg_type)> key_gen = [](int32_t msg_class, int32_t msg_type)->int64_t
    {
        return msg_class * 100000 + msg_type;
    };
    static std::map<int64_t, EServerRouteNodeType> s_client_msg_forward_node_type
    {
        {INT_MSGCLASS(E_PROTOCOL_CLASS_LOGIN)*key_factor + E_LOGIN_MSG_C2S_TEST_ROLE_MSG, EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC}
    };

    int32_t msg_class = msg_pt->GetMsgClass();
    int32_t msg_type = msg_pt->GetMsgType();

    int64_t cur_key = key_gen(msg_class, msg_type);
    auto it_forward = s_client_msg_forward_node_type.find(cur_key);
    if (it_forward == s_client_msg_forward_node_type.end())
    {
        return false;
    }

    int64_t role_id = 0;
    auto session_pt = dynamic_cast<ServerSession*>(msg_pt->GetConnection());
    if (!session_pt)
    {
        return false;
    }
    if (session_pt->GetChannelInfo().user_id < 1)
    {
        return false;
    }
    auto user_info = g_ClientNetNodeMgr.HoldClientUser(session_pt->GetChannelInfo().user_id);
    if (!user_info)
    {
        return false;
    }
    role_id = user_info->role_id;

    EServerRouteNodeType node_type = it_forward->second;

    // msg_pt->SetParam(role_id);
    auto server_node = g_ClientNetNodeMgr.GetRoleNode(role_id, node_type);
    if (!server_node)
    {
        TERROR("not found client node,node_type:" << static_cast<int32_t>(node_type) << ", role_id:" << role_id);
        return false;
    }
    TDEBUG("forward client role msg, role_id:" << role_id << ", msg_class:" << msg_pt->GetMsgClass()
        << ", msg_type:" << msg_pt->GetMsgType());

    MsgAddonParam msg_addonparam;
    msg_addonparam.role_id = role_id;
    auto cb_param = g_MsgHelper.GenAsyncMsgEnvParam(session_pt, msg_pt);
    g_MsgHelper.ForwardAsyncMessage(msg_pt->GetMsgClass(), msg_pt->GetMsgType(),
        msg_pt->GetContent(), 
        [](const NetMessage * rep_msg, const AsyncMsgParam & cb_param)
        {
            g_MsgHelper.SendAsyncRepNetMsg(rep_msg, cb_param);
        },
        std::move(cb_param),
        node_type,
        server_node->node_index,
        server_node->zone_id,
        &msg_addonparam);

    return true;
}