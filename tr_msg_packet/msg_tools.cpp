/*=======================================================
# Author       : mknight
# Last modified: 2019-07-13 11:17
# Email        : 824338670@qq.com
# Filename     : msg_tools.cpp
# Description  : 
# =======================================================*/
#include "msg_tools.h"
#include "net_message.h"
#include "tr_network/net_connection.h"
#include "net_message.h"
#include "tr_log/log_module.h"
#include "tr_timer/server_time.h"
#include "tr_common/func_tools.h"
MessageTools::MessageTools()
{

}

MessageTools::~MessageTools()
{

}

bool MessageTools::BindAsyncCallback(NetMessage * msg, asyncmsg_callback_t && callback, AsyncMsgParam && cb_params)
{
    if (!msg)
        return false;
    
    auto async_id = GenNewAsyncId();

    AsyncMsgEnv env;
    env.callback = std::move(callback);
    env.start_time = g_ServerTime.NowTime();
    // env.session_pt = session_pt;
    env.msg_class = msg->GetMsgClass();
    env.msg_type = msg->GetMsgType();
    env.cb_param = cb_params;
    auto ret = async_msgs_.insert(std::make_pair(async_id, env));
    if (!ret.second)
    {
        TERROR("bind async msg call back failed, async_id:" << async_id);
        return false;
    }
    msg->SetReqNo(async_id);
    TDEBUG("bind async msg callback, async_id:" << async_id << ", msg_class:" << msg->GetMsgClass() << ", msg_type:" 
        << msg->GetMsgType());
    return true;
}

// 每秒更新
void MessageTools::Update(time_t cur_time)
{
    // 回调超时处理
    for (auto it = async_msgs_.begin(); it != async_msgs_.end(); ++it)
    {
        if (cur_time - it->second.start_time >= 60)
        {
            TERROR("async msg timeout, start_time:" << it->second.start_time
                << ", msg_class:" << it->second.msg_class
                << ", msg_type:" << it->second.msg_type);

            async_msgs_.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

int64_t MessageTools::GenNewAsyncId()
{
    ++async_id_;
    if (async_id_ <= 0)
    {
        async_id_ = 1;
    }

    return async_id_;
}

bool MessageTools::HandleAsyncMsg(const NetMessage * msg)
{
    if (!msg)
        return false;
    
    if (msg->GetRepNo() < 1)
        return false;
    
    auto async_id = msg->GetRepNo();
    AsyncMsgEnv * async_env = FuncTools::GetMapValue(async_msgs_, async_id);
    if (!async_env)
    {
        // 超时了
        TINFO("async env not found, may timeout, async_id:" << async_id
            << ", msg_class:" << msg->GetMsgClass()
            << ", msg_type:" << msg->GetMsgType());
        return true;
    }
    async_env->callback(msg, async_env->cb_param);
    async_msgs_.erase(async_id);
    return true;
}

// 生成一个网络消息对象
NetMessage MessageTools::MakeNetMessage(int32_t msg_class, int32_t msg_type, const std::string & content)
{
    NetMessage msg(msg_class, msg_type);
    msg.SetContent(content);
    return msg;
}