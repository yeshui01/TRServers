/* ======================================================
 * Author       : tianyh(2216)
 * Last modified: 2019-05-29 21:29
 * Email        : 824338670@qq.com
 * Filename     : server_define.h
 * Description  : 
 * ======================================================*/

#ifndef __GAME_SERVER_DEFINE_H__
#define __GAME_SERVER_DEFINE_H__
#include "tr_common/base_type_def.h"

// 服务器类型枚举
enum class EServerType : uint8_t
{
	E_SERVER_TYPE_INVALID_SERVER = 0, 	// 无效的服务器类型
	
	E_SERVER_TYPE_INTERFACE_SERVER = 1,	// 对外平台接口服务器
	E_SERVER_TYPE_ROOT_SERVER = 2,		// 大区管理服务器
	E_SERVER_TYPE_CENTER_SERVER = 3,	// 区域中心服务器
	E_SERVER_TYPE_DATA_SERVER = 4,		// 区域数据服务器
	E_SERVER_TYPE_LOG_SERVER = 5,		// 区域日志服务器
	E_SERVER_TYPE_LOGIC_SERVER = 6,		// 逻辑服务器
	E_SERVER_TYPE_GATE_SERVER = 7,		// 网关服务器

	E_SERVER_TYPE_WORLD_SERVER = 100, 	// 世界服务器
	E_SERVER_TYPE_WORLD_CENTER = 101,	// 世界中心逻辑服
	E_SERVER_TYPE_SCENE_SERVER = 102,	// 场景服务器

	E_SERVER_TYPE_LOGIN_SERVER = 150,	// 登录服务器
	E_SERVER_TYPE_LIST_SERVER = 151,	// 选服列表服务器
};

// 服务器路由节点类型
enum class EServerRouteNodeType : uint8_t
{
	E_SERVER_ROUTE_NODE_NONE = 0,
	
	E_SERVER_ROUTE_NODE_INTERFACE = 1,		// 平台接口
	E_SERVER_ROUTE_NODE_ROOT = 2,			// 大区管理
	E_SERVER_ROUTE_NODE_CENTER = 3,			// 区域中心
	E_SERVER_ROUTE_NODE_DATA = 4,			// 数据服务
	E_SERVER_ROUTE_NODE_LOG = 5,			// 日志服务
	E_SERVER_ROUTE_NODE_LOGIC = 6,			// 逻辑
	E_SERVER_ROUTE_NODE_GATE = 7,			// 网关

	E_SERVER_ROUTE_NODE_WORLD = 100,		// 世界服
	E_SERVER_ROUTE_NODE_WORLD_CENTER = 101,	// 世界逻辑
	E_SERVER_ROUTE_NODE_SCENE = 102,		// 场景服务器

	E_SERVER_ROUTE_NODE_LOGIN = 150,		// 登录服
	E_SERVER_ROUTE_NODE_LIST = 151,			// 列表服
};

// //////////////////////////////////////////////////////////////////////////////////////////
#define INT_SERVERTYPE(server_type) int32_t(EServerType::server_type)
#define INT_NODETYPE(node_type) int32_t(EServerRouteNodeType::node_type)

// pb结构转成str
#define PBMSG_TO_STR(pb_msg, msg_str) std::string msg_str; \
pb_msg.SerializeToString(&msg_str);

// pb从str反序列化
#define STRING_TO_PBMSG(str, pb_msg) pb_msg.ParseFromString(str)

// proto消息基本类型
#define PROTO_BASEMSG ::google::protobuf::Message

// msg_class转int32_t
#define INT_MSGCLASS(msg_class) int32_t(EProtocolClass::msg_class)

#endif // __GAME_SERVER_DEFINE_H__