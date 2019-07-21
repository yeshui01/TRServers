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
    */
   bool ForwardMessage(int32_t msg_class, int32_t msg_type, 
        const std::string & content, 
        EServerRouteNodeType node_type, 
        int32_t node_index);

   /*
    *  转发异步回调消息到某个服务器
    *  @param msg_class : 消息大类
    *  @param msg_type  : 消息类型
    *  @param content   : 消息类容
    *  @param callback  : 异步回调函数
    *  @param node_type : 服务器节点类型
    *  @oaram node_inde : 服务器节点类型下的索引
    */
   bool ForwardAsyncMessage(int32_t msg_class, int32_t msg_type, 
        const std::string & content, asyncmsg_callback_t && callback, 
        AsyncMsgParam && cb_param,
        EServerRouteNodeType node_type, 
        int32_t node_index);
        
    
    
    
    /**
     * 转发protobuf格式的异步回调消息
     * @param  msg_class  : 消息大类
     * @param  msg_type   : 消息小类
     * @param  pb_msg     : protobuf格式的消息
     * @param  callback   : 回调函数
     * @param  cb_param   : 函调函数相关参数
     * @param  node_type  : 节点类型
     * @param  node_index : 节点索引
     * @return {bool}     : 成功返回true,失败返回false
     */
    bool ForwardAsyncPbMessage(int32_t msg_class, int32_t msg_type, 
        ::google::protobuf::Message & pb_msg, asyncmsg_callback_t && callback, 
        AsyncMsgParam && cb_param, 
        EServerRouteNodeType node_type, 
        int32_t node_index);
protected:
    void EnsureMsgBuffer(int32_t buffer_size);
protected:
    char *msg_buffer_ = nullptr;
    int32_t buffer_size_ = 0;
};

#define g_MsgHelper GameMsgHelper::Instance()
#endif  // __TR_GAME_MSG_HELPER_H__