/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-29 21:16
 * Email        : 824338670@qq.com
 * Filename     : game_config.h
 * Description  : 
 * ======================================================*/

#ifndef __TR_GAME_CONFIG_H__
#define __TR_GAME_CONFIG_H__
#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_common/singleton.h"
#include "server_define.h"
class GameConfig : public TSingleton<GameConfig>
{
public:
	GameConfig();
	~GameConfig();

	void Load();

};

#define g_GameConfig GameConfig::Instance()

#endif // __TR_GAME_CONFIG_H__