/*=======================================================
# Author       : mknight
# Last modified: 2019-07-20 14:32
# Email        : 824338670@qq.com
# Filename     : root_frame_handle.h
# Description  : 
# =======================================================*/

#ifndef __ROOT_FRAME_MSG_HANDLE_H__
#define __ROOT_FRAME_MSG_HANDLE_H__
#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/net_message.h"
#include "tr_msg_packet/msg_handler.h"
#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_log/log_module.h"

class RootFrameHandler : public IMessageHandler
{
public:
    RootFrameHandler();
	virtual ~RootFrameHandler();
    // 绑定消息处理接口
	void BindMsgHandle() override;

public:
    // 消息处理成员函数
    // 处理注册服务器信息
    EMsgHandleResult OnRegisterServerInfo(TConnection *session_pt, const NetMessage * messag_pt);
    // 查询类型节点服务器列表
    EMsgHandleResult OnQueryServerNodeList(TConnection *session_pt, const NetMessage * messag_pt);
    // 服务器等待开始
    EMsgHandleResult OnServerWaitOtherStart(TConnection *session_pt, const NetMessage * messag_pt);
};

#endif // __ROOT_FRAME_MSG_HANDLE_H__