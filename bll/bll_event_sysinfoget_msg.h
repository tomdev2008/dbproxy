/*
 * bll_event_sysinfoget_msg.h
 *
 *  Created on: 2013-3-7
 *      Author: liufl
 */

#ifndef BLL_EVENT_SYSINFOGET_MSG_H_
#define BLL_EVENT_SYSINFOGET_MSG_H_

#include "../def/def_dbproxy.h"
#include "common/common_singleton.h"
#include "frame_session.h"
#include "frame_protocolhead.h"
#include "lightframe_impl.h"
#include "bll_base.h"
#include "../dal/common_query.h"

#include "../util.h"
#include "../dal/dal_player.h"
#include "../dal/billengine.h"
#include "../def/def_dbproxy_errorcode.h"
#include "sysinfoserver_message_define.h"


FRAME_DBPROXY_NAMESPACE_BEGIN

class CFromSysinfoGetEvent: public CBllBase
{
public:
	CFromSysinfoGetEvent(){}
	virtual ~CFromSysinfoGetEvent(){}

public:
	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL)
	{
		return S_OK;
	}
public:
	int32_t OnMessage_getmsgcount(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessSysGetMsgCount(const CGetMessageCountReq& reqbody, CGetMessageCountResp& respbody);
	int32_t QueryRoleMsgCount(const RoleID roleid, const ChannelType channeltype, const uint8_t readed, uint32_t& msgcount);

	int32_t OnMessage_getnoti(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QuerySystemNotice(CGetNoticeResp& respbody);

	int32_t OnMessage_getNotiVersion(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QueryNoticeVersion(int32_t& sysinfo_version);

	int32_t OnMessage_getRankInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QueryRankInfo(CRankInfoResp& respbody);

	int32_t OnMessage_getRecharge(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessSysGetRecharge(const CGetSupplymentReq& reqbody, CGetSupplymentResp& respbody);
	int32_t QueryUserTotalRechargeInTimeBucket(const RoleID roleid, const uint64_t starttime, const uint64_t endtime, int32_t& totalRechargeAmount);

	int32_t OnMessage_getVipInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QueryUserVipInfo(uint32_t& vipplayercount, SingleRoleInfo arrVipPlayerInfo[], const CGetVipRoleReq& reqbody);

	int32_t OnMessage_getMsg(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessSysGetMsg(const CGetMessageReq& reqbody, CGetMessageResp& respbody);
	int32_t GetUnReadeSysMessage(const CGetMessageReq& reqbody, CGetMessageResp& respbody);
	int32_t GetReadedSysMessage(const CGetMessageReq& reqbody, CGetMessageResp& respbody);
	int32_t GetTotalSysMessage(const CGetMessageReq& reqbody, CGetMessageResp& respbody);



};

#define	CREATE_FROM_SYSINFOGET_EVENT_INSTANCE	        CSingleton<CFromSysinfoGetEvent>::CreateInstance
#define	GET_FROM_SYSINFOGET_EVENT_INSTANCE		        CSingleton<CFromSysinfoGetEvent>::GetInstance
#define	DESTROY_FROM_SYSINFOGET_EVENT_INSTANCE	        CSingleton<CFromSysinfoGetEvent>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END


#endif /* BLL_EVENT_SYSINFOGET_MSG_H_ */
