/*=======================================================
# Author       : mknight
# Last modified: 2020-02-15 14:24
# Email        : 824338670@qq.com
# Filename     : world_frame_handle.h
# Description  : 
# =======================================================*/
#ifndef __TR_WORLD_FRAME_HANDLE_H__
#define __TR_WORLD_FRAME_HANDLE_H__

#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/net_message.h"
#include "tr_msg_packet/msg_handler.h"
#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_log/log_module.h"

class WorldFrameHandler : public IMessageHandler
{
public:
    WorldFrameHandler();
	virtual ~WorldFrameHandler();
    // 绑定消息处理接口
	void BindMsgHandle() override;

public:
    // ----------------- 消息处理成员函数 -------------------------
    // 服务器信息注册
    DECLARE_MSG_HANDLER_FUN(OnRegisterServerInfo);
};
#endif // __TR_WORLD_FRAME_HANDLE_H__
