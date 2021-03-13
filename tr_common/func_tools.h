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
#include <string>
#include <vector>
namespace FuncTools
{

/* 两数相加求和
 * @param T : 只支持 int32_t 或者 int64_t等数字类型
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

/* 两数相减求差
 * @param T : 只支持 int32_t 或者 int64_t等数字类型
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

/* 获取map的值的指针
 *  @param dic_value : 要获取的map的对象
 *  @param key       : 要所以的键
 *  @return          : 值的指针，如果没有返回nullptr
 */
template <typename map_type>
    typename map_type::mapped_type * GetMapValue(map_type & dic_value, typename map_type::key_type key)
    {
        auto it = dic_value.find(key);
        if (it != dic_value.end())
        {
            return &it->second;
        }
        return nullptr;
    }
/**
 * 判断字符串是否为数字字符串
 * @param  {std::string} str : 
 * @return {bool}            : 
 */
bool IsNumStr(const std::string & str);

/**
 * 分割字符串
 * @param  {char*} src                        : 源字符串
 * @param  {size_t} src_length                : 远字符串长度
 * @param  {char} ch_delim                    : 分割字符
 * @param  {std::vector<std::string>} ret_str : 结果输出
 */
void SplitString(const char * src, size_t src_length, char ch_delim, std::vector<std::string> & ret_str);
// 左闭右开区间
int32_t RandomNum(int32_t begin, int32_t end);
}   // namespace FuncTools end


#endif  // __FUNC_TOOLS_H__

