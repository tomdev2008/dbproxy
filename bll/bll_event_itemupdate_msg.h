/*
 * bll_event_itemupdate_msg.h
 *
 *  Created on: 2013-3-5
 *      Author: liufl
 */

#ifndef BLL_EVENT_ITEMUPDATE_MSG_H_
#define BLL_EVENT_ITEMUPDATE_MSG_H_
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

class CFromItemUpdateEvent: public CBllBase
{
public:
	CFromItemUpdateEvent(){}
	virtual ~CFromItemUpdateEvent(){}

public:
	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL)
	{
		return S_OK;
	}
public:
	int32_t OnMessageBatchUpdateAsset(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessItemBatchUpdateAsset(const CBatchUpdateAssetReq& reqbody, CBatchUpdateAssetResp& respbody);
	int32_t UpdateRoleAsset(const RoleID roleid, const UpdateAssetOperate opmod, const uint32_t nMoney, const char* szTransID, uint32_t& nLeftMoney);

	int32_t OnMessageSetAchieve(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t InsertUserAchievement(const CUpdateIdentityNoti& msgbody);

	int32_t OnMessageSetJackpot(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessItemSetJackpot(const CSetItemJackpotInfoReq& reqbody);
	int32_t UpdateItemJacketPot(const ItemID itemid, const int32_t multiple,
			const int32_t ratio, const int32_t jacketpotAmount, const int32_t probability,
			const int32_t probabilityjackpot, const int8_t flag);

	int32_t OnMessageSetSofa(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessItemSetSofaNotice(const CSetSofaNoti& reqbody);
	int32_t UpdateRoomSofa(const CSetSofaNoti& reqbody);

	int32_t OnMessageSetStorepool(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UpdateItemStorePool(const ItemID itemid, const int32_t storeAmount);

	int32_t OnMessageSetUpdateItem(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessItemUpdateItemReq(const CUpdateItemReq& reqbody, CUpdateItemResp& respbody);
	int32_t UpdateUserItem(const RoleID roleid, const int32_t opmode, const ItemUnit& itemunit);

	int32_t OnMessageSetLuckyuser(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UpdateLuckyUser(const CSetLuckyUserNoti& reqbody);

	int32_t OnMessageSetPriceJackpot(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UpdatePriceJackpot(const CSetJackpotMoneyNoti& reqbody);

};

#define	CREATE_FROM_ITEMUPDATE_EVENT_INSTANCE	        CSingleton<CFromItemUpdateEvent>::CreateInstance
#define	GET_FROM_ITEMUPDATE_EVENT_INSTANCE		        CSingleton<CFromItemUpdateEvent>::GetInstance
#define	DESTROY_FROM_ITEMUPDATE_EVENT_INSTANCE	        CSingleton<CFromItemUpdateEvent>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END

#endif /* BLL_EVENT_ITEMUPDATE_MSG_H_ */
