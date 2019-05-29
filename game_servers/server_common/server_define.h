/* ======================================================
 * Author       : tianyh(2216)
 * Last modified: 2019-05-29 21:29
 * Email        : tianyiheng@me-game.com
 * Filename     : server_define.h
 * Description  : 
 * ======================================================*/

#ifndef __GAME_SERVER_DEFINE_H__
#define __GAME_SERVER_DEFINE_H__
#include "tr_common/base_type_def.h"

// 服务器类型枚举
enum class EServerType : int32_t
{
	E_SERVER_TYPE_INVALID_SERVER = 0, // 无效的服务器类型
	E_SERVER_TYPE_LOGIN_SERVER = 1,	// 登录服务器
	E_SERVER_TYPE_WORLD_SERVER = 2, // 世界服务器
	E_SERVER_TYPE_ROOT_SERVER = 3,	// 大区管理服务器
	E_SERVER_TYPE_CENTER_SERVER = 4,	// 区域中心服务器
	E_SERVER_TYPE_DATA_SERVER = 5,	// 区域数据服务器
	E_SERVER_TYPE_LOGIC_SERVER = 6,	// 游戏逻辑服务器
	E_SERVER_TYPE_GATE_SERVER = 7,	// 网关服务器
};


#endif // __GAME_SERVER_DEFINE_H__