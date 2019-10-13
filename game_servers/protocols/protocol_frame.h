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

enum class EFrameMsgType : int32_t
{
    E_FRAME_MSG_REGISTER_SERVER_INFO                = 1,  // 注册服务器信息
    E_FRAME_MSG_NEW_SERVER_BOOTUP                   = 2,  // 通知服务器,有新的服务器连接
    E_FRAME_MSG_QUERY_SERVER_NODE_LIST              = 3,  // 查询服务器节点
    E_FRAME_MSG_ROOT_TO_XS_START_RUN                = 4,  // root server通知各服务器运行
    E_FRAME_MSG_XS_TO_ROOT_WAIT_OTHERS              = 5,  // 告诉rootserver,开始等待其他服务器了
    E_FRAME_MSG_FORWARD_MESSAGE                     = 6,  // 转发消息
    E_FRAME_MSG_FORWARD_TEST_MESSAGE                = 7,  // 测试用的转发消息
};

#define INT_FRAMEMSG(msg_type) int32_t(EFrameMsgType::msg_type)

#endif // __TR_PROTOCOL_FRAME_H__
