/*=======================================================
# Author       : mknight
# Last modified: 2020-01-26 16:07
# Email        : 824338670@qq.com
# Filename     : tb_role_base.cpp
# Description  : 
# =======================================================*/
#include "tb_role_base.h"

TbRoleBase::TbRoleBase(int32_t table_id) : DataTableBase(table_id)
{
    // role_id
    AddField(E_FIELD_VALUE_TYPE_BIG_INT, true);
    // acc_id
    AddField(E_FIELD_VALUE_TYPE_BIG_INT, false);
    // nickname
    AddField(E_FIELD_VALUE_TYPE_STRING, false);
    // create_time
    AddField(E_FIELD_VALUE_TYPE_BIG_INT, false);
    // zone_id
    AddField(E_FIELD_VALUE_TYPE_INT, false);
    // inited
    AddField(E_FIELD_VALUE_TYPE_INT, false);
}
TbRoleBase::~TbRoleBase()
{
}
