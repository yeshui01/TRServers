/*=======================================================
# Author       : mknight
# Last modified: 2019-11-03 16:16
# Email        : 824338670@qq.com
# Filename     : server_describe.h
# Description  : 用于debug查看日志等
# =======================================================*/

#include "tr_common/singleton.h"
#include <map>
#include <string>
#include "tr_common/base_type_def.h"
#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_common/singleton.h"
#include "server_define.h"
#include <string>
#include "json/json.h"

#ifndef __TR_SERVER_DESCRIBE_H__
#define __TR_SERVER_DESCRIBE_H__

class ServerDes : public TSingleton<ServerDes>
{
public:
    ServerDes();
    ~ServerDes();
    void Load();
    std::string GetServerNodeName(int32_t route_node_type);
    std::string GetMsgClassName(int32_t msg_class);
    std::string GetMsgTypeName(int32_t msg_class, int32_t msg_type);

protected:
    std::map<int32_t, std::string> node_type_des_;  // key:node_type value:node_name
    std::map<int32_t, std::string> msg_class_des_;  // key:msg_class value:msg_class_name
    std::map<int32_t, std::map<int32_t, std::string> > msg_type_des_; // key1: msg_class key2:msg_type  vlaue:msg_type_name
};

#define g_ServerDes ServerDes::Instance()

#endif // __TR_SERVER_DESCRIBE_H__