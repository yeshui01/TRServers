/*=======================================================
# Author       : mknight
# Last modified: 2020-01-27 11:23
# Email        : 824338670@qq.com
# Filename     : logic_global.cpp
# Description  : 
# =======================================================*/
#include "logic_global.h"
#include "tr_log/log_module.h"
#include "tr_common/common_define.h"
#include "logic_player.h"
#include "logic_table_service.h"
LogicGlobal::LogicGlobal()
{

}

LogicGlobal::~LogicGlobal()
{
    for (auto it = logic_players_.begin(); it != logic_players_.end(); ++it)
    {
        SAFE_DELETE_PTR(it->second);
    }
}

bool LogicGlobal::Init()
{
    TINFO("LogicGLobal init");
    return true;
}

void LogicGlobal::Stop()
{

}

LogicPlayer * LogicGlobal::GetLogicPlayer(int64_t role_id)
{
    auto it = logic_players_.find(role_id);

    return it != logic_players_.end() ? it->second : nullptr;
}

bool LogicGlobal::AddLogicPlayer(int64_t role_id, LogicPlayer * p)
{
    if (nullptr == p)
    {
        return false;
    }
    auto ret = logic_players_.insert(std::make_pair(role_id, p));
    return ret.second;
}

void LogicGlobal::FrameUpdate(time_t mtime)
{

}

void LogicGlobal::SecUpdate(time_t cur_time)
{
    TDEBUG("logic global sec update:" << cur_time);
    g_LogicTableService.Update(cur_time);
}