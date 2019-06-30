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
GameServer::GameServer(int32_t index)
{
	index_ = index;
}
GameServer::~GameServer()
{

}

bool GameServer::Init()
{
	if (!g_ServerConfig.Load())
    {
        Stop();
    }
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
    std::map<EServerRouteNodeType, std::vector<std::string> > node_bootup_connect = 
    {
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_ROOT, {"login_server"}},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_DATA, {"root_server"}},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_CENTER, {"root_server", "data_server"}},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_LOGIC, {"root_server", "center_server", "data_server"}},
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, {"root_server", "center_server"}},
    };
    auto it_list = node_bootup_connect.find(node_type_);
    if (it_list == node_bootup_connect.end())
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
        server_session->SetChannelType(ESessionChannelType::E_CHANNEL_SERVER_TO_SERVER);
        auto result = GetRouteTypeByServerName(server_name);
        if (result.first)
        {
            server_session->SetChannelServerInfo(result.second, 0, ServerSession::CalcServerId(result.second, 0));
        }
        else
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
        g_ServerManager.AddServerInfo(server_session->GetConnId(), 
            result.second, 
            server_type_ret.second, 
            0);
        
        TINFO("bootup add server info, session_id:" << server_session->GetConnId()
            << ", node_type:" << (int32_t)(result.second) 
            << ", server_type:" << (int32_t)server_type_ret.second);
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
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, "gate_server"}
    };
    auto it = s_route_server_name.find(route_type);
    if (it != s_route_server_name.end())
    {
        return std::make_pair(true, it->second);
    }
   
    return std::make_pair(false, "");
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
        {"gate_server", EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE}
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
        {EServerRouteNodeType::E_SERVER_ROUTE_NODE_GATE, EServerType::E_SERVER_TYPE_GATE_SERVER}
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