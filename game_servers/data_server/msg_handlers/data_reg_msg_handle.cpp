/*=======================================================
# Author       : mknight
# Last modified: 2019-07-20 10:44
# Email        : 824338670@qq.com
# Filename     : data_reg_msg_handle.cpp
# Description  : 
# =======================================================*/

#include "data_server/data_server.h"
#include "tr_msg_packet/msg_dispatcher.h"
#include "tr_msg_packet/msg_handler.h"
#include "protcl_frame.pb.h"
#include "protocol_class.h"
#include "protocol_frame.h"
#include "tr_log/log_module.h"
#include "data_frame_handle.h"
#include "data_login_handle.h"
void DataServer::RegisterMsgHandle()
{
    AddLoopRun([](time_t cur_mtime)
	{
		g_MsgDispatcher.DispatchMessage();
	});
    
    g_MsgDispatcher.RegisterMsgHandlerEx<DataFrameHandler>(int32_t(EProtocolClass::E_PROTOCOL_CLASS_FRAME));
    g_MsgDispatcher.RegisterMsgHandlerEx<DataLoginHandler>(int32_t(EProtocolClass::E_PROTOCOL_CLASS_LOGIN));

    TINFO("data server register msg handlers");
}