/*=====================================================
# Author       : mknight
# Last modified: 2021-02-18 09:57
# Email        : 824338670@qq.com
# Filename     : server_buffer.h
# Description  : 
# =====================================================*/

#ifndef __TR_SERVER_BUFFER_H__
#define __TR_SERVER_BUFFER_H__

#define BUFFER_BLOCK_SIZE_1K    1024
#define BUFFER_BLOCK_SIZE_2K    2048
#define BUFFER_BLOCK_SIZE_4k    4096
#define BUFFER_BLOCK_SIZE_8k    8192
#define BUFFER_BLOCK_SIZE_16k   16384

#include "tr_common/base_type_def.h"
#include "tr_common/common_define.h"
#include "tr_common/singleton.h"
#include <cassert>

struct ServerBufferElem
{
    ServerBufferElem() = delete;
    ServerBufferElem(const ServerBufferElem & r_buffer) = delete;
    ServerBufferElem(ServerBufferElem && rr_buffer) = delete;
    ServerBufferElem & operator = (const ServerBufferElem & r_buffer) = delete;

    ServerBufferElem(int32_t need_size);
    ~ServerBufferElem();
    void Clean();

    // data
    char * buffer = nullptr;
    int32_t buffer_size = 0;
};

class ServerBuffer : public TSingleton<ServerBuffer>
{
public:
    ServerBuffer();
    ~ServerBuffer();
public:
    ServerBufferElem * HoldMsgBuffer(size_t apply_size);

protected:
    ServerBufferElem * msg_buffer_ = nullptr;
};


#define g_ServerBuffer ServerBuffer::Instance()

#endif // __TR_SERVER_BUFFER_H__