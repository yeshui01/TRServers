/*=====================================================
# Author       : mknight
# Last modified: 2021-02-18 09:57
# Email        : 824338670@qq.com
# Filename     : server_buffer.cpp
# Description  : 
# =====================================================*/

#include "server_buffer.h"

ServerBufferElem::ServerBufferElem(int32_t need_size)
{
    buffer = new char[need_size];
    assert(buffer);
    buffer_size = need_size;
}

ServerBufferElem::~ServerBufferElem()
{
    Clean();
}

void ServerBufferElem::Clean()
{
    if (buffer)
    {
        delete [] buffer;
        buffer = nullptr;
        buffer_size = 0;
    }
}

ServerBuffer::ServerBuffer()
{

}

ServerBuffer::~ServerBuffer()
{
    if (msg_buffer_)
    {
       delete msg_buffer_;
       msg_buffer_ = nullptr;
    }
}

ServerBufferElem * ServerBuffer::HoldMsgBuffer(size_t apply_size)
{
    if (msg_buffer_)
    {
        if (msg_buffer_->buffer_size < apply_size)
        {
            delete msg_buffer_;
            msg_buffer_ = nullptr;
        }
        else
        {
            return msg_buffer_;
        }
    }
    // 按KB为单位
    int32_t block_size = apply_size % BUFFER_BLOCK_SIZE_1K;
    block_size = block_size > 0 ? (apply_size / BUFFER_BLOCK_SIZE_1K + 1) : (apply_size / BUFFER_BLOCK_SIZE_1K);
    block_size *= BUFFER_BLOCK_SIZE_1K;

    msg_buffer_ = new ServerBufferElem(block_size);
    return msg_buffer_;
}