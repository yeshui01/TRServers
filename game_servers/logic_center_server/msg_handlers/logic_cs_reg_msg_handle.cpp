/*=======================================================
# Author       : mknight
# Last modified: 2020-02-03 14:22
# Email        : 824338670@qq.com
# Filename     : logic_cs_reg_msg_handle.cpp
# Description  : 
# =======================================================*/

#include "logic_center_server/logic_center_server.h"
#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/msg_handler.h"
#include "protcl_frame.pb.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "tr_log/log_module.h"
#include "logic_cs_frame_handle.h"

void LogicCenterServer::RegisterMsgHandle()
{
    AddLoopRun([](time_t cur_mtime)
	{
		g_MsgDispatcher.DispatchMessage();
	});
    
    g_MsgDispatcher.RegisterMsgHandlerEx<LogicCenterFrameHandler>(int32_t(EProtocolClass::E_PROTOCOL_CLASS_FRAME));

    TINFO("logic center server register msg handlers");
}