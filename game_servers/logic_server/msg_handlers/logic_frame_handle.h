/*=======================================================
# Author       : mknight
# Last modified: 2019-07-20 14:34
# Email        : 824338670@qq.com
# Filename     : logic_frame_handle.h
# Description  : 
# =======================================================*/


#ifndef __LOGIC_FRAME_MSG_HANDLE_H__
#define __LOGIC_FRAME_MSG_HANDLE_H__
#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/net_message.h"
#include "tr_msg_packet/msg_handler.h"
#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_log/log_module.h"

class LogicFrameHandler : public IMessageHandler
{
public:
    LogicFrameHandler();
	virtual ~LogicFrameHandler();
    // 绑定消息处理接口
	void BindMsgHandle() override;

public:
    // 消息处理成员函数
    EMsgHandleResult OnRegisterServerInfo(TConnection *session_pt, const NetMessage * message_pt);
    // 等待其他服务器完成，开始运行
    EMsgHandleResult OnRecvRootStartCmd(TConnection *session_pt, const NetMessage * message_pt);
    // 客户端下线
    DECLARE_MSG_HANDLER_FUN(OnLogicUserOffline);
};

#endif // __LOGIC_FRAME_MSG_HANDLE_H__