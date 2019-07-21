/*=======================================================
# Author       : mknight
# Last modified: 2019-07-13 11:16
# Email        : 824338670@qq.com
# Filename     : msg_tools.h
# Description  : 
# =======================================================*/

#ifndef __TR_MSG_TOOLS_H__
#define __TR_MSG_TOOLS_H__
#include "tr_common/base_type_def.h"
#include "net_message.h"
#include "tr_common/singleton.h"
#include <functional>
#include <map>
class TConnection;
class NetMessage;

struct AsyncMsgParam
{
    TConnection * session_pt = nullptr;
};

using asyncmsg_callback_t = std::function<void (const NetMessage * rep_msg, const AsyncMsgParam & cb_param)>;

// 消息异步回调的上下文
struct AsyncMsgEnv
{
    time_t start_time = 0;
    TConnection * session_pt = nullptr;
    asyncmsg_callback_t callback;
    int32_t msg_class = 0;
    int32_t msg_type = 0;
    AsyncMsgParam cb_param;
};

class MessageTools : public TSingleton<MessageTools>
{
public:
    MessageTools();
    ~MessageTools();
public:
    /* 
    * 将消息和异步回调进行绑定
    * @param msg : 需要异步回调的消息
    * @param callback : 回调对象
    * @param session_pt : 回调对应的连接
    */
    bool BindAsyncCallback(NetMessage * msg, asyncmsg_callback_t && callback, AsyncMsgParam && cb_params);

    // 每秒更新
    void Update(time_t cur_time);

    // 处理回调消息
    bool HandleAsyncMsg(const NetMessage * msg);
    
    // 生成一个网络消息对象
    NetMessage MakeNetMessage(int32_t msg_class, int32_t msg_type, const std::string & content);
protected:
    int64_t GenNewAsyncId();

protected:
    // 回调唯一标识生成序号,大于0才有效
    int64_t async_id_ = 0;

    // 异步消息回调记录,key:async_id value:async info
    std::map<int64_t, AsyncMsgEnv> async_msgs_;
};

#define g_MsgTools MessageTools::Instance()

#endif // __TR_MSG_TOOLS_H__