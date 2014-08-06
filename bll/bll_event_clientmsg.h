/*
 * bll_event_clientmsg.h
 *
 *  Created on: 2013-2-26
 *      Author: liufl
 */

#ifndef BLL_EVENT_CLIENTMSG_H_
#define BLL_EVENT_CLIENTMSG_H_

#include "../def/def_dbproxy.h"
#include "common/common_singleton.h"
#include "frame_session.h"
#include "frame_protocolhead.h"
#include "lightframe_impl.h"
#include "bll_base.h"
#include "../dal/common_query.h"

#include "../util.h"
#include "../dal/dal_player.h"
#include "gameserver_message_define.h"
#include "public_message_define.h"
#include "roomserver_message_define.h"
#include "tunnelserver_message_define.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

class CFromClientEvent: public CBllBase
{
public:
	CFromClientEvent(){}
	virtual ~CFromClientEvent(){}

public:
	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL)
	{
		return S_OK;
	}

	int32_t OnMessage_get_fans(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QueryFansInfo(const RoleID roleid, CClientGetFansResp& respbody);

	int32_t OnMessage_netinfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t InsertRoomCollectionInfo(CRoomCollectInfoNotice* msgbody, const RoleID roleid);

	int32_t OnMessage_gold_exchange(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UpdateRoleExchange(const CGameSvrCurrencyExchangeReq* reqbody, CSvrGameCurrencyExchangeResp& respbody);

	int32_t OnMessage_get_exchangelog(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QueryExchangeLog(const CGameSvrGetExchangeLogReq* reqbody, CSvrGameGetExchangeLogResp& respbody);
	int32_t QueryTotalExchangeLogCount(const CGameSvrGetExchangeLogReq* reqbody, int32_t& totalcount);

	int32_t OnMessage_get_rolebaseinfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnMessage_update_asset(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UpdateGameAsset(const CGameSvrUpdateAssetReq& reqbody);
	int32_t QueryPlayerGoldBean(const RoleID roleid, uint32_t& goldbean);
	int32_t UpdatePlayerGoldBean(const CGameSvrUpdateAssetReq& reqbody, const uint32_t left_goldbean);

	int32_t OnMessage_GetRecharge(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QueryUserRecharge(const CGameSvrGetRechargeMsgReq& reqbody, CGameSvrGetRechargeMsgResp& respbody);


};

#define	CREATE_FROM_CLIENT_EVENT_INSTANCE	        CSingleton<CFromClientEvent>::CreateInstance
#define	GET_FROM_CLIENT_EVENT_INSTANCE		        CSingleton<CFromClientEvent>::GetInstance
#define	DESTROY_FROM_CLIENT_EVENT_INSTANCE	        CSingleton<CFromClientEvent>::DestroyInstance


FRAME_DBPROXY_NAMESPACE_END
#endif /* BLL_EVENT_CLIENTMSG_H_ */
