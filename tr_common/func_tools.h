/*=======================================================
# Author       : mknight
# Last modified: 2019-05-11 06:33
# Email        : 824338670@qq.com
# Filename     : func_tools.h
# Description  : 
# =======================================================*/

#ifndef __FUNC_TOOLS_H__
#define __FUNC_TOOLS_H__
#include <limits>

namespace FuncTools
{

/* 两数相加求和
 * @param T : 只支持 int32_t 或者 int64_t
 * @param x1 : param 1
 * @param x2 : param2
 * @return : x1 + x2 final result
 */
template <typename T>
T SafeAdd(T x1, T x2)
{
    if (x1 < 0 && x2 < 0)
    {
        if (x1 < std::numeric_limits<T>::min() - x2)
        {
            return std::numeric_limits<T>::min();
        }
    }
    else if (x1 > 0 && x2 > 0)
    {
        if (x1 > std::numeric_limits<T>::max() - x2)
        {
            return std::numeric_limits<T>::max();
        }
    }
    return x1 + x2;
};

/* 两数相加求差
 * @param T : 只支持 int32_t 或者 int64_t
 * @param x1 : param 1
 * @param x2 : param2
 * @return : x1 - x2 final result
 */
template <typename T>
T SafeSub(T x1, T x2)
{
    if (x1 < 0 && x2 > 0)
    {
        if ((x1 * (-1)) > std::numeric_limits<T>::max() - x2)
        {
            return std::numeric_limits<T>::min();
        }
    }
    else if (x1 > 0 && x2 < 0)
    {
        if ((-1)*x2 > std::numeric_limits<T>::max() - x1)
        {
            return std::numeric_limits<T>::max();
        }
    }
    return x1 - x2;
}

}   // namespace FuncTools end


#endif  // __FUNC_TOOLS_H__

