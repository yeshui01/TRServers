/*=======================================================
# Author       : mknight
# Last modified: 2020-01-31 10:37
# Email        : 824338670@qq.com
# Filename     : center_global.cpp
# Description  : 
# =======================================================*/
#include "center_global.h"
#include "tr_log/log_module.h"
#include "tr_common/common_define.h"
#include "center_player.h"
#include "tr_timer/global_timer.h"
#include "center_timers.h"
#include "tr_timer/server_time.h"
// #include "logic_table_service.h"
CenterGlobal::CenterGlobal()
{

}

CenterGlobal::~CenterGlobal()
{
    for (auto it = center_players_.begin(); it != center_players_.end(); ++it)
    {
        SAFE_DELETE_PTR(it->second);
    }
    center_players_.clear();
}

bool CenterGlobal::Init()
{
    InitTimer();
    return true;
}

void CenterGlobal::Stop()
{

}

void CenterGlobal::FrameUpdate(time_t mtime)
{

}
void CenterGlobal::SecUpdate(time_t cur_time)
{

}

CenterPlayer * CenterGlobal::GetCenterPlayer(int64_t role_id)
{
    auto it = center_players_.find(role_id);
    return it != center_players_.end() ? it->second : nullptr;
}

CenterPlayer * CenterGlobal::CreateCenterPlayer(int64_t role_id)
{
    auto it = center_players_.find(role_id);
    if (it != center_players_.end())
    {
        return it->second;
    }
    CenterPlayer *p = new CenterPlayer(role_id);
    if (!p)
    {
        TERROR("MAY BE memory error");
        return nullptr;
    }
    center_players_.insert({role_id, p});
    return p;
}

void CenterGlobal::InitTimer()
{
    // 定时器初始化
    g_GlobalTimer.Init(g_ServerTime.NowTimeMs(), 100, 100); // 一轮10秒
    {
        std::shared_ptr<CenterSecTimer> sec_timer(new CenterSecTimer(g_ServerTime.NowTimeMs() + 5000));
        g_GlobalTimer.AddTimer(sec_timer);
    }
}