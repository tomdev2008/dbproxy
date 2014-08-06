/*
 * bll_event_roomget_msg.h
 *
 *  Created on: 2013-3-4
 *      Author: liufl
 */

#ifndef BLL_EVENT_ROOMGET_MSG_H_
#define BLL_EVENT_ROOMGET_MSG_H_

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

class CFromRoomGetEvent: public CBllBase
{
public:
	CFromRoomGetEvent(){}
	virtual ~CFromRoomGetEvent(){}

public:
	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL)
	{
		return S_OK;
	}
public:
	int32_t OnMessageGetRoleInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessRoomGetRoleInfo(const CRoomGetRoleInfoReq& reqbody, CRoomGetRoleInfoResp& respbody );
	int32_t QueryUserTotalOnlineTime(const RoleID roleid, uint32_t& total_online_time);
	int32_t QueryUserTotalOnMikeTime(const RoleID roleid, uint32_t& total_onmike_time);
	int32_t QueryRoleSelfSetInfo(const RoleID roleid, uint32_t& nUserInfoSelfSet);
	int32_t QueryAchievement(const RoleID roleid, int32_t& achieve_count, int32_t achieve[], const int32_t max_achieve_count);
	int32_t QueryRoleRoomMember(const RoleID roleid, int32_t& admincount, RoomID arrRoomID[], RoleRank arrRoleRank[], const int32_t max_admin_per_user);
	int32_t QueryRolecollection(const RoleID roleid, int32_t& cllcount, RoomID arrRoomID[], const int32_t max_collection_count);

	int32_t OnMessageGetRoomInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessRoomGetRoomInfoReq(const CRoomGetRoomInfoReq& reqbody, CRoomGetRoomInfoResp& respbody);
	int32_t QueryRoomMember(const RoomID roomid, uint16_t& room_admin_count, const int32_t max_admin_per_room, RoleID arrAdminRoleID[],
			RoleRank arrRoleRank[], uint32_t arrTotalInRoomTime[]);
	int32_t QueryUserTotalOnMikeTimeInOneRoom(const RoomID roomid, const RoleID roleid, uint32_t& total_onmike_time);
	int32_t QueryRoomBlackList(const RoomID roomid, uint32_t& room_black_list_player_count, RoleID arrBlackPlayerRoleID[]);
	int32_t QueryRoomLockInfo(const RoomID roomid, uint32_t& startLockTime, uint32_t& endLockTime, char szLockReason[]);

	int32_t OnMessageGetSongList(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QuerySongList(const CGetSongListFromDBReq& reqbody, CGetSongListFromDBResp& respbody);

	int32_t OnMessageGetRoomArtistList(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QueryArtistList(const CGetRoomArtistReq& reqbody, CGetRoomArtistResp& respbody);

	int32_t OnMessageGetArtistInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QueryArtistInfo(const CGetOfflineArtistInfoReq& reqbody, CGetOfflineArtistInfoResp& respbody);
};

#define	CREATE_FROM_ROOMGET_EVENT_INSTANCE	        CSingleton<CFromRoomGetEvent>::CreateInstance
#define	GET_FROM_ROOMGET_EVENT_INSTANCE		        CSingleton<CFromRoomGetEvent>::GetInstance
#define	DESTROY_FROM_ROOMGET_EVENT_INSTANCE	        CSingleton<CFromRoomGetEvent>::DestroyInstance




FRAME_DBPROXY_NAMESPACE_END
#endif /* BLL_EVENT_ROOMGET_MSG_H_ */
