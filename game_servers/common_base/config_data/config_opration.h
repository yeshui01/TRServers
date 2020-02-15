/*=======================================================
# Author       : mknight
# Last modified: 2020-02-15 14:50
# Email        : 824338670@qq.com
# Filename     : config_opration.h
# Description  : 
# =======================================================*/

#ifndef __TR_CONFIG_OPERATION_H__
#define __TR_CONFIG_OPERATION_H__

#include "tr_common/base_type_def.h"

namespace ConfigData
{
    /**
     * 初始化
     */
    bool Init();
    
    /**
     * 重新加载
     */
    bool Reload();

    /**
     * 清理数据
     */
    void Clear();
};

#endif // __TR_CONFIG_OPERATION_H__