/*=======================================================
# Author       : mknight
# Last modified: 2020-02-15 14:24
# Email        : 824338670@qq.com
# Filename     : world_reg_msg_handler.cpp
# Description  : 
# =======================================================*/

#include "world_server/world_server.h"
#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/msg_handler.h"
#include "protcl_frame.pb.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "tr_log/log_module.h"
#include "world_frame_handle.h"

void WorldServer::RegisterMsgHandle()
{
    AddLoopRun([](time_t cur_mtime)
	{
		g_MsgDispatcher.DispatchMessage();
	});
    
    g_MsgDispatcher.RegisterMsgHandlerEx<WorldFrameHandler>(int32_t(EProtocolClass::E_PROTOCOL_CLASS_FRAME));

    TINFO("world server register msg handlers");
}
