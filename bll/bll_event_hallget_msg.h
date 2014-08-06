/*
 * bll_event_hallget_msg.h
 *
 *  Created on: 2013-2-28
 *      Author: liufl
 */

#ifndef BLL_EVENT_HALLGET_MSG_H_
#define BLL_EVENT_HALLGET_MSG_H_
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

class CFromHallGetEvent: public CBllBase
{
public:
	CFromHallGetEvent(){}
	virtual ~CFromHallGetEvent(){}

public:
	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL)
	{
		return S_OK;
	}

public:
	int32_t OnMessage_HallGetRoleInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessHallLoginGetRolebaseinfoReq(const CHallGetRoleLoginInfoReq_Public* reqbody, CHallGetRoleInfoResp_Public& respbody);
	int32_t QueryRoleRoomMember(const RoleID roleid, int32_t& admincount, RoomID arrRoomID[], RoleRank arrRoleRank[], const int32_t max_admin_per_user);
	int32_t QueryUserTotalOnlineTime(const RoleID roleid, uint32_t& total_online_time);
	int32_t QueryRoleSelfSetInfo(const RoleID roleid, uint32_t& nUserInfoSelfSet);
	int32_t QueryUserAccountLock(const RoleID roleid, uint64_t& nTimeLock, uint64_t& nTimeUnlock, char szLockReason[]);
	int32_t QueryLockIPInfo(const char* ip, uint64_t& start_lock_time, uint64_t& end_lock_time, char* lock_reason);
	int32_t QueryLockMacInfo(const char* mac, uint64_t& start_lock_time, uint64_t& end_lock_time, char* lock_reason);
	int32_t QueryAchievement(const RoleID roleid, int32_t& achieve_count, int32_t achieve[], const int32_t max_achieve_count);

	int32_t OnMessage_HallGetRoomAdmcount(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessHallGetRoomAdmCountReq(const CHallGetRoomAdminCountReq& reqbody, CHallGetRoomAdminCountResp& respbody);
	int32_t QueryRoomAdminCount(const RoomID roomid, const RoleRank rolerank, int32_t& admincount);
	int32_t QueryRoomShowCapacity(const RoomID roomid, int32_t& roomShowCapacity);

	int32_t OnMessage_HallGetRoomInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessHallGetRoomInfoReq(const CHallGetRoomInfoReq_Public& reqbody, CHallGetRoomInfoResp_Public& respbody);

private:
	AccountID GetNormalAccount();
	bool AddUserBaseInfo(const char *szAccountName, AccountID nAccountID, RoleID nRoleID, int32_t nOSVersion, const char *szMacAddr, const char *szRegIP);
};

#define	CREATE_FROM_HALLGET_EVENT_INSTANCE	        CSingleton<CFromHallGetEvent>::CreateInstance
#define	GET_FROM_HALLGET_EVENT_INSTANCE		        CSingleton<CFromHallGetEvent>::GetInstance
#define	DESTROY_FROM_HALLGET_EVENT_INSTANCE	        CSingleton<CFromHallGetEvent>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END
#endif /* BLL_EVENT_HALLGET_MSG_H_ */
