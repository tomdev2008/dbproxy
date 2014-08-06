/*
 * bll_event_itemget_msg.h
 *
 *  Created on: 2013-3-5
 *      Author: liufl
 */

#ifndef BLL_EVENT_ITEMGET_MSG_H_
#define BLL_EVENT_ITEMGET_MSG_H_

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
#include "itemserver_message_define.h"


FRAME_DBPROXY_NAMESPACE_BEGIN

class CFromItemGetEvent: public CBllBase
{
public:
	CFromItemGetEvent(){}
	virtual ~CFromItemGetEvent(){}

public:
	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL)
	{
		return S_OK;
	}
public:
	int32_t OnMessage_getiteminfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessItemGetItemInfoReq(const CGetRoleItemInfoReq& reqbody, CGetRoleItemInfoResp& respbody);
	int32_t QueryRoleItemInfo(const RoleID roleid, uint16_t& itemcount, ItemUnit arrItem[]);

	int32_t OnMessage_getstorepool(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessItemGetStorePool(const CGetStorePoolInfoReq& reqbody, CGetStorePoolInfoResp& respbody);
	int32_t QueryStorePool(const ItemID itemid, int32_t& storepool);

	int32_t OnMessage_getjackpot(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessItemGetJackpot(const CGetItemJackpotInfoReq& reqbody, CGetItemJackpotInfoResp& respbody);
	int32_t QueryItemJacketpot(const CGetItemJackpotInfoReq& reqbody, CGetItemJackpotInfoResp& respbody);

	int32_t OnMessage_getPlayerMoney(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessItemGetOnlinePlayerMoney(const CGetOnlinePlayerMoneyReq& reqbody, CGetOnlinePlayerMoneyResp& respbody);
	int32_t GetPlayerMoneyFromMemcache(const RoleID roleid, int32_t& nLeftMoney, MagnateLevel& magnatelevel, uint64_t& consumeamt, uint64_t& nextlevelconsume);
	int32_t QueryRoleAsset(const RoleID roleid, int32_t& nLeftAsset, MagnateLevel& magnatelevel, uint64_t& consumeamt, uint64_t& nextlevelconsume);

	int32_t OnMessage_getSofa(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QueryRoomSofa(const CGetSofaReq& reqbody, CGetSofaResp& respbody);

	int32_t OnMessage_getStarPlayer(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QueryUserAchievement(CGetStarPlayerResp& respbody);

	int32_t OnMessage_getluckyuser(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QueryLuckyUser(const CGetLuckyUserReq& reqbody, CGetLuckyUserResp& respbody);

	int32_t OnMessage_getpricejackpot(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t QueryPriceJackpot(const CGetJackpotMoneyReq& reqbody, CGetJackpotMoneyResp& respbody);


};

#define	CREATE_FROM_ITEMGET_EVENT_INSTANCE	        CSingleton<CFromItemGetEvent>::CreateInstance
#define	GET_FROM_ITEMGET_EVENT_INSTANCE		        CSingleton<CFromItemGetEvent>::GetInstance
#define	DESTROY_FROM_ITEMGET_EVENT_INSTANCE	        CSingleton<CFromItemGetEvent>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END

#endif /* BLL_EVENT_ITEMGET_MSG_H_ */
