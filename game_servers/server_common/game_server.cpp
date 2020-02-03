/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-29 20:55
 * Email        : 824338670@qq.com
 * Filename     : game_server.cpp
 * Description  : 
 * ======================================================*/

#include "game_server.h"
#include "server_config.h"
#include "server_session.h"
#include "server_info_manager.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "protcl_frame.pb.h"
#include "game_msg_helper.h"
#include "tr_timer/global_timer.h"
#include "server_common/server_describe.h"

GameServer::GameServer(int32_t index)
{
	index_ = index;
}
GameServer::~GameServer()
{

}

std::vector<std::function<void (int32_t sig)> > GameServer::s_v_sigfuns_ = {};

std::map<EServerRouteNodeType, std::vector<std::string>> GameServer::s_node_bootup_connect_ =
{
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, {"login_server"}},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA, {"root_server"}},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, {"root_server", "data_server"}},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, {"root_server", "center_server", "data_server", "logic_center_server"}},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, {"root_server", "center_server", "logic_center_server"}},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC_CENTER, {"root_server", "center_server", "data_server"}},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_VIEW, {"view_manager_server"}},
};

bool GameServer::Init()
{
    signal(SIGINT, SignalHandle);
    signal(SIGTERM, SignalHandle);
    signal(SIGABRT, SignalHandle);
    if (EServerRouteNodeType::E_SERVER_ROUTE_NODE_NONE == node_type_ ||
        EServerType::E_SERVER_TYPE_INVALID_SERVER == server_type_)
    {
        TERROR("server type and node_type not set initvalue, maybe in constructor should init them");
        return false;
    }
    g_ServerDes.Load();
	if (!g_ServerConfig.Load())
    {
        Stop();
        return false;
    }
    
    g_ServerManager.SetCurrentServerInfo(node_type_,
        server_type_, index_);
    RegisterMsgHandle();

    
    RegSignalHandle([this](int32_t sig_num) mutable{
        const_cast<GameServer*>(this)->Stop();
    });
    
    AddLoopRun([](time_t cur_mtime)
    {
        g_GlobalTimer.Update(cur_mtime);
    });
    return true;
}

// 运行前的检查
bool GameServer::RunStepCheck()
{
	GameParentClass::RunStepCheck();
	return true;
}
// 正常运行
bool GameServer::RunStepRunning()
{
	return GameParentClass::RunStepRunning();
}

// 新的连接建立了
void GameServer::OnNewConnectComeIn(TConnection * new_connection)
{
	GameParentClass::OnNewConnectComeIn(new_connection);
}

