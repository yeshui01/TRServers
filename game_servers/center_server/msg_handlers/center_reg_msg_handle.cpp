/*=======================================================
# Author       : mknight
# Last modified: 2019-07-20 10:43
# Email        : 824338670@qq.com
# Filename     : center_reg_msg_handle.cpp
# Description  : 
# =======================================================*/

#include "center_server/center_server.h"
#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/msg_handler.h"
#include "protcl_frame.pb.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "tr_log/log_module.h"
#include "center_frame_handle.h"
void CenterServer::RegisterMsgHandle()
{
    AddLoopRun([](time_t cur_mtime)
	{
		g_MsgDispatcher.DispatchMessage();
	});
    
    g_MsgDispatcher.RegisterMsgHandlerEx<CenterFrameHandler>(int32_t(EProtocolClass::E_PROTOCOL_CLASS_FRAME));
    

    TINFO("center server register msg handlers");
}