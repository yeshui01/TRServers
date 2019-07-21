/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-29 20:54
 * Email        : 824338670@qq.com
 * Filename     : game_server.h
 * Description  : 游戏服务器基类
 * ======================================================*/

#ifndef __TR_GAME_SERVER_H__
#define __TR_GAME_SERVER_H__
#include <iostream>
#include <string>
#include <cstdint>
#include <csignal>
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
#include "server_define.h"
#include <string>

class GameServer;

using bootup_wait_event_do_t = std::function<void ()>;
enum class EBootupWaitEvtState : int8_t
{
    E_BOOTUP_WAIT_EVT_STATE_READY = 0,
    E_BOOTUP_WAIT_EVT_STATE_DOING = 1,
    E_BOOTUP_WAIT_EVT_STATE_FINISH = 2,    
};
// 服务器启动等待事件
struct ServerBootupWaitEvent
{
    int32_t evt_id = 0;
    time_t start_time = 0;                                      // 开始等待的时间
    EBootupWaitEvtState state = EBootupWaitEvtState::E_BOOTUP_WAIT_EVT_STATE_READY;
    bootup_wait_event_do_t do_event;
};

using GameParentClass = TBaseServer;
class GameServer : public TBaseServer
{
public:
	GameServer(int32_t index);
	~GameServer();
	
public:
    // 注册信号处理
    void RegSignalHandle(std::function<void (int32_t sig)> && sig_fun);
    virtual bool Init();
    // 新的连接建立了
    virtual void OnNewConnectComeIn(TConnection * new_connection);
    // 运行前的检查
    virtual bool RunStepCheck();
    // 即将运行
    virtual bool RunStepWillRun();
    // 等待启动
    virtual bool RunStepWaiting();
    // 等待其他服务器
    virtual bool RunStepWaitOtherServers();
    // 运行前一刻
    virtual bool RunStepPreRun();
    // 正常运行
    virtual bool RunStepRunning();
    // 注册消息处理
    virtual void RegisterMsgHandle();
    
    // 启动监听
    bool StartLocalListen(std::string server_name);
    bool StartLocalListenByIndex(std::string server_name, int32_t index);
    // 根据服务器节点类型获取服务器名字
    std::pair<bool, std::string> GetServerNameByNodeType(EServerRouteNodeType route_type);
    // 根据服务器名字获取路由节点类型
    std::pair<bool, EServerRouteNodeType> GetRouteTypeByServerName(const std::string server_name);
    // 根据服务器路由节点获取服务区类型
    std::pair<bool, EServerType> GetServerTypeByNodeType(EServerRouteNodeType route_type);
    // 完成某个启动等待的事件
    void FinishOneBootupWaitEvent(int32_t wait_evt_id);
    // 添加一个启动等待的类型事件
    void AddOneBootupWaitEvent(int32_t wait_evt_id, bootup_wait_event_do_t && evt_do);
    // 设置是否等待其他服务器
    void SetWaitOtherServers(bool wait);
protected:
    // 启动时连接其他服务器
    virtual bool BootUpConnectServer();
    // 检查服务器连接是否都连接完毕
    virtual bool CheckAllServerConnected();
    // 输出debug数据
    void PrintDebugInfo();
    // 信号处理
    static void SignalHandle(int32_t sig_num);
    // 查询并连接其他服务器节点
    void QueryAndConnectOtherServer(EServerRouteNodeType node_type, std::function<void ()> && finish_cb);
    // 向其他服务器节点注册自己的服务器节点数据
    void RegServerInfoToOtherServers(EServerRouteNodeType node_type, int32_t index);
    // 根据服务器节点类型和配置连接服务器
    bool ConnectToOtherServer(EServerRouteNodeType node_type, int32_t index);
protected:
	EServerType server_type_ = EServerType::E_SERVER_TYPE_INVALID_SERVER;
    EServerRouteNodeType node_type_ = EServerRouteNodeType::E_SERVER_ROUTE_NODE_NONE;
    int32_t index_ = 0;
    time_t bootup_update_time_ = 0;
    std::map<int32_t, ServerBootupWaitEvent> wait_events_;  // 等待启动的事件列表
    bool wait_other_servers_ = true;
protected:
    static std::vector<std::function<void (int32_t sig)> > s_v_sigfuns_;
    static std::map<EServerRouteNodeType, std::vector<std::string> > s_node_bootup_connect_;
};
#endif // __TR_GAME_SERVER_H__
