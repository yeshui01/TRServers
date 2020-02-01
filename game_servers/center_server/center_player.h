/*=======================================================
# Author       : mknight
# Last modified: 2020-01-31 10:39
# Email        : 824338670@qq.com
# Filename     : center_player.h
# Description  : 
# =======================================================*/
#ifndef __TR_CENTER_PLAYER_H__
#define __TR_CENTER_PLAYER_H__
#include "tr_common/base_type_def.h"
class CenterPlayer
{
public:
	CenterPlayer(int64_t role_id);
	~CenterPlayer();

protected:
	int64_t role_id_ = 0;
};

#endif // __TR_CENTER_PLAYER_H__
