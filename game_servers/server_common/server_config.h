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
protected:
	int32_t zone_id_ = 0;

	// json配置数据缓存
	Json::Value js_config_;
};

#define g_ServerConfig ServerConfig::Instance()

#endif // __TR_GAME_CONFIG_H__