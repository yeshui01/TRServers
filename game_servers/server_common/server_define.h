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
enum class EServerType : int8_t
{
	E_SERVER_TYPE_INVALID_SERVER = 0, 	// 无效的服务器类型
	E_SERVER_TYPE_LOGIN_SERVER = 1,		// 登录服务器
	E_SERVER_TYPE_LIST_SERVER = 2,		// 选服列表服务器
	E_SERVER_TYPE_INTERFACE_SERVER = 3,	// 对外平台接口服务器
	E_SERVER_TYPE_WORLD_SERVER = 4, 	// 世界服务器
	E_SERVER_TYPE_ROOT_SERVER = 5,		// 大区管理服务器
	E_SERVER_TYPE_CENTER_SERVER = 6,	// 区域中心服务器
	E_SERVER_TYPE_DATA_SERVER = 7,		// 区域数据服务器
	E_SERVER_TYPE_LOG_SERVER = 8,		// 区域日志服务器
	E_SERVER_TYPE_LOGIC_SERVER = 9,		// 逻辑服务器
	E_SERVER_TYPE_GATE_SERVER = 10,		// 网关服务器
};

// 服务器路由节点类型
enum class EServerRouteNodeType : int8_t
{
	E_SERVER_ROUTE_NODE_NONE = 0,
	E_SERVER_ROUTE_NODE_LOGIN = 1,		// 登录
	E_SERVER_ROUTE_NODE_LIST = 2,		// 列表
	E_SERVER_ROUTE_NODE_INTERFACE = 3,	// 平台接口
	E_SERVER_ROUTE_NODE_WORLD = 4,		// 世界
	E_SERVER_ROUTE_NODE_ROOT = 5,		// 大区管理
	E_SERVER_ROUTE_NODE_CENTER = 6,		// 区域中心
	E_SERVER_ROUTE_NODE_DATA = 7,		// 数据服务
	E_SERVER_ROUTE_NODE_LOGIC = 8,		// 逻辑
	E_SERVER_ROUTE_NODE_WORLD_LOGIC = 9,// 世界逻辑
	E_SERVER_ROUTE_NODE_SCENE = 10,		// 场景服务器
	E_SERVER_ROUTE_NODE_GATE = 11,		//  网关
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