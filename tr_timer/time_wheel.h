/* ======================================================
 * Author       : mknight
 * Last modified: 2019-03-04 13:55
 * Email        : 82433870@qq.com
 * Filename     : time_wheel.h
 * Description  : 时间轮
 * ======================================================*/
#ifndef __TR_TIME_WHEEL_H__
#define __TR_TIME_WHEEL_H__
#include "tr_common/base_type_def.h"

// 定时器每帧最小时间(毫秒)
#define TR_TIMER_FRAME_LIMIT_MSEC	(25)

class TRTimer;

struct TimeWhellNode
{
	int64_t round = 0;		// 需要执行时的轮次
	std::shared_ptr<TRTimer> timer_ptr;
};

struct TimeWheelCell
{
	std::list<TimeWhellNode> timer_list;
};


class TimeWheel
{
public:
	TimeWheel();
	~TimeWheel();
	// 初始化时间轮的大小
	bool Init(time_t cur_mtime, int32_t wheel_size, int32_t cell_msec);
	// 每帧更新
	void Update(time_t mtime);
	// 添加定时器
	void AddTimer(std::shared_ptr<TRTimer> todo_timer);
	// 删除定时器
	void DeleteTimer(std::shared_ptr<TRTimer> todo_timer);
	// 获取当前timer数量
	int32_t GetTimerSize();
protected:
	void RealAddTimer(std::shared_ptr<TRTimer> todo_timer);
protected:
	// 总大小
	int32_t wheel_size_ = 0;
	// 当前轮次
	int64_t wheel_round_ = 0;
	// 当前cell
	int32_t cell_index_ = 0;
	// 每个cell的时间(毫秒)
	int32_t cell_msec_ = 100;
	// 最近一次更新时间(毫秒)
	time_t last_update_mtime_ = 0;
	// 最近一次更新一个cell的时间
	time_t last_cell_mtime_ = 0;
	// 定时器容器
	std::vector<TimeWheelCell> time_cell_;
	// 待添加的队列
	std::multimap<time_t, std::shared_ptr<TRTimer> > todo_timers_;
	// 待删除的队列
	std::list<std::shared_ptr<TRTimer> > to_delete_timers_;
	// 定时器记录索引
	std::map<TRTimer*, int32_t> timer_cell_index_;	// key:timer_ptr value:cell_index
};



#endif // __TR_TIME_WHEEL_H__ end
