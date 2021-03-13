#include "func_tools.h"
#include <string>
namespace FuncTools
{
bool IsNumStr(const std::string & str)
{
    if (str.size() < 1)
    {
        return false;
    }
    bool all_num = true;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] < '0' && str[i] > '9')
        {
            all_num = false;
            break;
        }
    }
    return all_num;
}

void SplitString(const char * src, size_t src_length, char ch_delim, std::vector<std::string> & ret_str)
{
    if (!src)
	{
		return;
	}
	const char * start = nullptr;
	size_t part_len = 0;
	for (size_t i = 0; i < src_length; ++i)
	{
		if ((*(src + i )) != ch_delim)
		{
			if (!start)
			{
				start = src + i;
			}
			part_len += 1;
		}
		else
		{
			if (start)
			{
				std::string sub_str(start, part_len);
				ret_str.push_back(sub_str);
			}
			part_len = 0;
			start = nullptr;
		}
	}
	if (start && part_len > 0)
	{
		std::string sub_str(start, part_len);
		ret_str.push_back(sub_str);
	}
	part_len = 0;
	start = nullptr;
}

int32_t RandomNum(int32_t begin, int32_t end)
{
	int32_t range = end - begin;
	if (range < 1)
	{
		range = 1;
	}
	return rand() % range + begin;
}
}