bool GameServer::BootUpConnectServer()
{
    // 跟本区的唯一服建立路由连接
    // std::map<EServerRouteNodeType, std::vector<std::string> > s_node_bootup_connect_ = 
    // {
    //     {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, {"login_server"}},
    //     {EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA, {"root_server"}},
    //     {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, {"root_server", "data_server"}},
    //     {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, {"root_server", "center_server", "data_server"}},
    //     {EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, {"root_server", "center_server"}},
    // };
    auto it_list = s_node_bootup_connect_.find(node_type_);
    if (it_list == s_node_bootup_connect_.end())
    {
        return true;
    }
    for (auto & v : it_list->second)
    {
        auto const & jv_config = g_ServerConfig.GetJsonConfig();
        if (!jv_config.isMember(v))
        {
            TERROR("not found config:" << v);
            return false;
        }
        std::string server_name = v;
        std::string ip = jv_config[server_name]["ip"].asString();
        int32_t port = jv_config[server_name]["port"].asInt();
        auto session = AllocateConnect();
        if (!session)
        {
            TERROR("session is nullptr in bootup connect");
            return false;
        }
        ESocketOpCode op_code = session->Connect(ip, port);
        if (op_code != ESocketOpCode::E_SOCKET_OP_CODE_CORRECT)
        {
            TERROR("session is connect failed, code:" << (int32_t)op_code << ", server_name:" << server_name);
            return false;
        }
        TINFO("bootup connect success, server_name:" << server_name);
        ServerSession * server_session = dynamic_cast<ServerSession *>(session);
        if (!server_session)
        {
            TERROR("logic error, session type is error");
            session->Close();
            wait_recycle_connect_.push_back(session);
            return false;
        }
        server_session->SetStatus(ESocketStatus::E_SOCKET_STATUS_CONNECTED);
        // 1.设置连接通道数据
        // server_session->SetChannelType(ESessionChannelType::E_CHANNEL_SERVER_TO_SERVER);
        auto result = GetRouteTypeByServerName(server_name);
        // if (result.first)
        // {
        //     server_session->SetChannelServerInfo(result.second, 0, ServerSession::CalcServerId(result.second, 0));
        // }
        
        if (result.first == false)
        {
            TERROR("not found node type, server_name:" << server_name);
            session->Close();
            RecycleConnect(session);
            return false;
        }
        auto server_type_ret = GetServerTypeByNodeType(result.second);
        if (!server_type_ret.first)
        {
            TERROR("not found server type, node_type:" << (int32_t)result.second);
            session->Close();
            RecycleConnect(session);
            return false;
        }
        // 2.登记到服务器管理
        int32_t node_zone_id = g_ServerConfig.GetZoneId();
        // if (EServerRouteNodeType::E_SERVER_ROUTE_NODE_LIST == result.second || 
        //     EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIN == result.second || 
        //     EServerRouteNodeType::E_SERVER_ROUTE_NODE_WORLD == result.second || 
        //     EServerRouteNodeType::E_SERVER_ROUTE_NODE_WORLD_CENTER == result.second || 
        //     EServerRouteNodeType::E_SERVER_ROUTE_NODE_VIEW_MANAGER == result.second)
        if (g_MsgHelper.IsWorldServerNode(result.second))
        {
            node_zone_id = g_ServerConfig.GetWorldZoneId();
            TINFO("global or world node, zone id is node_zone_id");
        }
        else if (g_MsgHelper.IsGlobalServerNode(result.second))
        {
            node_zone_id = g_ServerConfig.GetGlobalZoneId();
        }
        g_ServerManager.AddServerInfo(server_session, 
            result.second, 
            server_type_ret.second, 
            0, node_zone_id);
        
        TINFO("bootup add server info, session_id:" << server_session->GetConnId()
            << ", node_type:" << (int32_t)(result.second) 
            << ", server_type:" << (int32_t)server_type_ret.second
            << ", fd:" << server_session->GetFd()
            << ", sessionptr:" << (int64_t)(server_session)
            << ", node_zone_id:" << node_zone_id);
        // 3.添加管理记录
        TBaseServer::OnNewConnectComeIn(server_session);
    }

    return true;
}

bool GameServer::StartLocalListen(std::string server_name)
{
	// 绑定地址
    auto const & jv_config = g_ServerConfig.GetJsonConfig();
    
    if (jv_config.isMember(server_name))
    {
        std::string ip = jv_config[server_name]["ip"].asString();
        int32_t port = jv_config[server_name]["port"].asInt();
        int32_t sock_num = jv_config[server_name]["sock_num"].asInt();
        auto code = Bind(ip, port);
        if (ESocketOpCode::E_SOCKET_OP_CODE_CORRECT == code)
        {
            if (ESocketOpCode::E_SOCKET_OP_CODE_CORRECT != Listen())
            {
                TERROR("listen error, ip:" << ip << ", port:" << port);
                return false;
            }
            else 
            {
                TINFO("listen sccess, ip:" << ip << ", port:" << port << ", sock_num:" << sock_num);
                InitConnectionPool<ServerSession>(sock_num);
            }
        }
        else
        {
            TERROR("bind error, ip:" << ip << ", port:" << port);
            return false;
        }
    }
    else
    {
        TERROR("not found config:" << server_name);
        return false;
    }
    
    return true;
}

