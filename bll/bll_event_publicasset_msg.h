/*
 * bll_event_publicasset_msg.h
 *
 *  Created on: 2013-3-5
 *      Author: liufl
 */

#ifndef BLL_EVENT_PUBLICASSET_MSG_H_
#define BLL_EVENT_PUBLICASSET_MSG_H_
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
#include "public_message_define.h"


FRAME_DBPROXY_NAMESPACE_BEGIN

class CFromPublicUpdateEvent: public CBllBase
{
public:
	CFromPublicUpdateEvent(){}
	virtual ~CFromPublicUpdateEvent(){}

public:
	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL)
	{
		return S_OK;
	}
public:
	int32_t OnMessagePublicAssetEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen =0, const void *pOptionData = NULL );
	int32_t ProcessPubUpAssetReq(const CUpdateAssetReq& reqbody, CUpdateAssetResp& respbody);
	int32_t UpdateRoleAsset(const RoleID roleid, const UpdateAssetOperate opmod, const uint32_t nMoney, const char* szTransID, uint32_t& nLeftMoney);
	int32_t UpdateMagnate(const RoleID roleid, const MagnateLevel magnatelevel,
			const uint64_t spendsum, const uint64_t nextlevelspend);

};

#define	CREATE_FROM_PUBLICUPDATE_EVENT_INSTANCE	        CSingleton<CFromPublicUpdateEvent>::CreateInstance
#define	GET_FROM_PUBLICUPDATE_EVENT_INSTANCE		        CSingleton<CFromPublicUpdateEvent>::GetInstance
#define	DESTROY_FROM_PUBLICUPDATE_EVENT_INSTANCE	        CSingleton<CFromPublicUpdateEvent>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END

#endif /* BLL_EVENT_PUBLICASSET_MSG_H_ */
