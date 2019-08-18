/* ======================================================
 * Author       : mknight
 * Last modified: 2019-08-18 13:55
 * Email        : 82433870@qq.com
 * Filename     : wheel_timer.h
 * Description  : 时间轮定时器
 * ======================================================*/
#ifndef __TR_TIMER_H__
#define __TR_TIMER_H__

#include <cstdint>
// 定时器基类,时间单位：毫秒
class TRTimer
{
public:
	TRTimer(time_t run_time);
	virtual ~TRTimer();
	// 运行定时器
	void Run(time_t cur_mtime);
	// 执行定时器逻辑
	virtual void RunLogic();
	// 返回下次执行的时间,如果返回0,表示不需要执行了
	virtual time_t NextTime();
	// 获取执行的时间
	time_t GetRunTime();
protected:
	time_t last_run_time_ = 0;
	time_t run_time_ = 0;	// 执行时间(单位:毫秒)
};

#endif // __TR_TIMER_H__