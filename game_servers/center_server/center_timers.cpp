/*=======================================================
# Author       : mknight
# Last modified: 2020-01-31 11:01
# Email        : 824338670@qq.com
# Filename     : center_timers.cpp
# Description  : 
# =======================================================*/
#include "center_timers.h"
#include "tr_timer/server_time.h"
#include "center_global.h"

CenterSecTimer::CenterSecTimer(time_t run_mtime) : TRTimer(run_mtime)
{

}

CenterSecTimer::~CenterSecTimer()
{
    
}

// 执行定时器逻辑
void CenterSecTimer::RunLogic()
{
    g_CenterGlobal.SecUpdate(GetRunTime() / TIME_MSEC_PER_SEC);
}

// 返回下次执行的时间,如果返回0,表示不需要执行了
time_t CenterSecTimer::NextTime()
{
    return g_ServerTime.NowTimeMs() + TIME_MSEC_PER_SEC * 1;
}