bool GameServer::StartLocalListenByIndex(std::string server_name, int32_t index)
{
    // 绑定地址
    auto const & jv_config = g_ServerConfig.GetJsonConfig();
    
    if (jv_config.isMember(server_name))
    {
        if (jv_config.size() < (index_ + 1))
        {
            TERROR("jv_config size error, server_name:" << server_name);
            return false;
        }
        auto const & jv_elem = jv_config[server_name][index_];
        std::string ip = jv_elem["ip"].asString();
        int32_t port = jv_elem["port"].asInt();
        int32_t sock_num = jv_elem["sock_num"].asInt();
        auto code = Bind(ip, port);
        if (ESocketOpCode::E_SOCKET_OP_CODE_CORRECT == code)
        {
            if (ESocketOpCode::E_SOCKET_OP_CODE_CORRECT != Listen())
            {
                TERROR("listen error, ip:" << ip << ", port:" << port << ", index:" << index_);
                return false;
            }
            else 
            {
                TINFO("listen sccess, ip:" << ip << ", port:" << port << ", sock_num:" << sock_num << ",index:" << index_);
                InitConnectionPool<ServerSession>(sock_num);
            }
        }
        else
        {
            TERROR("bind error, ip:" << ip << ", port:" << port << ", index:" << index_);
            return false;
        }
    }
    else
    {
        TERROR("not found config:" << server_name);
        return false;
    }
    
    return true;
}

// 根据服务器节点类型获取服务器名字
std::pair<bool, std::string> GameServer::GetServerNameByNodeType(EServerRouteNodeType route_type)
{
    static std::map<EServerRouteNodeType, std::string> s_route_server_name = 
    {
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIN, "login_server"},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, "root_server"},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA, "data_server"},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, "center_server"},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, "logic_server"},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, "gate_server"},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC_CENTER, "logic_center_server"},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_VIEW_MANAGER, "view_manager_server"},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_VIEW, "view_server"},
    };
    auto it = s_route_server_name.find(route_type);
    if (it != s_route_server_name.end())
    {
        return std::make_pair(true, it->second);
    }
   
    return std::make_pair(false, "unknown_server");
}
// 根据服务器名字获取路由节点类型
std::pair<bool, EServerRouteNodeType> GameServer::GetRouteTypeByServerName(const std::string server_name)
{
    static std::map<std::string, EServerRouteNodeType> s_server_name_route = 
    {
        {"login_server", EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIN},
        {"root_server", EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT},
        {"data_server", EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA},
        {"center_server", EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER},
        {"logic_server", EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC},
        {"gate_server", EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE},
        {"logic_center_server", EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC_CENTER},
        {"view_manager_server", EServerRouteNodeType::E_SERVER_ROUTE_NODE_VIEW_MANAGER},
        {"view_server", EServerRouteNodeType::E_SERVER_ROUTE_NODE_VIEW}
    };

    auto it = s_server_name_route.find(server_name);
    if (it != s_server_name_route.end())
    {
        return std::make_pair(true, it->second);
    }

    return std::make_pair(false, EServerRouteNodeType::E_SERVER_ROUTE_NODE_NONE);
}

std::pair<bool, EServerType> GameServer::GetServerTypeByNodeType(EServerRouteNodeType route_type)
{
    static std::map<EServerRouteNodeType, EServerType> s_node_server_type = 
    {
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIN, EServerType::E_SERVER_TYPE_LOGIN_SERVER},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, EServerType::E_SERVER_TYPE_ROOT_SERVER},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA, EServerType::E_SERVER_TYPE_DATA_SERVER},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, EServerType::E_SERVER_TYPE_CENTER_SERVER},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, EServerType::E_SERVER_TYPE_LOGIC_SERVER},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, EServerType::E_SERVER_TYPE_GATE_SERVER},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC_CENTER, EServerType::E_SERVER_TYPE_LOGIC_CENTER_SERVER},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_VIEW_MANAGER, EServerType::E_SERVER_TYPE_VIEW_MANAGER_SERVER},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_VIEW, EServerType::E_SERVER_TYPE_VIEW_SERVER}
    };

    auto it = s_node_server_type.find(route_type);
    if (it != s_node_server_type.end())
    {
        return std::make_pair(true, it->second);
    }
    return std::make_pair(false, EServerType::E_SERVER_TYPE_INVALID_SERVER);
}

