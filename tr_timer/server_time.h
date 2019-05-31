/*=======================================================
# Author       : mknight
# Last modified: 2019-05-26 12:43
# Email        : 824338670@qq.com
# Filename     : server_time.h
# Description  : 
# =======================================================*/
#ifndef __SERVER_TIME_H__
#define __SERVER_TIME_H__
#include "base_type_def.h"
#include "singleton.h"

// 每微秒多少纳秒
#define TIME_NSEC_PER_USEC  (1000)
// 每毫秒多少微秒
#define TIME_USEC_PER_MSEC  (1000)
// 每秒多少毫秒
#define TIME_MSEC_PER_SEC   (1000)
// 每秒多少微妙
#define TIME_USEC_PER_SEC   (1000000)

class ServerTime : public TSingleton<ServerTime>
{
public:
    ServerTime();
    ~ServerTime();
    // 更新一下时间记录,一般每帧更新一次
    void UpdateTime(int64_t shift_time_sec = 0);
    // 当前时间戳,单位秒
    time_t NowTime();
    // 当前时间戳,单位毫秒
    time_t NowTimeMs();
    // 当前时间错，单位微秒
    time_t NowTimeUs();
protected:
    time_t time_usec_ = 0;      // epoch时间 微妙
    time_t time_msec_ = 0;      // epoch时间 毫秒
    time_t time_sec_ = 0;       // epoch时间 秒
    int64_t time_sec_shift_ = 0;    // 时间偏移(秒) 调时间用
};

#define g_ServerTime ServerTime::Instance()

#endif // __SERVER_TIME_H__
