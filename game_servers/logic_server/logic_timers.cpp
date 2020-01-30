/*=======================================================
# Author       : mknight
# Last modified: 2020-01-27 11:50
# Email        : 824338670@qq.com
# Filename     : logic_timers.cpp
# Description  : 
# =======================================================*/
#include "logic_timers.h"
#include "tr_timer/server_time.h"
#include "logic_global.h"

LogicSecTimer::LogicSecTimer(time_t run_mtime) : TRTimer(run_mtime)
{

}

LogicSecTimer::~LogicSecTimer()
{
    
}

// 执行定时器逻辑
void LogicSecTimer::RunLogic()
{
    g_LogicGlobal.SecUpdate(GetRunTime() / TIME_MSEC_PER_SEC);
}

// 返回下次执行的时间,如果返回0,表示不需要执行了
time_t LogicSecTimer::NextTime()
{
    return g_ServerTime.NowTimeMs() + TIME_MSEC_PER_SEC * 1;
}
