/*=======================================================
# Author       : mknight
# Last modified: 2019-06-01 13:30
# Email        : 824338670@qq.com
# Filename     : protocol_frame.h
# Description  : 
# =======================================================*/

#ifndef __TR_PROTOCOL_FRAME_H__
#define __TR_PROTOCOL_FRAME_H__

#include "base_type_def.h"

enum EFrameMsgType : int32_t
{
    E_FRAME_MSG_REGISTER_SERVER_INFO                = 1,  // 注册服务器信息
    E_FRAME_MSG_NEW_SERVER_BOOTUP                   = 2,  // 通知服务器,有新的服务器连接
    E_FRAME_MSG_QUERY_SERVER_NODE_LIST              = 3,  // 查询服务器节点
    E_FRAME_MSG_ROOT_TO_XS_START_RUN                = 4,  // root server通知各服务器运行
    E_FRAME_MSG_XS_TO_ROOT_WAIT_OTHERS              = 5,  // 告诉rootserver,开始等待其他服务器了
    E_FRAME_MSG_FORWARD_MESSAGE                     = 6,  // 转发消息
    E_FRAME_MSG_FORWARD_TEST_MESSAGE                = 7,  // 测试用的转发消息
    E_FRAME_MSG_ROOT2XS_UPDATE_CLIENT_NET_NODE      = 8,  // 更新客户端网络节点
    E_FRAME_MSG_GG2ROOT_CLIENT_OFFLINE              = 9,  // gg->root客户端下线
    E_FRAME_MSG_ROOT_XS_CLIENT_OFFLINE              = 10, // root->xs 通知其他服务器客户端下线
    
    E_FRAME_MSG_XS_TO_DATA_SAVE_PLAYER_TABLES       = 11, // 保存玩家数据
    E_FRAME_MSG_XS_TO_DATA_SAVE_GLOBAL_TABLES       = 12, // 保存全局数据
    E_FRAME_MSG_XS_TO_DATA_LOAD_PLAYER_TABLES       = 13, // 加载玩家数据表
    E_FRAME_MSG_XS_TO_DATA_LOAD_GLOBAL_TABLES       = 14, // 加载全局数据表
};

#define INT_FRAMEMSG(msg_type) int32_t(EFrameMsgType::msg_type)

#endif // __TR_PROTOCOL_FRAME_H__
