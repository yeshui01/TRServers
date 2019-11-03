/*=======================================================
# Author       : mknight
# Last modified: 2019-08-18 11:50
# Email        : 824338670@qq.com
# Filename     : global_timer.cpp
# Description  : 
# =======================================================*/
#include "global_timer.h"

GlobalTimer::GlobalTimer()
{
    time_wheel_.reset(new TimeWheel());
}

GlobalTimer::~GlobalTimer()
{
}

void GlobalTimer::Init(time_t cur_mtime, int32_t wheel_size, int32_t cell_msec)
{
    time_wheel_->Init(cur_mtime, wheel_size, cell_msec);
}

 void GlobalTimer::Update(time_t cur_mtime)
 {
    time_wheel_->Update(cur_mtime);
 }

 // 添加定时器
void GlobalTimer::AddTimer(std::shared_ptr<TRTimer> todo_timer)
{
    time_wheel_->AddTimer(todo_timer);
}
// 删除定时器
void GlobalTimer::DeleteTimer(std::shared_ptr<TRTimer> todo_timer)
{
    time_wheel_->DeleteTimer(todo_timer);
}
// 获取当前timer数量
int32_t GlobalTimer::GetTimerSize()
{
    return time_wheel_->GetTimerSize();
}
