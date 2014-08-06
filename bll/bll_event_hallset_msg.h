/*
 * bll_event_hallset_msg.h
 *
 *  Created on: 2013-3-6
 *      Author: liufl
 */

#ifndef BLL_EVENT_HALLSET_MSG_H_
#define BLL_EVENT_HALLSET_MSG_H_

#include "../def/def_dbproxy.h"
#include "common/common_singleton.h"
#include "frame_session.h"
#include "frame_protocolhead.h"
#include "lightframe_impl.h"
#include "bll_base.h"
#include "../dal/common_query.h"
#include "../def/def_dbproxy_errorcode.h"

#include "../util.h"
#include "../dal/dal_player.h"
#include "hallserver_message_define.h"


FRAME_DBPROXY_NAMESPACE_BEGIN

class CFromHallSetEvent: public CBllBase
{
public:
	CFromHallSetEvent(){}
	virtual ~CFromHallSetEvent(){}

public:
	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL)
	{
		return S_OK;
	}

public:
	int32_t OnMessage_OnlineNotice(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UpdateLastLoginIP(const RoleID roleid);
	int32_t UpdateLoginIP(const RoleID roleid, const char* szLoginIP);
	int32_t UpdateLastLoginTime(const RoleID roleid, const int64_t lastlogintime);
	int32_t UpdateLoginTimes(const RoleID roleid);

	int32_t OnMessage_SetLoginInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UpdateOSVersion(const RoleID roleid, const uint32_t os_version);
	int32_t UpdateMacAddr(const RoleID roleid, const char* szMacAddr);
	int32_t UpdateLoginChannel(const RoleID roleid, const uint32_t channel);

	int32_t OnMessage_SetLogoutInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UpdateLastLogoutTime(const RoleID roleid, const int64_t last_logout_time);

	int32_t OnMessage_SetRoleRank(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessHallSetRoleRankReq(const CHallSetTitleReq& reqbody, CHallSetTitleResp& respbody);
	int32_t UpdateRoleRankInRoom(const RoomID roomid, const RoleID roleid, const RoleRank rolerank,  const RoleID operatorid);

	int32_t OnMessage_SetSelfInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessHallSetSelfInfo(const CHallSetSelfInfoReq& reqbody, CDBSetSelfSelfInfoResp& respbody);
	int32_t UpdateUserSelfSetting(const RoleID roleid, const uint32_t userinfoSelfSet);

	void UpdateMemcache(RoleBaseInfo &rolebaseinfo);

};

#define	CREATE_FROM_HALLSET_EVENT_INSTANCE	        CSingleton<CFromHallSetEvent>::CreateInstance
#define	GET_FROM_HALLSET_EVENT_INSTANCE		        CSingleton<CFromHallSetEvent>::GetInstance
#define	DESTROY_FROM_HALLSET_EVENT_INSTANCE	        CSingleton<CFromHallSetEvent>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END
#endif /* BLL_EVENT_HALLSET_MSG_H_ */
