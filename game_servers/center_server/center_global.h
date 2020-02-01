/*=======================================================
# Author       : mknight
# Last modified: 2020-01-31 10:37
# Email        : 824338670@qq.com
# Filename     : center_global.h
# Description  : 
# =======================================================*/
#ifndef __TR_CENTER_GLOBAL_H__
#define __TR_CENTER_GLOBAL_H__

#include "tr_common/base_type_def.h"
#include "tr_common/singleton.h"
#include <map>
#include <utility>

class CenterPlayer;

class CenterGlobal : public TSingleton<CenterGlobal>
{
public:
    CenterGlobal();
    ~CenterGlobal();

    bool Init();
    void Stop();

    void InitTimer();

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

    CenterPlayer * GetCenterPlayer(int64_t role_id);
    CenterPlayer * CreateCenterPlayer(int64_t role_id);
protected:
    std::map<int64_t, CenterPlayer* > center_players_;  // key:role_id value:center_player
};

#define g_CenterGlobal CenterGlobal::Instance()


#endif // __TR_CENTER_GLOBAL_H__
