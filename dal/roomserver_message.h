/***************************************************************
* filename: roomserver_messge.h
* created:  2012-02-06 10:45
* author:   xiaohq
* purpose:  encode and decode message from roomserver
* version:  1.0
* revision: not yet
***************************************************************/

#ifndef _DBPROXY_ROOMSERVER_MESSAGE_DEFINE_H_
#define _DBPROXY_ROOMSERVER_MESSAGE_DEFINE_H_

//#include "frame/dal/frame_common_codeengine.h"
#include "../def/def_dbproxy.h"
#include "roomserver_message_define.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

class CRoomSetTitleReq : public  CSetTitleReq_Public
{

};

class CRoomSetTitleResp: public CSetTitleResp_Public
{

};

/*一定要实现构造函数和析构函数嚒？*/
class CRoomGetRoleInfoReq: public CGetRoleInfoReq_Public
{

};

class CRoomGetRoleInfoResp: public CGetRoleInfoResp_Public
{

};

class CRoomGetRoomInfoReq: public CGetRoomInfoReq_Public
{

};

class CRoomGetRoomInfoResp: public CGetRoomInfoResp_Public
{

};

class CRoomDelAdminReq: public CDelTitleReq_Public
{

};

class CRoomDelAdminResp: public CDelTitleResp_Public
{

};




MESSAGEMAP_DECL_BEGIN(message_from_roomserver)

//设置房间管理员请求
MESSAGEMAP_DECL_MSG(MSGID_RSDB_SET_TITLE_REQ, (new CRoomSetTitleReq()))
//获取玩家信息请求
MESSAGEMAP_DECL_MSG(MSGID_RSDB_GETROLEINFO_REQ, (new CRoomGetRoleInfoReq()))
//获取房间信息请求
MESSAGEMAP_DECL_MSG(MSGID_RSDB_GETROOMINFO_REQ, (new CRoomGetRoomInfoReq()))


MESSAGEMAP_DECL_END()

FRAME_DBPROXY_NAMESPACE_END

#endif
