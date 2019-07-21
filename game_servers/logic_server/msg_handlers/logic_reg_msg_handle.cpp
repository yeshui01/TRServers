/*=======================================================
# Author       : mknight
# Last modified: 2019-07-20 10:45
# Email        : 824338670@qq.com
# Filename     : logic_reg_msg_handle.cpp
# Description  : 
# =======================================================*/

#include "logic_server/logic_server.h"
#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/msg_handler.h"
#include "protcl_frame.pb.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "tr_log/log_module.h"
#include "logic_frame_handle.h"
void LogicServer::RegisterMsgHandle()
{
    AddLoopRun([](time_t cur_mtime)
	{
		g_MsgDispatcher.DispatchMessage();
	});
    
    g_MsgDispatcher.RegisterMsgHandlerEx<LogicFrameHandler>(int32_t(EProtocolClass::E_PROTOCOL_CLASS_FRAME));
    

    TINFO("logic server register msg handlers");
}