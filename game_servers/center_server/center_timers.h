/*=======================================================
# Author       : mknight
# Last modified: 2020-01-31 11:01
# Email        : 824338670@qq.com
# Filename     : center_timers.h
# Description  : 
# =======================================================*/
#ifndef __TR_CENTER_TIMERS_H__
#define __TR_CENTER_TIMERS_H__

#include "tr_timer/wheel_timer.h"
#include "tr_common/base_type_def.h"

class CenterSecTimer : public TRTimer
{
public:
    CenterSecTimer(time_t run_mtime);
    ~CenterSecTimer();
    // 执行定时器逻辑
	virtual void RunLogic();
	// 返回下次执行的时间,如果返回0,表示不需要执行了
	virtual time_t NextTime();
};

#endif //__TR_CENTER_TIMERS_H__
