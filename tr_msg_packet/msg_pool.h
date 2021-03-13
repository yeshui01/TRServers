/*=====================================================
# Author       : mknight
# Last modified: 2021-02-18 10:31
# Email        : 824338670@qq.com
# Filename     : msg_pool.h
# Description  : 
# =====================================================*/

#ifndef __TR_MSG_POOL_H__
#define __TR_MSG_POOL_H__

#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_common/singleton.h"
#include <list>

class NetMessage;

class NetMsgPool : public TSingleton<NetMsgPool>
{
public:
    NetMsgPool();
    ~NetMsgPool();

    NetMessage * ApplyMsg();
    void RecycleMsg(NetMessage * & msg_data);

// protected:
//     std::list<NetMessage *> net_messges_;
//     int32_t limit_size_ = 1;
};

#define g_NetMsgPool NetMsgPool::Instance()
#endif // TR_MSG_POOL_H__