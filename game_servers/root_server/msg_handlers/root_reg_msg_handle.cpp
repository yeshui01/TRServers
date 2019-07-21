/*=======================================================
# Author       : mknight
# Last modified: 2019-07-13 19:38
# Email        : 824338670@qq.com
# Filename     : root_reg_msg_handle.cpp
# Description  : 
# =======================================================*/

#include "root_server/root_server.h"
#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/msg_handler.h"
#include "protcl_frame.pb.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "tr_log/log_module.h"
#include "root_frame_handle.h"
void RootServer::RegisterMsgHandle()
{
    AddLoopRun([](time_t cur_mtime)
	{
		g_MsgDispatcher.DispatchMessage();
	});
    
    g_MsgDispatcher.RegisterMsgHandlerEx<RootFrameHandler>(int32_t(EProtocolClass::E_PROTOCOL_CLASS_FRAME));
    

    TINFO("root server register msg handlers");
}