/* ======================================================
 * Author       : mknight
 * Last modified: 2019-03-04 13:55
 * Email        : 82433870@qq.com
 * Filename     : time_wheel.cpp
 * Description  : 时间轮
 * ======================================================*/
#include "tr_timer/time_wheel.h"
#include "wheel_timer.h"
#include <map>
// #include <iostream>

TimeWheel::TimeWheel()
{

}

TimeWheel::~TimeWheel()
{

}

bool TimeWheel::Init(time_t cur_mtime, int32_t wheel_size, int32_t cell_msec)
{
	if (cur_mtime <= 0 || wheel_size < 1)
	{
		return false;
	}
	wheel_size_ = wheel_size;
	if (cell_msec < TR_TIMER_FRAME_LIMIT_MSEC)
	{
		cell_msec = TR_TIMER_FRAME_LIMIT_MSEC;
	}
	cell_msec_ = cell_msec;
	for (auto i = 0; i < wheel_size; ++i)
	{
		time_cell_.emplace_back(TimeWheelCell());
	}
	last_update_mtime_ = cur_mtime;
	return true;
}

void TimeWheel::Update(time_t mtime)
{
	if (time_cell_.empty())
		return;
	if (mtime - last_update_mtime_ < TR_TIMER_FRAME_LIMIT_MSEC)
	{
		// 每帧限制最低时间
		return;
	}
	last_update_mtime_ = mtime;
	
	if (mtime - last_cell_mtime_ >= cell_msec_)
	{
		if (cell_index_ >= 0 && cell_index_ < int32_t(time_cell_.size()))
		{
			TimeWheelCell & one_cell = time_cell_[cell_index_];
			// 更新定时器
			for (auto it = one_cell.timer_list.begin(); it != one_cell.timer_list.end();)
			{
				TimeWhellNode & timer_node = *it;
				if (timer_node.round > wheel_round_)
				{
					++it;
				}
				else
				{
					if (timer_node.timer_ptr)
					{
						timer_node.timer_ptr->Run(mtime);
						if (timer_node.timer_ptr->NextTime() > 0)
						{
							// 还要继续执行
							todo_timers_.insert(std::make_pair(timer_node.timer_ptr->NextTime(), timer_node.timer_ptr));
						}
						timer_cell_index_.erase(timer_node.timer_ptr.get());
					}

					one_cell.timer_list.erase(it++);
				}
			}
		}
		++cell_index_;
		if (cell_index_ >= int32_t(time_cell_.size()))
		{
			++wheel_round_;
			cell_index_ = 0;
		}
		last_cell_mtime_ = mtime;
	}
	

	if (!todo_timers_.empty())
	{
		for (auto it = todo_timers_.begin(); it != todo_timers_.end(); ++it)
		{
			// 继续添加定时器
			RealAddTimer(it->second);
		}
		todo_timers_.clear();
	}
	
	// 删除
	if (!to_delete_timers_.empty())
	{
		for (auto it = to_delete_timers_.begin(); it != to_delete_timers_.end(); ++it)
		{
			for (auto it_add = todo_timers_.begin(); it_add != todo_timers_.end();)
			{
				if (it_add->second.get() == (*it).get())
				{
					timer_cell_index_.erase((*it).get());
					todo_timers_.erase(it_add++);
				}
				else
				{
					++it_add;
				}
			}
			for (auto & one_cell : time_cell_)
			{
				for (auto it_node = one_cell.timer_list.begin(); it_node != one_cell.timer_list.end();)
				{
					if (it_node->timer_ptr.get() == (*it).get())
					{
						timer_cell_index_.erase((*it).get());
						one_cell.timer_list.erase(it_node++);
					}
					else
					{
						++it_node;
					}
				}
			}
		}
		to_delete_timers_.clear();
	}
}

// 添加定时器
void TimeWheel::AddTimer(std::shared_ptr<TRTimer> todo_timer)
{
	todo_timers_.insert(std::make_pair(todo_timer->GetRunTime(), todo_timer));
}
// 删除定时器
void TimeWheel::DeleteTimer(std::shared_ptr<TRTimer> todo_timer)
{
	to_delete_timers_.push_back(todo_timer);
}

void TimeWheel::RealAddTimer(std::shared_ptr<TRTimer> todo_timer)
{
	if (!todo_timer)
		return;
	if (time_cell_.empty())
		return;
	auto it_timer_index = timer_cell_index_.find(todo_timer.get());
	if (it_timer_index != timer_cell_index_.end())
	{
		return;
	}
	time_t run_time = todo_timer->GetRunTime();
	time_t interval_time = run_time - last_cell_mtime_;
	if (interval_time < 0)
	{
		// 保护一下,不能是过去的时间
		interval_time = 0;
	}
	int32_t cell_count = static_cast<int32_t>(interval_time / cell_msec_);
	int32_t new_cell_index = cell_index_ + (cell_count > 0 ? cell_count - 1:0);
	
	TimeWhellNode one_node;
	one_node.timer_ptr = todo_timer;
	one_node.round = wheel_round_;
	
	new_cell_index = new_cell_index % time_cell_.size();
	int32_t shift_cell_count = cell_count - (time_cell_.size() - cell_index_);
	if (shift_cell_count > 0)
	{
		one_node.round = shift_cell_count / time_cell_.size() + wheel_round_;
		if (shift_cell_count % time_cell_.size() != 0)
		{
			one_node.round += 1;
		}
	}
	/*std::cout << "add new timer node, round:" << one_node.round 
		<< ", cell_index:" << new_cell_index << ", cell_count:" << cell_count
		<< std::endl;*/
	time_cell_[new_cell_index].timer_list.push_back(one_node);
	timer_cell_index_.insert(std::make_pair(todo_timer.get(), new_cell_index));
}

int32_t TimeWheel::GetTimerSize()
{
	int32_t count = 0;
	count += todo_timers_.size();
	for (auto & one_cell : time_cell_)
	{
		count += one_cell.timer_list.size();
	}
	return count;
}