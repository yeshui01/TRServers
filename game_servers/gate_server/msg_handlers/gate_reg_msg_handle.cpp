/*=======================================================
# Author       : mknight
# Last modified: 2019-07-20 10:45
# Email        : 824338670@qq.com
# Filename     : gate_reg_msg_handle.cpp
# Description  : 
# =======================================================*/

#include "gate_server/gate_server.h"
#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/msg_handler.h"
#include "protcl_frame.pb.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "tr_log/log_module.h"
#include "gate_frame_handle.h"
#include "gate_login_handle.h"
void GateServer::RegisterMsgHandle()
{
    AddLoopRun([](time_t cur_mtime)
	{
		g_MsgDispatcher.DispatchMessage();
	});
    
    g_MsgDispatcher.RegisterMsgHandlerEx<GateFrameHandler>(int32_t(EProtocolClass::E_PROTOCOL_CLASS_FRAME));
    g_MsgDispatcher.RegisterMsgHandlerEx<GateLoginHandler>(int32_t(EProtocolClass::E_PROTOCOL_CLASS_LOGIN));

    TINFO("gate server register msg handlers");
}