/********************************************************
# Author       : mknight
# Last modified: 2019-05-02 10:20
# Email        : 824338670@qq.com
# Filename     : singleton.h
# Description  : 
# *******************************************************/

#ifndef __TR_SINGLETON_H__
#define __TR_SINGLETON_H__

template <class T>
class TSingleton
{
public:
	static T &Instance()
	{
#ifdef MT_SINGLETON
		if (nullptr == s_instance_)
		{
			std::unique_lock<std::mutex> scoped_lock(s_mutex_);
#endif
			if (nullptr == s_instance_)
			{
				s_instance_ = new T();
			}
#ifdef MT_SINGLETON
		}
#endif
		return *s_instance_;
	}

	static void Release()
	{
#ifdef MT_SINGLETON
		if (nullptr != s_instance_)
		{
			std::unique_lock<std::mutex> scoped_lock(s_mutex_);
#endif
			if (nullptr != s_instance_)
			{
				delete s_instance_;
				s_instance_ = nullptr;
			}
#ifdef MT_SINGLETON
		}
#endif
	}

protected:
	TSingleton() {}
	~TSingleton() {}

private:
	static T *s_instance_;

#ifdef MT_SINGLETON
	static std::mutex s_mutex_;
#endif
};

// init
template <class T>
T *TSingleton<T>::s_instance_ = nullptr;

#ifdef MT_SINGLETON
template <class T>
std::mutex TSingleton<T>::s_mutex_;
#endif

#endif // __TR_SINGLETON_H__
