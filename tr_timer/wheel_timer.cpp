/* ======================================================
 * Author       : mknight
 * Last modified: 2019-08-18 13:55
 * Email        : 82433870@qq.com
 * Filename     : wheel_timer.cpp
 * Description  : 时间轮的定时器
 * ======================================================*/
#include "wheel_timer.h"

TRTimer::TRTimer(time_t run_time)
{
	run_time_ = run_time;
}

TRTimer::~TRTimer()
{
}

void TRTimer::Run(time_t cur_mtime)
{
	run_time_ = cur_mtime;
	RunLogic();
	last_run_time_ = cur_mtime;
	run_time_ = NextTime();
}

time_t TRTimer::NextTime()
{
	return 0;
}

time_t TRTimer::GetRunTime()
{
	return run_time_;
}

void TRTimer::RunLogic()
{
	// TODO:
}