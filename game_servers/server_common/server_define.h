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
	E_SERVER_TYPE_INVALID_SERVER = 0, 	// 无效的服务器类型
	E_SERVER_TYPE_LOGIN_SERVER = 1,		// 登录服务器
	E_SERVER_TYPE_LIST_SERVER = 2,		// 选服列表服务器
	E_SERVER_TYPE_INTERFACE_SERVER = 3,	// 对外平台接口服务器
	E_SERVER_TYPE_WORLD_SERVER = 4, 	// 世界服务器
	E_SERVER_TYPE_ROOT_SERVER = 5,		// 大区管理服务器
	E_SERVER_TYPE_CENTER_SERVER = 6,	// 区域中心服务器
	E_SERVER_TYPE_DATA_SERVER = 7,		// 区域数据服务器
	E_SERVER_TYPE_LOGIC_SERVER = 8,		// 游戏逻辑服务器
	E_SERVER_TYPE_GATE_SERVER = 9,		// 网关服务器
};


#endif // __GAME_SERVER_DEFINE_H__