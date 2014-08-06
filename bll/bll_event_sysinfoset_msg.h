/*
 * bll_event_sysinfoset_msg.h
 *
 *  Created on: 2013-3-8
 *      Author: liufl
 */

#ifndef BLL_EVENT_SYSINFOSET_MSG_H_
#define BLL_EVENT_SYSINFOSET_MSG_H_
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

class CFromSysinfoSetEvent: public CBllBase
{
public:
	CFromSysinfoSetEvent(){}
	virtual ~CFromSysinfoSetEvent(){}

public:
	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL)
	{
		return S_OK;
	}
public:
	int32_t OnMessage_ClearVip(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessSysClearTimeOutVip(const CClearTimeoutVipReq& reqbody, CClearTimeoutVipResp& respbody);
	int32_t QueryTimeOutPurpleVip(RoleID arrRoleID[], int32_t& nTimeOutVipCount);
	int32_t UpdateTimeOutPurpleVipLevel(const RoleID roleid);

	int32_t OnMessage_Degrade(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessSysDegrade(const CDegradeReq& reqbody, CDegradeResp& respbody);
	int32_t DegradeUserVipLevel(const RoleID roleid, const VipLevel cur_viplevel);

	int32_t OnMessage_GroupDegrade(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessSysGroupDegrade(const CGroupDegradeReq& reqbody, CGroupDegradeResp& respbody);

	int32_t OnMessage_NewMsg(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessSysNewMsg(const CNewMessageReq& reqbody, CNewMessageResp& respbody);
	int32_t InsertNewMessage(const RoleID roleid, const ChannelType channeltype, const char* szContent, const int32_t nContentLen);

	int32_t OnMessage_WeekRecharge(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QueryWeekRecharge(const RoleID roleid, const uint32_t weekindex, uint32_t& amount);

};

#define	CREATE_FROM_SYSINFOSET_EVENT_INSTANCE	        CSingleton<CFromSysinfoSetEvent>::CreateInstance
#define	GET_FROM_SYSINFOSET_EVENT_INSTANCE		        CSingleton<CFromSysinfoSetEvent>::GetInstance
#define	DESTROY_FROM_SYSINFOSET_EVENT_INSTANCE	        CSingleton<CFromSysinfoSetEvent>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END

#endif /* BLL_EVENT_SYSINFOSET_MSG_H_ */
