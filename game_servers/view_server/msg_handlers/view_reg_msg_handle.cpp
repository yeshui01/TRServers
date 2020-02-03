/*=======================================================
# Author       : mknight
# Last modified: 2020-02-03 16:01
# Email        : 824338670@qq.com
# Filename     : view_reg_msg_handle.cpp
# Description  : 
# =======================================================*/

#include "view_server/view_server.h"
#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/msg_handler.h"
#include "protcl_frame.pb.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "tr_log/log_module.h"
#include "view_frame_handle.h"

void ViewServer::RegisterMsgHandle()
{
    AddLoopRun([](time_t cur_mtime)
	{
		g_MsgDispatcher.DispatchMessage();
	});
    
    g_MsgDispatcher.RegisterMsgHandlerEx<ViewFrameHandler>(int32_t(EProtocolClass::E_PROTOCOL_CLASS_FRAME));

    TINFO("view server register msg handlers");
}