bool GameServer::RunStepWillRun()
{
    GameParentClass::RunStepWillRun();
    if (!BootUpConnectServer())
    {
        Stop();
        TERROR("BootUpConnectServer failed");
        return false;
    }
    return true;
}

void GameServer::RegisterMsgHandle()
{

}

bool GameServer::CheckAllServerConnected()
{
    return true;
}

void GameServer::PrintDebugInfo()
{
    // TODO:
}

void GameServer::RegSignalHandle(std::function<void (int32_t sig)> && sig_fun)
{
    s_v_sigfuns_.push_back(sig_fun);
}

void GameServer::SignalHandle(int32_t sig_num)
{
    TINFO("signal trigger, sig_num:" << sig_num);
    for (auto && f : s_v_sigfuns_)
    {
        f(sig_num);
    }
}

// 等待启动
bool GameServer::RunStepWaiting()
{
    bool all_finished = true;
    for (auto it = wait_events_.begin(); it != wait_events_.end(); ++it)
    {
        if (it->second.state == EBootupWaitEvtState::E_BOOTUP_WAIT_EVT_STATE_READY)
        {
            it->second.do_event();
            it->second.state = EBootupWaitEvtState::E_BOOTUP_WAIT_EVT_STATE_DOING;
        }
        if (it->second.state != EBootupWaitEvtState::E_BOOTUP_WAIT_EVT_STATE_FINISH)
        {
            all_finished = false;
        }
    }
    if (all_finished)
    {
        if (server_type_ != EServerType::E_SERVER_TYPE_ROOT_SERVER && !g_MsgHelper.IsGlobalServerNode(g_ServerManager.GetCurrentNodeType()))
        {
            TINFO("all wait event finished");
            // 通知root，开始等待其他服务器启动了
            REQMSG(E_FRAME_MSG_XS_TO_ROOT_WAIT_OTHERS) req;
            req.set_node_type(int32_t(node_type_));
            req.set_node_index(index_);
            g_MsgHelper.ForwardAsyncPbMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
            INT_FRAMEMSG(E_FRAME_MSG_XS_TO_ROOT_WAIT_OTHERS), req,
            [](const NetMessage *rep_msg, const AsyncMsgParam &cb_param) {
                REPMSG(E_FRAME_MSG_XS_TO_ROOT_WAIT_OTHERS) rep;
                STRING_TO_PBMSG(rep_msg->GetContent(), rep);
                TDEBUG("asyncmsg callback:rep_E_FRAME_MSG_XS_TO_ROOT_WAIT_OTHERS:" << rep.ShortDebugString());
            },
            AsyncMsgParam(), EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, 0);
        }
    }
    return all_finished;
}

bool GameServer::RunStepWaitOtherServers()
{
    return !wait_other_servers_;
}
// 运行前一刻
bool GameServer::RunStepPreRun()
{
    return GameParentClass::RunStepPreRun();
}

void GameServer::FinishOneBootupWaitEvent(int32_t wait_evt_id)
{
    auto it = wait_events_.find(wait_evt_id);
    if (it != wait_events_.end())
    {
        wait_events_.erase(it);
        TINFO("FinishOneBootupWaitEvent, wait_evt_id:" << wait_evt_id);
    }
}

