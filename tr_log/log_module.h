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

// tmp code, to be continue to change to log4cplusplus
#define TDEBUG(x) std::cout << "[debug]:file[" << __FILE__ << "]\nline[" << __LINE__ << "]" << x << std::endl;
#define TERROR(x) std::cerr << "[error]:file[" << __FILE__ << "]\nline[" << __LINE__ << "]" << x << std::endl;

#endif	// __LOG_MODULE_H__
