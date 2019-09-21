/*=======================================================
# Author       : mknight
# Last modified: 2019-08-18 11:49
# Email        : 824338670@qq.com
# Filename     : global_timer.h
# Description  : 
# =======================================================*/
#ifndef __GLOBAL_TIMER_H__
#define __GLOBAL_TIMER_H__
#include "base_type_def.h"
#include "time_wheel.h"
#include "singleton.h"

class GlobalTimer : public TSingleton<GlobalTimer>
{
public:
    GlobalTimer();
    ~GlobalTimer();
    void Init(time_t cur_mtime, int32_t wheel_size, int32_t cell_msec);
    void Update(time_t cur_mtime);
    // 添加定时器
	void AddTimer(std::shared_ptr<TRTimer> todo_timer);
	// 删除定时器
	void DeleteTimer(std::shared_ptr<TRTimer> todo_timer);
	// 获取当前timer数量
	int32_t GetTimerSize();
protected:
    std::shared_ptr<TimeWheel> time_wheel_;
};

#define g_GlobalTimer GlobalTimer::Instance();

#endif // __GLOBAL_TIMER_H__