// 添加一个启动等待的类型事件
void GameServer::AddOneBootupWaitEvent(int32_t wait_evt_id, bootup_wait_event_do_t && evt_do)
{
    ServerBootupWaitEvent wait_evt;
    wait_evt.evt_id = wait_evt_id;
    wait_evt.do_event = evt_do;
    wait_evt.state = EBootupWaitEvtState::E_BOOTUP_WAIT_EVT_STATE_READY;
    auto ret = wait_events_.insert(std::make_pair(wait_evt_id, wait_evt));
    if (!ret.second)
    {
        TERROR("add wait event faield, evt_id:" << wait_evt_id);
        return;
    }
    TINFO("AddOneBootupWaitEvent, wait_evt_id:" << wait_evt_id << ", evt_size:" << wait_events_.size());
}

void GameServer::QueryAndConnectOtherServer(EServerRouteNodeType node_type, std::function<void ()> && finish_cb)
{
    TINFO("QueryAndConnectOtherServer, node_type:" << int32_t(node_type));
    REQMSG(E_FRAME_MSG_QUERY_SERVER_NODE_LIST) req;
    req.set_node_type(int32_t(node_type));
    g_MsgHelper.ForwardAsyncPbMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
        INT_FRAMEMSG(E_FRAME_MSG_QUERY_SERVER_NODE_LIST), req,
        [finish_cb, this](const NetMessage *rep_msg, const AsyncMsgParam &cb_param) {
            REPMSG(E_FRAME_MSG_QUERY_SERVER_NODE_LIST) rep;
            STRING_TO_PBMSG(rep_msg->GetContent(), rep);
            TDEBUG("asyncmsg callback:E_FRAME_MSG_QUERY_SERVER_NODE_LIST:" << rep.ShortDebugString());
            for (auto i = 0; i < rep.server_list_size(); ++i)
            {
                int32_t node_type = rep.server_list(i).node_type();
                // int32_t server_type = rep.server_list_(i).server_type();
                int32_t index = rep.server_list(i).server_index();
                this->ConnectToOtherServer(EServerRouteNodeType(node_type), index);
            }
            for (auto i = 0; i < rep.server_list_size(); ++i)
            {
                int32_t node_type = rep.server_list(i).node_type();
                // int32_t server_type = rep.server_list_(i).server_type();
                int32_t index = rep.server_list(i).server_index();
                this->RegServerInfoToOtherServers(EServerRouteNodeType(node_type), index);
            }
            finish_cb();
        },
        AsyncMsgParam(), EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, 0);
}

// 向其他服务器节点注册自己的服务器节点数据
void GameServer::RegServerInfoToOtherServers(EServerRouteNodeType node_type, int32_t index)
{
    TINFO("RegServerInfoToOtherServers, node_type:" << int32_t(node_type) << ", index:" << index);
    REQMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) req;
    req.mutable_server_node()->set_node_type(int32_t(node_type_));
    req.mutable_server_node()->set_server_type(int32_t(server_type_));
    req.mutable_server_node()->set_server_index(index_);
    req.mutable_server_node()->set_zone_id(g_ServerConfig.GetZoneId());
    // PBMSG_TO_STR(req, msg_str);

    g_MsgHelper.ForwardAsyncPbMessage(INT_MSGCLASS(E_PROTOCOL_CLASS_FRAME),
        INT_FRAMEMSG(E_FRAME_MSG_REGISTER_SERVER_INFO), req,
        [](const NetMessage *rep_msg, const AsyncMsgParam &cb_param) {
            REPMSG(E_FRAME_MSG_REGISTER_SERVER_INFO) rep;
            STRING_TO_PBMSG(rep_msg->GetContent(), rep);
            TDEBUG("asyncmsg callback:rep_E_FRAME_MSG_REGISTER_SERVER_INFO:" << rep.ShortDebugString());
        }, AsyncMsgParam(), node_type, index);
}

