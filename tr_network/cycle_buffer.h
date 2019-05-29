/********************************************************
# Author       : mknight
# Last modified: 2019-05-01 14:26
# Email        : 824338670@qq.com
# Filename     : cycle_buffer.h
# Description  : 
# *******************************************************/

#ifndef __CYCLE_BUFFER_H__
#define __CYCLE_BUFFER_H__
#include <memory.h>
#include <cstdint>
#include <string>
#include <vector>
// 环形字节缓冲区
template<typename elem_type>
class CycleBuffer
{
public:
    CycleBuffer() = delete;
    CycleBuffer(int32_t init_size)
    {
        Reset();
        v_buffer_.resize(init_size);
        total_space_ = init_size;
        init_space_ = init_size;
    }
    
    ~CycleBuffer()
    {
    }

    void Reset()
    {
        v_buffer_.clear();
        v_buffer_.resize(init_space_);
        read_index_ = 0;
        write_index_ = 0;
        data_size_ = 0;
        limit_grow_ = false;
        total_space_ = init_space_;
    }
    // 获取当前容量
    int32_t GetSpace()
    {
        return total_space_;
    }
    // 获取剩余空间
    int32_t GetRestSpace()
    {
        return total_space_ - data_size_;
    }
    // 查看当前数据大小
    int32_t PeekDataSize()
    {
        return data_size_;
    }
    // 当前读索引
    int32_t GetReadIndex()
    {
        return read_index_;
    }
    // 获取写索引
    int32_t GetWriteIndex()
    {
        return write_index_;
    }
    void SetReadIndex(int32_t index)
    {
        read_index_ = index;
    }
    void SetWriteIndex(int32_t index)
    {
        write_index_ = index;
    }
    // 增长空间,返回剩余空间
    int32_t GrowSpace(int32_t grow_size)
    {
        if (grow_size <= 0)
        {
            return total_space_ - data_size_;
        }
        if (total_space_ + grow_size > v_buffer_.size())
        {
            std::vector<elem_type> copy_buffer;
            copy_buffer.resize(total_space_ + grow_size);
            int32_t cur_data_size = PeekDataSize();
            ReadData(&copy_buffer[0], cur_data_size);
            
            v_buffer_.swap(copy_buffer);
            total_space_ += grow_size;
            read_index_ = 0;
            write_index_ = cur_data_size;
            data_size_ = cur_data_size;
        }
        return total_space_ - data_size_;
    }
    // 放入数据到, 返回实际放入的大小
    int32_t WriteData(const elem_type * data_pt, int32_t to_write_size)
    {
        if (!data_pt)
            return 0;
        int32_t writed_size = 0;
        if (GetRestSpace() < to_write_size)
        {
            if (!limit_grow_ && GetSpace() < MAX_SOCKET_BUFFER_SIZE)
            {
                GrowSpace(init_space_);
            }
            else 
            {
                to_write_size = GetRestSpace() >= to_write_size ? to_write_size : GetRestSpace();
            }
        }
        if (to_write_size < 1)
            return 0;
        if (write_index_ == read_index_ && GetRestSpace() == 0)
        {
            // 数据满了，不能再写了
            return 0;
        }
        if (GetRestSpace() >= to_write_size)
        {
            // 可以全部放入
            // if (write_index_ == read_index_)
            // {
            //     // if (0 == write_index_)
            //     // {
            //     //     // 首次开始
            //     //     memcpy(&v_buffer_[write_index_], data_pt, sizeof(elem_type) * to_write_size);
            //     //     write_index_ += to_write_size;
            //     //     writed_size = to_write_size;
            //     // }
            //     // else 
            //     // {
            //     //     int32_t right_data_size = (v_buffer_.size() - write_index_) >= to_write_size ? to_write_size : (v_buffer_.size() - write_index_);
            //     //     memcpy(&v_buffer_[write_index_], data_pt, sizeof(elem_type) * right_data_size);
            //     //     write_index_ += right_data_size;
            //     //     if (write_index_ >= v_buffer_.size())
            //     //         write_index_ %= v_buffer_.size();
                
            //     //     if (to_write_size - right_data_size > 0)
            //     //     {
            //     //         memcpy(&v_buffer_[write_index_], data_pt + right_data_size, sizeof(elem_type) * (to_write_size - right_data_size));
            //     //         write_index_ += (to_write_size - right_data_size);
            //     //     }
            //     // }  
            //     int32_t right_data_size = (v_buffer_.size() - write_index_) >= to_write_size ? to_write_size : (v_buffer_.size() - write_index_);
            //     memcpy(&v_buffer_[write_index_], data_pt, sizeof(elem_type) * right_data_size);
            //     write_index_ += right_data_size;
            //     if (write_index_ >= v_buffer_.size())
            //         write_index_ %= v_buffer_.size();
                
            //     if (to_write_size - right_data_size > 0)
            //     {
            //         memcpy(&v_buffer_[write_index_], data_pt + right_data_size, sizeof(elem_type) * (to_write_size - right_data_size));
            //         write_index_ += (to_write_size - right_data_size);
            //     }  
            // }
            // else if (write_index_ > read_index_)
            // {
            //     int32_t right_data_size = (v_buffer_.size() - write_index_) >= to_write_size ? to_write_size : (v_buffer_.size() - write_index_);
            //     memcpy(&v_buffer_[write_index_], data_pt, sizeof(elem_type) * right_data_size);
            //     write_index_ += right_data_size;
            //     if (write_index_ >= v_buffer_.size())
            //         write_index_ %= v_buffer_.size();
                
            //     if (to_write_size - right_data_size > 0)
            //     {
            //         memcpy(&v_buffer_[write_index_], data_pt + right_data_size, sizeof(elem_type) * (to_write_size - right_data_size));
            //         write_index_ += (to_write_size - right_data_size);
            //     }
            // }
            
            int32_t right_data_size = (v_buffer_.size() - write_index_) >= to_write_size ? to_write_size : (v_buffer_.size() - write_index_);
            memcpy(&v_buffer_[write_index_], data_pt, sizeof(elem_type) * right_data_size);
            write_index_ += right_data_size;
            if (write_index_ >= v_buffer_.size())
                write_index_ %= v_buffer_.size();
                
            if (to_write_size - right_data_size > 0)
            {
                memcpy(&v_buffer_[write_index_], data_pt + right_data_size, sizeof(elem_type) * (to_write_size - right_data_size));
                write_index_ += (to_write_size - right_data_size);
            }  

            writed_size = to_write_size;
        }
        data_size_ += writed_size;
        if (write_index_ >= v_buffer_.size())
        {
            write_index_ %= v_buffer_.size();
        }
        return writed_size;
    }

