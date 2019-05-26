/*=======================================================
# Author       : mknight
# Last modified: 2019-05-26 12:43
# Email        : 824338670@qq.com
# Filename     : server_time.cpp
# Description  : 
# =======================================================*/
#include "server_time.h"
#include <chrono>

ServerTime::ServerTime()
{
    UpdateTime();
}
ServerTime::~ServerTime()
{

}
// 更新一下时间记录,一般每帧更新一次
void ServerTime::UpdateTime(int64_t shift_time_sec/* = 0*/)
{
    time_sec_shift_ = shift_time_sec;
    using namespace std::chrono;
    system_clock::time_point tp = system_clock::now();
    system_clock::duration dtn = tp.time_since_epoch();

    time_usec_ = dtn.count() / TIME_NSEC_PER_USEC + TIME_USEC_PER_SEC * time_sec_shift_;
    time_msec_ = time_usec_ / TIME_USEC_PER_MSEC/* + time_sec_shift_ * TIME_MSEC_PER_SEC*/;
    time_sec_ = time_msec_ / TIME_MSEC_PER_SEC/* + time_sec_shift_*/;
}
// 当前时间戳,单位秒
time_t ServerTime::NowTime()
{
    return time_sec_;
}
// 当前时间戳,单位毫秒
time_t ServerTime::NowTimeMs()
{
    return time_msec_;
}

time_t ServerTime::NowTimeUs()
{
    return time_usec_;
}