/*
 * bll_event_roomadd_msg.h
 *
 *  Created on: 2013-3-6
 *      Author: liufl
 */

#ifndef BLL_EVENT_ROOMADD_MSG_H_
#define BLL_EVENT_ROOMADD_MSG_H_

#include "../def/def_dbproxy.h"
#include "common/common_singleton.h"
#include "frame_session.h"
#include "frame_protocolhead.h"
#include "lightframe_impl.h"
#include "bll_base.h"
#include "../dal/common_query.h"

#include "../util.h"
#include "../dal/dal_player.h"
#include "../dal/roomserver_message.h"
#include "../def/def_dbproxy_errorcode.h"
#include "roomserver_message_define.h"


FRAME_DBPROXY_NAMESPACE_BEGIN

class CFromRoomAddEvent: public CBllBase
{
public:
	CFromRoomAddEvent(){}
	virtual ~CFromRoomAddEvent(){}

public:
	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL)
	{
		return S_OK;
	}
public:
	int32_t OnMessage_AddBlacklist(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t InsertPlayerIntoRoomBlackList(const RoleID roleid, const RoomID roomid, const char* szDateTime, const RoleID operatorID);

	int32_t OnMessage_AddInRoomTime(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UpdateAdminInRoomOnlineTimeDay(const CAddAdmInRoomOnLineTimeNotice& msgbody, const RoleRank rolerank);
	int32_t UpdateAdminInRoomOnlineTimeTotal(const RoleID roleid, const RoomID roomid, const uint32_t addtime, const RoleRank rolerank);

	int32_t OnMessage_AddOnMike(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QueryRoleRankInRoom(const RoleID roleid, const RoomID roomid, RoleRank& rolerank);
	int32_t UpdateRoleOnMikeDay(const CAddOnMicTimeNotice& msgbody, const RoleRank rolerank);
	int32_t UpdateRoleOnMikeTotal(const RoomID roomid, const RoleID roleid, const uint32_t addtime, const RoleRank rolerank);

};

#define	CREATE_FROM_ROOMADD_EVENT_INSTANCE	        CSingleton<CFromRoomAddEvent>::CreateInstance
#define	GET_FROM_ROOMADD_EVENT_INSTANCE		        CSingleton<CFromRoomAddEvent>::GetInstance
#define	DESTROY_FROM_ROOMADD_EVENT_INSTANCE	        CSingleton<CFromRoomAddEvent>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END
#endif /* BLL_EVENT_ROOMADD_MSG_H_ */
