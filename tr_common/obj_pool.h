/********************************************************
# Author       : mknight
# Last modified: 2019-05-02 10:24
# Email        : 824338670@qq.com
# Filename     : obj_pool.h
# Description  : 
# *******************************************************/

#ifndef __OBJ_POOL_H__
#define __OBJ_POOL_H__
#include <list>

template <typename T>
class TObjPool
{
public:
    TObjPool()
    {
    }
    ~TObjPool()
    {
        for (auto it = working_objs_.begin(); it != working_objs_.end(); ++it)
        {
            delete (*it);
        }
        working_objs_.clear();
        for (auto it = idle_objs_.begin(); it != idle_objs_.end(); ++it)
        {
            delete (*it);
        }
        idle_objs_.clear();
    }
public:
    void Init(int32_t obj_num)
    {
        for (int32_t i = 0; i < obj_num; ++i)
        {
            T * pt = new T();
            if (pt)
            {
                idle_objs_.push_back(pt);
            }
        }
    }
    template <class obj_type>
    void InitEx(int32_t obj_num)
    {
        for (int32_t i = 0; i < obj_num; ++i)
        {
            obj_type * pt = new obj_type();
            if (pt)
            {
                idle_objs_.push_back(pt);
            }
        }
    }
    T* Pop()
    {
        if (idle_objs_.empty())
            return nullptr;
        auto t = idle_objs_.front();
        idle_objs_.pop_front();
        working_objs_.push_back(t);
        return t;
    }
    void Push(T *pt)
    {
        for (auto it = working_objs_.begin(); it != working_objs_.end();)
        {
            if ((*it) == pt)
            {
                working_objs_.erase(it);
                idle_objs_.push_back(pt);
                break;
            }
            else
                ++it;
        }
    }
    int32_t GetIdleSize()
    {
        return idle_objs_.size();
    }
    int32_t GetWorkingSize()
    {
        return working_objs_.size();
    }
protected:
    std::list<T*> idle_objs_;    // 空闲的obj
    std::list<T*> working_objs_;    // 使用中的obj
};


#endif  // __OBJ_POOL_H__