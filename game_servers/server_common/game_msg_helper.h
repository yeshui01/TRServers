/*=======================================================
# Author       : mknight
# Last modified: 2019-07-13 12:29
# Email        : 824338670@qq.com
# Filename     : game_msg_helper.h
# Description  : 
# =======================================================*/

#ifndef __TR_GAME_MSG_HELPER_H__
#define __TR_GAME_MSG_HELPER_H__
#include "game_server.h"
#include "server_define.h"
#include "tr_msg_packet/msg_tools.h"
#include "tr_common/singleton.h"

#include <google/protobuf/message.h>
class GameMsgHelper : public TSingleton<GameMsgHelper>
{
public:
    GameMsgHelper();
    ~GameMsgHelper();
    
public:
    /*
    *  转发消息到某个服务器
    *  @param msg_class : 消息大类
    *  @param msg_type  : 消息类型
    *  @param content   : 消息类容
    *  @param node_type : 服务器节点类型
    *  @oaram node_inde : 服务器节点类型下的索引
    *  @param zone_id   : 大区id,-1表示本区
    */
   bool ForwardMessage(int32_t msg_class, int32_t msg_type, 
        const std::string & content, 
        EServerRouteNodeType node_type, 
        int32_t node_index, int32_t zone_id = -1);

   /*
    *  转发异步回调消息到某个服务器
    *  @param msg_class : 消息大类
    *  @param msg_type  : 消息类型
    *  @param content   : 消息类容
    *  @param callback  : 异步回调函数
    *  @param node_type : 服务器节点类型
    *  @oaram node_inde : 服务器节点类型下的索引
    *  @param zone_id   : 大区id,-1表示本区
    */
   bool ForwardAsyncMessage(int32_t msg_class, int32_t msg_type, 
        const std::string & content, asyncmsg_callback_t && callback, 
        AsyncMsgParam && cb_param,
        EServerRouteNodeType node_type, 
        int32_t node_index, int32_t zone_id = -1);
        
    /**
     * 转发protobuf格式的异步回调消息
     * @param  msg_class  : 消息大类
     * @param  msg_type   : 消息小类
     * @param  pb_msg     : protobuf格式的消息
     * @param  callback   : 回调函数
     * @param  cb_param   : 函调函数相关参数
     * @param  node_type  : 节点类型
     * @param  node_index : 节点索引
     * @param zone_id     : 大区id,-1表示本区
     * @return {bool}     : 成功返回true,失败返回false
     */
    bool ForwardAsyncPbMessage(int32_t msg_class, int32_t msg_type, 
        ::google::protobuf::Message & pb_msg, asyncmsg_callback_t && callback, 
        AsyncMsgParam && cb_param, 
        EServerRouteNodeType node_type, 
        int32_t node_index, int32_t zone_id = -1);

    /**
    * 生成异步message上下文参数
    * @param  session_pt       : 
    * @param  message_pt       : 
    * @return {AsyncMsgParam}  : 
    */
	AsyncMsgParam GenAsyncMsgEnvParam(TConnection *session_pt, const NetMessage * message_pt);

    /**
     * 发送异步回复消息
     * @param  pb_msg      : 
     * @param  async_param : 
     */
    void SendAsyncRepMsg(::google::protobuf::Message & pb_msg, const AsyncMsgParam & async_param);

    /*
    *  转发消息到某个服务器
    *  @param msg_class : 消息大类
    *  @param msg_type  : 消息类型
    *  @param pb_msg    : protobuf格式的消息
    *  @param node_type : 服务器节点类型
    *  @oaram node_inde : 服务器节点类型下的索引
    *  @param zone_id   : 大区id,-1表示本区
    */
    bool ForwardPbMessage(int32_t msg_class, int32_t msg_type, 
        ::google::protobuf::Message & pb_msg, 
        EServerRouteNodeType node_type, 
        int32_t node_index, int32_t zone_id = -1);

    // 是否是世界节点
    bool IsWorldServerNode(EServerRouteNodeType node_type);
    // 是否是全局节点
    bool IsGlobalServerNode(EServerRouteNodeType node_type);
    // 是否是大区节点
    bool IsZoneServerNode(EServerRouteNodeType node_type);
protected:
    void EnsureMsgBuffer(int32_t buffer_size);
    
protected:
    char *msg_buffer_ = nullptr;
    int32_t buffer_size_ = 0;
    // 路由路径,key1:start_node key2:end_node value:中间的转发节点
    static std::map<EServerRouteNodeType, std::map<EServerRouteNodeType, std::vector<EServerRouteNodeType> > > s_route_path_;
};

#define g_MsgHelper GameMsgHelper::Instance()

// 异步消息宏定义,简化代码书写
#define TR_BEGIN_ASYNC_MSG_WITH_PARAM(MSG_CLASS, MSG_TYPE, pb_req, lambda_list) g_MsgHelper.ForwardAsyncPbMessage(INT_MSGCLASS(MSG_CLASS),\
    MSG_TYPE, pb_req,\
    lambda_list(const NetMessage *rep_msg, const AsyncMsgParam &cb_param){\
    REPMSG(MSG_TYPE) cb_rep;\
    STRING_TO_PBMSG(rep_msg->GetContent(), cb_rep);\
    TDEBUG("asyncmsg callback:rep_" << #MSG_TYPE << ":" << cb_rep.ShortDebugString());
    

#define TR_END_ASYNC_MSG_WITH_PARAM(ROUTE_NODE_TYPE, NODE_INDEX) },g_MsgHelper.GenAsyncMsgEnvParam(session_pt, messag_pt),\
EServerRouteNodeType::ROUTE_NODE_TYPE, NODE_INDEX);



#endif  // __TR_GAME_MSG_HELPER_H__