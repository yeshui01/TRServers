/*=====================================================
# Author       : mknight
# Last modified: 2021-02-18 10:31
# Email        : 824338670@qq.com
# Filename     : msg_pool.cpp
# Description  : 
# =====================================================*/

#include "msg_pool.h"
#include "net_message.h"

NetMsgPool::NetMsgPool()
{

}

NetMsgPool::~NetMsgPool()
{

}

NetMessage * NetMsgPool::ApplyMsg()
{
    return new NetMessage();
}

void NetMsgPool::RecycleMsg(NetMessage * & msg_data)
{
    if (msg_data)
    {
        delete msg_data;
        msg_data = nullptr;
    }
}