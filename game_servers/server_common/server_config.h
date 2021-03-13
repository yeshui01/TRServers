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
#include <string>
#include "json/json.h"


class ServerConfig : public TSingleton<ServerConfig>
{
public:
	ServerConfig();
	~ServerConfig();

public:
	// 加载服务器配置数据
	bool Load();
	// 清理配置数据
	void Clear();
	// 获取json配置
	const Json::Value & GetJsonConfig();
	// 获取大区id
	int32_t GetZoneId();
	// 获取世界的大区id标识
	int32_t GetWorldZoneId();
	// 动态修改zoneid
	void SetZoneId(int32_t zone_id);
	// 获取全局区id
	int32_t GetGlobalZoneId();
	// 是否是调试玩家
	bool IsDebugUser(int64_t user_id);
protected:
	int32_t zone_id_ = 0;
	int32_t world_zone_id_ = 0;
	int32_t global_zone_id_ = 0;
	int64_t debug_user_ = 0;
	int32_t debug_mode_ = 0;
	// json配置数据缓存
	Json::Value js_config_;
};

#define g_ServerConfig ServerConfig::Instance()

#endif // __TR_GAME_CONFIG_H__