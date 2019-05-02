/* ======================================================
 * Author       : mknight
 * Last modified: 2019-03-04 13:55
 * Email        : 82433870@qq.com
 * Filename     : time_wheel.h
 * Description  : 时间轮
 * ======================================================*/
#ifndef __TR_TIME_WHEELH__
#define __TR_TIME_WHEELH__
#include "tr_common/base_type_def.h"

#include <chrono>
#include <ratio>

class TRTimer;

class TimeWheel
{
public:
	explicit TimeWheel(size_t wheel_size, int32_t cell_sec);
	~TimeWheel();
	
	struct WheelCell
	{
									
	};


	void Update(time_t cur_time);
protected:
  	// 时间轮单元数
 	size_t wheel_size_;	
	// 每个单元的切换时间(秒)
	int32_t cell_sec_;
	// 当前轮数
	int64_t cur_round_ = 0;
	
};


#endif // __TR_TIME_WHEELH__ end
