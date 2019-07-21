/*=======================================================
# Author       : mknight
# Last modified: 2019-07-13 15:35
# Email        : 824338670@qq.com
# Filename     : login_reg_msg_handle.cpp
# Description  : 
# =======================================================*/

#include "login_server/login_server.h"
#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/msg_handler.h"
#include "login_frame_handle.h"
#include "protcl_frame.pb.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "tr_log/log_module.h"
void LoginServer::RegisterMsgHandle()
{
    AddLoopRun([](time_t cur_mtime)
	{
		g_MsgDispatcher.DispatchMessage();
	});
    
    g_MsgDispatcher.RegisterMsgHandlerEx<LoginFrameHandler>(int32_t(EProtocolClass::E_PROTOCOL_CLASS_FRAME));
    

    TINFO("login server register msg handlers");
}