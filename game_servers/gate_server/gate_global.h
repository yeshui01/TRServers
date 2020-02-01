/*=======================================================
# Author       : mknight
# Last modified: 2020-01-31 17:02
# Email        : 824338670@qq.com
# Filename     : gate_global.h
# Description  : 
# =======================================================*/
#ifndef __TR_GATE_GLOBAL_H__
#define __TR_GATE_GLOBAL_H__
#include "tr_common/base_type_def.h"
#include "tr_common/singleton.h"
#include <map>
#include <utility>

class NetMessage;

class GateGlobal : public TSingleton<GateGlobal>
{
public:
    GateGlobal();
    ~GateGlobal();

    bool Init();
    void Stop();

    // 转发消息
    bool ForwardClientMsg(NetMessage * msg_pt);
};

#define g_GateGlobal GateGlobal::Instance()

#endif // __TR_GATE_GLOBAL_H__
