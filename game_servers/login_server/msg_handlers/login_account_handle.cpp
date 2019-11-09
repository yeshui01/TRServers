/*=======================================================
# Author       : mknight
# Last modified: 2019-10-04 15:36
# Email        : 824338670@qq.com
# Filename     : login_account_handle.cpp
# Description  : 
# =======================================================*/
#include "login_account_handle.h"

#include "protocol_class.h"
#include "protocol_login.h"
#include "protcl_login.pb.h"

#include "server_common/server_define.h"
#include "tr_log/log_module.h"
#include "server_common/server_info_manager.h"
#include "server_common/server_session.h"
#include "protocol_error_code.h"
#include "database_tools/database_hash.h"
#include "login_server/login_global.h"
#include "tr_timer/server_time.h"
LoginAccountHandler::LoginAccountHandler()
{

}

LoginAccountHandler::~LoginAccountHandler()
{

}

void LoginAccountHandler::BindMsgHandle()
{
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_C2S_REGISTER_ACCOUNT), LoginAccountHandler, OnRegisnterAccount);
    MSG_BIND_HANDLER(INT_LOGINMSG(E_LOGIN_MSG_ROOT2LOGIN_ACCT_CHECK), LoginAccountHandler, OnAccountCheck);
}

// 注册账号
EMsgHandleResult LoginAccountHandler::OnRegisnterAccount(TConnection *session_pt, const NetMessage * message_pt)
{
    TINFO("OnRegisnterAccount");
    REQMSG(E_LOGIN_MSG_C2S_REGISTER_ACCOUNT) req;
    REPMSG(E_LOGIN_MSG_C2S_REGISTER_ACCOUNT) rep;
    if (!STRING_TO_PBMSG(message_pt->GetContent(), req))
    {
        rep.set_isok((INT_PROTOERR(E_PROTOCOL_ERR_PB_PARSE_ERROR)));
        TERROR("parse pbmsg failed");
        rep.SerializeToString(&rep_content_);
        return EMsgHandleResult::E_MSG_HANDLE_RETURN_CONTENT;
    }
    TDEBUG("req_msg:" << req.ShortDebugString());
    rep.set_account_name(req.account_name());
    rep.set_pswd(req.pswd());
    if (req.account_name().length() < 2 || req.pswd().length() < 6)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_PARAM_ERROR, rep);
    }
    int32_t acc_db_id = DatabaseHash::AccountDataBaseHashIdByAccName(req.account_name());
    std::string acc_tb = DatabaseHash::AccountTbNameHashByAccName(req.account_name());
    TDEBUG("acc_name:" << req.account_name() << ", acc_db_id:" << acc_db_id << ", acc_tb:" << acc_tb);
    // 是否已经存在这个名字
    auto database_pt = g_LoginGlobal.db_helper_.HoldDataBase(acc_db_id);
    if (!database_pt)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_CORRECT, rep);
    }
    std::vector<DataTableItem> v_local_set;
    std::stringstream ss_sql;
    ss_sql << "select user_id, user_name from " << acc_tb << " where user_name = '" << req.account_name() << "';";
    TDEBUG("ss_sql:" << ss_sql.str());
    database_pt->Query(ss_sql.str(), v_local_set);
    if (v_local_set.size() > 0)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_ACCNAME_EXISTED, rep);
    }
    int64_t reg_time = g_ServerTime.NowTime();
    ss_sql.str("");
    ss_sql.clear();
    ss_sql << "insert into " << acc_tb << "(`user_name`, `pswd`, `reg_time`) values(" << "'" << req.account_name() << "','" << req.pswd() << "'," << reg_time << ");";
    TDEBUG("insert new account info:" << ss_sql.str());
    if (database_pt->ExecuteUpdate(ss_sql.str()) > 0)
    {
        v_local_set.clear();
        ss_sql.str("");
        ss_sql.clear();
        ss_sql << "select user_id, user_name from " << acc_tb << " where user_name = '" << req.account_name() << "';";
        database_pt->Query(ss_sql.str(), v_local_set);
        if (v_local_set.size() > 0)
        {
            int64_t acc_id = v_local_set[0].GetFieldBigIntValue(0);
            TDEBUG("reg account success, acc_id:" << acc_id);
            rep.set_account_id(acc_id);
            rep.set_account_name(req.account_name());
            rep.set_pswd(req.pswd());
        }
        else 
        {
            SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_PARAM_ERROR, rep);
        }
    }
    else 
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_PARAM_ERROR, rep);
    }
    rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_CORRECT));
    
    RETURN_REP_CONTENT(rep);
}

EMsgHandleResult LoginAccountHandler::OnAccountCheck(TConnection *session_pt, const NetMessage * message_pt)
{
    TINFO("OnAccountCheck");
    REQMSG(E_LOGIN_MSG_ROOT2LOGIN_ACCT_CHECK) req;
    REPMSG(E_LOGIN_MSG_ROOT2LOGIN_ACCT_CHECK) rep;
    if (!STRING_TO_PBMSG(message_pt->GetContent(), req))
    {
        TERROR("parse pbmsg failed");
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_PB_PARSE_ERROR, rep);
    }
    TINFO("req_E_LOGIN_MSG_ROOT2LOGIN_ACCT_CHECK:" << req.ShortDebugString());
    rep.set_isok(INT_PROTOERR(E_PROTOCOL_ERR_CORRECT));

    if (req.account_name().length() < 2 || req.pswd().length() < 6)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_PARAM_ERROR, rep);
    }
    int32_t acc_db_id = DatabaseHash::AccountDataBaseHashIdByAccName(req.account_name());
    std::string acc_tb = DatabaseHash::AccountTbNameHashByAccName(req.account_name());
    TDEBUG("acc_name:" << req.account_name() << ", acc_db_id:" << acc_db_id << ", acc_tb:" << acc_tb);
    // 是否已经存在这个名字
    auto database_pt = g_LoginGlobal.db_helper_.HoldDataBase(acc_db_id);
    if (!database_pt)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_CORRECT, rep);
    }
    std::vector<DataTableItem> v_local_set;
    std::stringstream ss_sql;
    ss_sql << "select user_id, user_name, pswd from " << acc_tb << " where user_name = '" << req.account_name() << "';";
    TDEBUG("ss_sql:" << ss_sql.str());
    database_pt->Query(ss_sql.str(), v_local_set);
    if (v_local_set.size() < 1)
    {
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_ACCOUNT_NOT_EXISTED, rep);
    }
    if (v_local_set.size() > 1)
    {
        // 数据异常
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_PARAM_ERROR, rep);
    }
    // 密码是否正确
    if (v_local_set[0].GetFieldStringValue(2) != req.pswd())
    {
        // 密码错误
        SET_ISOK_AND_RETURN_CONTENT(E_PROTOCOL_ERR_ACCOUNT_PSWD_ERROR, rep);
    }
    RETURN_REP_CONTENT(rep);
}