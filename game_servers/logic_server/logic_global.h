/*=======================================================
# Author       : mknight
# Last modified: 2020-01-27 11:22
# Email        : 824338670@qq.com
# Filename     : logic_global.h
# Description  : 
# =======================================================*/
#ifndef __TR_LOGIC_GLOBAL_H__
#define __TR_LOGIC_GLOBAL_H__
#include "tr_common/base_type_def.h"
#include "tr_common/singleton.h"
#include <map>
#include <utility>

class LogicPlayer;

class LogicGlobal : public TSingleton<LogicGlobal>
{
public:
    LogicGlobal();
    ~LogicGlobal();

    bool Init();
    void Stop();
    /**
     * 每帧更新
     * @param  mtime : 当前时间(毫秒)
     */
    void FrameUpdate(time_t mtime);
    
    /**
     * 每秒更新
     * @param  cur_time : 当前时间戳(秒) 
     */
    void SecUpdate(time_t cur_time);

    LogicPlayer * GetLogicPlayer(int64_t role_id);
    bool AddLogicPlayer(int64_t role_id, LogicPlayer * p);

protected:
    std::map<int64_t, LogicPlayer* > logic_players_;  // key:role_id value:logic_player
};

#define g_LogicGlobal LogicGlobal::Instance()
#endif // __TR_LOGIC_GLOBAL_H__
