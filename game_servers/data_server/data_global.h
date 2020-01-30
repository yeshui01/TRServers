/*=======================================================
# Author       : mknight
# Last modified: 2020-01-26 20:28
# Email        : 824338670@qq.com
# Filename     : data_global.h
# Description  : 
# =======================================================*/
#ifndef __TR_DATA_GLOBAL_H__
#define __TR_DATA_GLOBAL_H__
#include "tr_common/singleton.h"
#include "tr_database/database_helper.h"

class DataPlayer;

class DataGlobal : public TSingleton<DataGlobal>
{
public:
    DataGlobal();
    ~DataGlobal();

    bool Init();
    void Stop();

    // 初始化数据库
    bool InitDB();

    DataPlayer * GetDataPlayer(int64_t role_id);
    bool AddDataPlayer(int64_t role_id, DataPlayer * p);

    // 每帧更新
    void FrameUpdate(time_t cur_mtime);

public:
    TRDataBaseHelper db_helper_;    // 数据库管理助手

protected:
    std::map<int64_t, DataPlayer* > data_players_;  // key:role_id value:data_player
};

#define g_DataGlobal DataGlobal::Instance()

#endif // __TR_DATA_GLOBAL_H__