    // 读取数据
    int32_t ReadData(elem_type * data_buffer, int32_t to_read_size, bool peek = false)
    {
        if (!data_buffer)
            return 0;

        if (read_index_ == write_index_ && data_size_ < 1)
            return 0;
        int32_t bk_read_index_ = read_index_;
        int32_t readed_size = 0;
        int32_t max_read_size = data_size_ >= to_read_size ? to_read_size : data_size_;

        if (write_index_ > read_index_)
        {
            int32_t right_data_size = (write_index_ - read_index_ >= max_read_size) ? max_read_size : (write_index_ - read_index_);
            // 直接读取
            memcpy(data_buffer, &v_buffer_[read_index_], sizeof(elem_type) * right_data_size);
            read_index_ += right_data_size;
            if (read_index_ >= v_buffer_.size())
                read_index_ %= v_buffer_.size();

            readed_size += right_data_size;
        }
        else if (read_index_ > write_index_)
        {
            int32_t right_data_size = (v_buffer_.size() - read_index_ >= max_read_size) ? max_read_size : (v_buffer_.size() - read_index_);
            memcpy(data_buffer, &v_buffer_[read_index_], sizeof(elem_type) * right_data_size);
            read_index_ += right_data_size;
            if (read_index_ >= v_buffer_.size())
                read_index_ %= v_buffer_.size();
            readed_size += right_data_size;

            if (readed_size < max_read_size)
            {
                // 继续读
                memcpy(data_buffer + readed_size, &v_buffer_[read_index_], sizeof(elem_type) * (max_read_size - readed_size));
                read_index_ += (max_read_size - readed_size);
                readed_size = max_read_size;
            }
            if (read_index_ >= v_buffer_.size())
                read_index_ %= v_buffer_.size();
        }
        else if (read_index_ == write_index_)
        {
            // 直接读取
            // if (0 == read_index_)
            // {
            //     memcpy(data_buffer, &v_buffer_[read_index_], sizeof(elem_type) * max_read_size);
            //     read_index_ += max_read_size;
            //     if (read_index_ >= v_buffer_.size())
            //         read_index_ %= v_buffer_.size();

            //     readed_size += max_read_size;
            // }
            // else 
            // {
            //     int32_t right_data_size = (v_buffer_.size() - read_index_ >= max_read_size) ? max_read_size : (v_buffer_.size() - read_index_);
            //     memcpy(data_buffer, &v_buffer_[read_index_], sizeof(elem_type) * right_data_size);
            //     read_index_ += right_data_size;
            //     if (read_index_ >= v_buffer_.size())
            //         read_index_ %= v_buffer_.size();
            //     readed_size += right_data_size;

            //     if (readed_size < max_read_size)
            //     {
            //         // 继续读
            //         memcpy(data_buffer + readed_size, &v_buffer_[read_index_], sizeof(elem_type) * (max_read_size - readed_size));
            //         read_index_ += (max_read_size - readed_size);
            //         readed_size = max_read_size;
            //     }
            // }

            int32_t right_data_size = (v_buffer_.size() - read_index_ >= max_read_size) ? max_read_size : (v_buffer_.size() - read_index_);
            memcpy(data_buffer, &v_buffer_[read_index_], sizeof(elem_type) * right_data_size);
            read_index_ += right_data_size;
            if (read_index_ >= v_buffer_.size())
                read_index_ %= v_buffer_.size();
            readed_size += right_data_size;

            if (readed_size < max_read_size)
            {
                // 继续读
                memcpy(data_buffer + readed_size, &v_buffer_[read_index_], sizeof(elem_type) * (max_read_size - readed_size));
                read_index_ += (max_read_size - readed_size);
                readed_size = max_read_size;
            }
        }
        if (read_index_ >= v_buffer_.size())
            read_index_ %= v_buffer_.size();

        if (!peek)
            data_size_ -= readed_size;
        if (peek)
            read_index_ = bk_read_index_;

        return readed_size;
    }

protected:
    std::vector<elem_type> v_buffer_;
    int32_t read_index_ = 0;
    int32_t write_index_ = 0;
    int32_t total_space_ = 0;   // 总空间大小
    int32_t data_size_ = 0;     // 当前数据大小
    int32_t limit_grow_ = false;    // 是否限制增长
    int32_t init_space_ = 1;
};


#endif // __CYCLE_BUFFER_H__