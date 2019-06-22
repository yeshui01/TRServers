/********************************************************
# Author       : mknight
# Last modified: 2019-05-01 11:57
# Email        : 824338670@qq.com
# Filename     : log_module.h
# Description  : 
# *******************************************************/
#ifndef __LOG_MODULE_H__
#define __LOG_MODULE_H__
#include <cstdint>
#include <string>
#include <iostream>
#include "singleton.h"

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/loggingmacros.h>

// tmp code, to be continue to change to log4cplusplus
// #define TDEBUG(x) std::cout << "[debug]:file[" << __FILE__ << "]\nline[" << __LINE__ << "]" << x << std::endl;
// #define TERROR(x) std::cerr << "[error]:file[" << __FILE__ << "]\nline[" << __LINE__ << "]" << x << std::endl;

class ServerLog : public TSingleton<ServerLog>
{
public:
    ServerLog(){}
    ~ServerLog(){}
    void Init(std::string config_file)
    {
        if (!inited_)
        {
            auto tmp_logger = log4cplus::Logger::getRoot(); // 因为源码中logger类不允许使用右值引用赋值,所以这里先定义一个临时变量
            logger_.swap(tmp_logger);
	        log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(config_file.c_str()));
            inited_ = true;
        }
    }
    log4cplus::Logger& GetLogger()
    {
        if (!inited_)
        {
            Init("log_config/default_log.properties");
        }
        return logger_;
        // return log4cplus::Logger::getRoot();
    }
protected:
    log4cplus::Logger logger_;
    bool inited_ = false;
};

#define g_ServerLog ServerLog::Instance()


#define TDEBUG(x) LOG4CPLUS_DEBUG(g_ServerLog.GetLogger(), "[" << __FILE__ << ":" << __LINE__ << "]" << x);
#define TINFO(x) LOG4CPLUS_INFO(g_ServerLog.GetLogger(), "[" << __FILE__ << ":" << __LINE__ << "]" << x);
#define TWARN(x) LOG4CPLUS_WARN(g_ServerLog.GetLogger(), "[" << __FILE__ << ":" << __LINE__ << "]" << x);
#define TERROR(x) LOG4CPLUS_ERROR(g_ServerLog.GetLogger(), "[" << __FILE__ << ":" << __LINE__ << "]" << x);

#endif	// __LOG_MODULE_H__