bool GameServer::ConnectToOtherServer(EServerRouteNodeType node_type, int32_t index)
{
    auto check_ret = GetServerNameByNodeType(node_type);
    if (!check_ret.first)
    {
        TERROR("GetServerNameByNodeType failed, node_type:" << int32_t(node_type));
        return false;
    }
    std::string server_name = check_ret.second;
    auto const & jv_config = g_ServerConfig.GetJsonConfig();
    std::string ip = jv_config[server_name][index]["ip"].asString();
    int32_t port = jv_config[server_name][index]["port"].asInt();
    TINFO("ConnectToOtherServer, node_type:" << int32_t(node_type) 
        << ", index:" << index 
        << ", name:" << server_name
        << ", ip:" << ip 
        << ", port:" << port);
    auto session = AllocateConnect();
    if (!session)
    {
        TERROR("session is nullptr in bootup connect");
        return false;
    }
    ESocketOpCode op_code = session->Connect(ip, port);
    if (op_code != ESocketOpCode::E_SOCKET_OP_CODE_CORRECT)
    {
        TERROR("session is connect failed, code:" << (int32_t)op_code << ", server_name:" << server_name);
        return false;
    }
    TINFO("bootup connect success, server_name:" << server_name);
    ServerSession *server_session = dynamic_cast<ServerSession *>(session);
    if (!server_session)
    {
        TERROR("logic error, session type is error");
        session->Close();
        wait_recycle_connect_.push_back(session);
        return false;
    }
    server_session->SetStatus(ESocketStatus::E_SOCKET_STATUS_CONNECTED);
    // 1.设置连接通道数据
    server_session->SetChannelType(ESessionChannelType::E_CHANNEL_SERVER_TO_SERVER);
    auto result = GetRouteTypeByServerName(server_name);
    // if (result.first)
    // {
    //     server_session->SetChannelServerInfo(result.second, 0, ServerSession::CalcServerId(result.second, 0));
    // }
    // else
    if (result.first == false)
    {
        TERROR("not found node type, server_name:" << server_name);
        session->Close();
        RecycleConnect(session);
        return false;
    }
    auto server_type_ret = GetServerTypeByNodeType(result.second);
    if (!server_type_ret.first)
    {
        TERROR("not found server type, node_type:" << (int32_t)result.second);
        session->Close();
        RecycleConnect(session);
        return false;
    }
    int32_t node_zone_id = g_ServerConfig.GetZoneId();
    if (g_MsgHelper.IsWorldServerNode(result.second))
    {
        node_zone_id = g_ServerConfig.GetWorldZoneId();
        TINFO("global or world node, zone id is node_zone_id");
    }
    else if (g_MsgHelper.IsGlobalServerNode(result.second))
    {
        node_zone_id = g_ServerConfig.GetGlobalZoneId();
    }
    // 2.登记到服务器管理
    g_ServerManager.AddServerInfo(server_session,
                                  result.second,
                                  server_type_ret.second,
                                  index, node_zone_id);

    TINFO("bootup add server info, session_id:" << server_session->GetConnId()
                                                << ", node_type:" << (int32_t)(result.second)
                                                << ", server_type:" << (int32_t)server_type_ret.second
                                                << ", index:" << index
                                                << ", fd:" << server_session->GetFd()
                                                << ", sessionptr:" << (int64_t)(server_session));
    // 3.添加管理记录
    TBaseServer::OnNewConnectComeIn(server_session);
    return true;
}

void GameServer::SetWaitOtherServers(bool wait)
{
    wait_other_servers_ = wait;
    TINFO("SetWaitOtherServers, wait:" << wait);
}

bool GameServer::RunStepStop()
{
    GameParentClass::RunStepStop();

    return true;
}

// 获取节点类型
EServerRouteNodeType GameServer::GetRouteNodeType()
{
    return node_type_;
}
// 获取节点索引
int32_t GameServer::GetRouteNodeIndex()
{
    return index_;
}