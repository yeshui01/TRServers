/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-29 21:16
 * Email        : 824338670@qq.com
 * Filename     : game_config.cpp
 * Description  : 
 * ======================================================*/

#include "server_config.h"
#include "json/json.h"
#include <fstream>
#include "log_module.h"
ServerConfig::ServerConfig()
{
}
ServerConfig::~ServerConfig()
{
}

bool ServerConfig::Load()
{
    Json::Reader reader;
    std::ifstream fs;
    fs.open("server_config.json", std::ios::binary);
    if (!fs.is_open())
    {
        TERROR("open server_config.json failed");
        return false;
    }
    if (!reader.parse(fs, js_config_, false))
    {
        fs.close();
        TERROR("parse server_config.json failed");
        return false;
    }
    fs.close();
    TDEBUG("parse server config sucess");
    
    if (js_config_["zone_id"].isNull())
    {
        TERROR("not found zone_id");
        return false;
    }
    if (js_config_["world_zone_id"].isNull())
    {
        TERROR("not found world_zone_id");
        return false;
    }
    if (js_config_["global_zone_id"].isNull())
    {
        TERROR("not found global_zone_id");
        return false;
    }
    zone_id_ = js_config_["zone_id"].asInt();
    world_zone_id_ = js_config_["zone_id"].asInt();
    global_zone_id_ = js_config_["global_zone_id"].asInt();
    TDEBUG("config zone_id:" << zone_id_);
    return true;
}

void ServerConfig::Clear()
{
    js_config_.clear();
}

const Json::Value & ServerConfig::GetJsonConfig()
{
    return js_config_;
}

int32_t ServerConfig::GetZoneId()
{
    return zone_id_;
}

int32_t ServerConfig::GetWorldZoneId()
{
    return world_zone_id_;
}

void ServerConfig::SetZoneId(int32_t zone_id)
{
    zone_id_ = zone_id;
}

// 获取全局区id
int32_t ServerConfig::GetGlobalZoneId()
{
    return global_zone_id_;
}