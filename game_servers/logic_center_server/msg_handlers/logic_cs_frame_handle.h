/*=======================================================
# Author       : mknight
# Last modified: 2020-02-03 14:21
# Email        : 824338670@qq.com
# Filename     : logic_cs_frame_handle.h
# Description  : 
# =======================================================*/
#ifndef __TR_LOGIC_CS_FRAME_HANDLE_H__
#define __TR_LOGIC_CS_FRAME_HANDLE_H__

#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/net_message.h"
#include "tr_msg_packet/msg_handler.h"
#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_log/log_module.h"

class LogicCenterFrameHandler : public IMessageHandler
{
public:
    LogicCenterFrameHandler();
	virtual ~LogicCenterFrameHandler();
    // 绑定消息处理接口
	void BindMsgHandle() override;

public:
    // ----------------- 消息处理成员函数 -------------------------
    // 服务器信息注册
    DECLARE_MSG_HANDLER_FUN(OnRegisterServerInfo);
    // 等待其他服务器完成，开始运行
    DECLARE_MSG_HANDLER_FUN(OnRecvRootStartCmd);
};
#endif // __TR_LOGIC_CS_FRAME_HANDLE_H__
