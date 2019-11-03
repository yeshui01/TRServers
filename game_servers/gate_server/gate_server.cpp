/*=======================================================
# Author       : mknight
# Last modified: 2019-06-29 15:05
# Email        : 824338670@qq.com
# Filename     : gate_server.cpp
# Description  : 
# =======================================================*/

#include "gate_server.h"
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
#include "server_common/launch_wait_event.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "protocol_error_code.h"
#include "protcl_frame.pb.h"
#include "server_common/game_msg_helper.h"

#include <string>

GateServer::GateServer(int32_t index):GameServer(index)
{
    server_type_ = EServerType::E_SERVER_TYPE_GATE_SERVER;
    node_type_ = EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE;
}

GateServer::~GateServer()
{

}

bool GateServer::Init()
{
    if (!GateParentClass::Init())
    {
        return false;
    }
    return true;
}

// 运行前的检查
bool GateServer::RunStepCheck()
{
    if (!StartLocalListenByIndex("gate_server", index_))
    {
        Stop();
        return false;
    }
    
    return true;
}

// 正常运行
bool GateServer::RunStepRunning()
{
    GateParentClass::RunStepRunning();
    return true;
}

void GateServer::OnNewConnectComeIn(TConnection * new_connection)
{
    GateParentClass::OnNewConnectComeIn(new_connection);
}

bool GateServer::RunStepWillRun()
{
    if (!GateParentClass::RunStepWillRun())
        return false;
    // 通知其他服务器自己的服务器节点数据
    std::vector<EServerRouteNodeType> v_node_type = {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT,
        /*EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA,*/
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER};
    for (auto && node_type : v_node_type)
    {
        RegServerInfoToOtherServers(node_type, 0);
    }

    // 查询并且连接logic_server
    int32_t evt_id_connect_logic_server = E_SERVER_LAUNCH_WAIT_LOGIC_SERVERS;
    AddOneBootupWaitEvent(evt_id_connect_logic_server, [evt_id_connect_logic_server, this]()
    {
        TINFO("do event func, evt_id:" << evt_id_connect_logic_server);
        this->QueryAndConnectOtherServer(EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, [evt_id_connect_logic_server,this]()
        {
            const_cast<GateServer*>(this)->FinishOneBootupWaitEvent(evt_id_connect_logic_server);
        });
    });
    return true;
}
bool GateServer::RunStepWaiting()
{
    return GateParentClass::RunStepWaiting();
}

bool GateServer::RunStepPreRun()
{
    GateParentClass::RunStepPreRun();
    // test ,往login_server发一个测试消息
    REQMSG(E_FRAME_MSG_FORWARD_TEST_MESSAGE) req;
    TINFO("send test message");
    req.set_test_id(1);
    req.set_show_text("hello forward ascync");
    g_MsgHelper.ForwardAsyncPbMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
        INT_FRAMEMSG(E_FRAME_MSG_FORWARD_TEST_MESSAGE), req,
        [](const NetMessage * rep_msg, const AsyncMsgParam & cb_param){
            REPMSG(E_FRAME_MSG_FORWARD_TEST_MESSAGE) rep;
            STRING_TO_PBMSG(rep_msg->GetContent(), rep);
            TDEBUG("asyncmsg callback:rep_E_FRAME_MSG_FORWARD_TEST_MESSAGE:" << rep.ShortDebugString());
        }, AsyncMsgParam(),
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIN, 0, 0);

    req.set_test_id(2);
    req.set_show_text("hello cross forward message");
    g_MsgHelper.ForwardPbMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
        INT_FRAMEMSG(E_FRAME_MSG_FORWARD_TEST_MESSAGE), req,
        EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIN, 0, -1);
    return true;
}