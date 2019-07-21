/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-29 11:35
 * Email        : 824338670@qq.com
 * Filename     : common_define.h
 * Description  : 
 * ======================================================*/

#ifndef __COMMON_DEFINE_H__
#define __COMMON_DEFINE_H__


#define SAFE_DELETE_PTR(x) if (x) \
{\
delete x; \
x=nullptr; \
}

#define REQMSG(msg_type) protos::req_##msg_type
#define REPMSG(msg_type) protos::rep_##msg_type

#endif // __COMMON_DEFINE_H__