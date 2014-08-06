/*
 * bll_event_roomset_msg.h
 *
 *  Created on: 2013-3-6
 *      Author: liufl
 */

#ifndef BLL_EVENT_ROOMSET_MSG_H_
#define BLL_EVENT_ROOMSET_MSG_H_


#include "../def/def_dbproxy.h"
#include "common/common_singleton.h"
#include "frame_session.h"
#include "frame_protocolhead.h"
#include "lightframe_impl.h"
#include "bll_base.h"
#include "../dal/common_query.h"

#include "../util.h"
#include "../dal/dal_player.h"
#include "../def/def_dbproxy_errorcode.h"
#include "roomserver_message_define.h"


FRAME_DBPROXY_NAMESPACE_BEGIN

class CFromRoomSetEvent: public CBllBase
{
public:
	CFromRoomSetEvent(){}
	virtual ~CFromRoomSetEvent(){}

public:
	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL)
	{
		return S_OK;
	}
public:
	int32_t OnMessage_EnterRoomNotice(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UpdateAdminLastEnterRoomTime(const RoomID roomid, const RoleID roleid, const char* szDateTime);
	int32_t UpdateRoleLatestEnterRoomInfo(const RoleID roleid, const RoomID roomid, const char* szLastEnterTime);

	int32_t OnMessage_SetAttr(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessRoomSetAttrReq(const CSetRoomAttrReq& reqbody, CSetRoomAttrResp& respbody, const RoomID roomid);
	int32_t UpdateRoomBaseInfo(const RoomBaseInfo& roombaseinfo, const int32_t roombaseinfotype);

	int32_t OnMessage_SetPlayerCount(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnMessage_SetTitle(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UpdateRoomAdminInfo(const RoomAdminInfo& roomadmin);

	int32_t OnMessage_UpdownMike(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t InsertUpdownMikeInfo(const CStaPlayerMicStatus& msgbody);

	int32_t  OnMessage_SetSongList(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UpdateSongList(const CUpdateSongListNoti& reqbody);

	int32_t OnMessage_SetSongOrder(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t InsertRoomSongOrder(const CUpdateSongOrderInfoNoti& reqbody);

};

#define	CREATE_FROM_ROOMSET_EVENT_INSTANCE	        CSingleton<CFromRoomSetEvent>::CreateInstance
#define	GET_FROM_ROOMSET_EVENT_INSTANCE		        CSingleton<CFromRoomSetEvent>::GetInstance
#define	DESTROY_FROM_ROOMSET_EVENT_INSTANCE	        CSingleton<CFromRoomSetEvent>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END
#endif /* BLL_EVENT_ROOMSET_MSG_H_ */
