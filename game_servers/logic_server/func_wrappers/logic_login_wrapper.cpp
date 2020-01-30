/*=======================================================
# Author       : mknight
# Last modified: 2020-01-28 09:28
# Email        : 824338670@qq.com
# Filename     : logic_login_wrapper.cpp
# Description  : 
# =======================================================*/
#include "logic_login_wrapper.h"
#include "logic_server/logic_player.h"

LcLoginWrapper::LcLoginWrapper(LogicPlayer *p)
{
    player_ = p;
}

LcLoginWrapper::~LcLoginWrapper()
{

}

void LcLoginWrapper::InitCreateData()
{
    // TODO:
}