/*
 * bll_event_itemupdate_msg.cpp
 *
 *  Created on: 2013-3-5
 *      Author: liufl
 */

#include "bll_event_itemupdate_msg.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

int32_t CFromItemUpdateEvent::OnMessageBatchUpdateAsset(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: itemserver batch update asset!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CBatchUpdateAssetReq* pTmpBody = dynamic_cast<CBatchUpdateAssetReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CBatchUpdateAssetResp item_batchupdateasset;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBIS_BATCHUPDATEASSET_RESP);



	ret = ProcessItemBatchUpdateAsset(*pTmpBody, item_batchupdateasset);
	item_batchupdateasset.nResult = ret;
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process itemserver batch update asset request failed! errorcode=0x%08X\n",
				ret);

		g_Frame.PostMessage(&resphead, &item_batchupdateasset, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &item_batchupdateasset, nOptionLen, (const char*)pOptionData);
		return ret;
	}

	g_Frame.PostMessage(&resphead, &item_batchupdateasset, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &item_batchupdateasset, nOptionLen, (const char*)pOptionData);
	WRITE_DEBUG_LOG( "process itemserver batch update asset success! \n");

	return S_OK;
}
int32_t CFromItemUpdateEvent::ProcessItemBatchUpdateAsset(const CBatchUpdateAssetReq& reqbody, CBatchUpdateAssetResp& respbody)
{
	int32_t ret = S_OK;

	respbody.nResult = S_OK;
	respbody.nUpdateCount = 0;
	memset(respbody.arrRoleID, 0, sizeof(respbody.arrRoleID));

	//账单引擎对象
	CBillEngine& billEngine = GET_BILLENGINE_INSTANCE();

	//时间类
	CDateTime dt = CDateTime::CurrentDateTime();
	char szDateTime[enmMaxTimeStringLength] = {0};
	dt.ToDateTimeString(szDateTime);

	//缓存操作必须的参数声明
	char szKey[enmMaxMemcacheKeyLen] = {0};
	size_t keylen = 0;
	char szVal[enmMaxMemcacheValueLen] = {0};
	size_t vallen = 0;
	uint32_t uTmpValLen = 0;

	//memcache处理类对象
	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();

	//用户基本信息表结构
	RoleBaseInfo rolebaseinfo;

	//用户剩余资产
	uint32_t nLeftMoney = 0;

	//循环更新个人信息
	for(int32_t i=0; i<reqbody.nUpdateCount; ++i)
	{
		if(reqbody.arrAssetType[i] != enmAssetType_Money)
		{
			WRITE_WARNING_LOG( "asset type is not money! so just ignore! roleid=%d, assettype=%d, assetvalue=%d\n",
					reqbody.arrRoleID[i], reqbody.arrAssetType[i], reqbody.arrAssetValue[i]);
			continue;
		}
		//WRITE_DEBUG_LOG( "asset type is money! we will update it! roleid=%d, assettype=%d, assetvalue=%d\n",
		//		reqbody.arrRoleID[i], reqbody.arrAssetType[i], reqbody.arrAssetValue[i]);

		//更新数据库
		ret = UpdateRoleAsset(reqbody.arrRoleID[i], reqbody.arrUpdateAssetOperate[i],reqbody.arrAssetValue[i], reqbody.strTransID.GetString(), nLeftMoney);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("update role asset failed! roleid=%d, asset value=%d, errorcode=0x%08X \n",
					reqbody.arrRoleID[i],
					reqbody.arrAssetValue[i],
					ret);
			billEngine.WriteUpdateAssetFailBill(
					reqbody.strTransID.GetString(),
					reqbody.arrRoleID[i],
					nLeftMoney,
					reqbody.arrUpdateAssetOperate[i],
					reqbody.arrAssetValue[i],
					reqbody.arrCauseID[i],
					szDateTime,
					ret);
			continue;
		}
		//WRITE_DEBUG_LOG( "update role asset success! roleid=%d, asset value=%d \n",reqbody.arrRoleID[i], reqbody.arrAssetValue[i]);

		//删除网站的cache信息: 首先生成web那边的key的格式，然后删除之
//		GenerateMemcacheKeyForWeb(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoleID, reqbody.arrRoleID[i]);
//		ret = cacheobj.MemcacheDel(cacheobj.m_memc, szKey, keylen, 0);
//		if(0 > ret)
//		{
//			WRITE_ERROR_LOG("memcache delete failed: errorcode=0x%08X, key=%s, keylen=%d\n",
//					ret,
//					szKey,
//					keylen);
//			return ret;
//		}
		//WRITE_DEBUG_LOG( "memcache delete success! key=%s, keylen=%d\n",szKey,keylen);

		//将用户基本信息结构恢复到初始状态
		CVDCUserBaseInfo::VDCUserBaseInfoInit(rolebaseinfo);

		//查询该玩家基本信息
		ret = QueryRoleBaseInfo(reqbody.arrRoleID[i], rolebaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("query role base info failed! roleid=%d, errorcode=0x%08X \n",
					reqbody.arrRoleID[i], ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "query role base info from db success! roleid=%d \n",reqbody.arrRoleID[i]);

		//构造缓存信息
		memset(szVal, 0, sizeof(szVal));
		memset(szKey, 0, sizeof(szKey));
		vallen = 0;
		keylen = 0;
		uTmpValLen = 0;
		GenerateMemcacheKey(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoleID, reqbody.arrRoleID[i]);
		ret = CVDCUserBaseInfo::VDCUserBaseInfoEncode((uint8_t*)szVal, enmMaxMemcacheValueLen, uTmpValLen, rolebaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("encode role base info into memcache buffer faield! errorcode=0x%08X, roleid=%d \n",
					ret, reqbody.arrRoleID[i]);
			return ret;
		}
		//WRITE_DEBUG_LOG( "encode role base info into memcache buffer success@! congratulations! roleid=%d \n",reqbody.arrRoleID[i]);
		vallen = (size_t)uTmpValLen;

		//改写缓存
		ret = cacheobj.MemcacheSet(cacheobj.m_memc, szKey, keylen, szVal, vallen, 0);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("write player info into memcache failed! key=%s, errorcode=0x%08X \n",
					szKey, ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "write player info into memcache success! key=%s \n", szKey);

		//写资产更新账单
		billEngine.WriteUpdateAssetBill(reqbody.strTransID.GetString(), reqbody.arrRoleID[i], reqbody.arrCauseID[i], szDateTime, reqbody.arrAssetValue[i], nLeftMoney);

		//响应中的资产更新成功人数增加1个
		respbody.arrRoleID[respbody.nUpdateCount] = reqbody.arrRoleID[i];
		respbody.arrCauseID[respbody.nUpdateCount] = reqbody.arrCauseID[i];
		respbody.arrAssetType[respbody.nUpdateCount] = reqbody.arrAssetType[i];
		respbody.arrAssetValue[respbody.nUpdateCount] = reqbody.arrAssetValue[i];
		respbody.arrUpdateAssetOperate[respbody.nUpdateCount] = reqbody.arrUpdateAssetOperate[i];
		respbody.nUpdateCount++;
	}

	//如果一个人都没有更新成功，则返回错误
	if(0 >= respbody.nUpdateCount)
	{
		respbody.nResult = E_BATCH_UPDATE_ROLE_ASSET;
		return E_BATCH_UPDATE_ROLE_ASSET;
	}

	//一个或者一个以上的人更新成功， 返回S_OK
	respbody.nResult = S_OK;
	respbody.strTransID = reqbody.strTransID;
	return S_OK;
}
int32_t CFromItemUpdateEvent::UpdateRoleAsset(const RoleID roleid, const UpdateAssetOperate opmod, const uint32_t nMoney, const char* szTransID, uint32_t& nLeftMoney)
{
	int32_t ret = S_OK;

	if(0 == nMoney)
	{
		WRITE_WARNING_LOG( "update amount is zero , so just ignore it and return! transid=%s\n", szTransID);
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "update amount is not zero, so need to update it! transid=%s\n", szTransID);

	char szSql[enmMaxSqlStringLength] = {0};
	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	//首先查询余额
	sprintf(szSql, "select `Money` from vdc_user.user_base_info where `RoleID`=%d", roleid);
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("db operation of query player money failed! roleid=%d, errorcode=0x%08X, sql=%s\n",
				roleid, ret, szSql);
		return ret;
	}
	nLeftMoney = (uint32_t)atoi(arrRecordSet[0]);
	WRITE_DEBUG_LOG( "db operation of query player left money success! roleid=%d, nLeftMoney=%u, "
			"opmode=%d(1:add, 2:decrease, other: invalide), updateAmount=%u, sql=%s\n",
			roleid, nLeftMoney, opmod, nMoney, szSql);

	//memset(szSql, 0, sizeof(szSql));
	switch(opmod)
	{
		case enmUpdateAssetOperate_Increase:
		if((nLeftMoney + nMoney) > (uint32_t)enmMaxPlayerMoney)
		{
			WRITE_ERROR_LOG("player left money plus nMoney is more than max player money limit! \
													roleid=%d, leftmoney=%u, nMoney=%u, plusresult=%u, transid=%s\n",
					roleid, nLeftMoney, nMoney, nLeftMoney + nMoney, szTransID);
			return E_OVERRUN_MAX_PLAYER_MONEY;
		}
		sprintf(szSql, "update vdc_user.user_base_info set `Money`=`Money`+%u where `RoleID`=%d and `Money`+%u <= %u",
				nMoney, roleid, nMoney, (uint32_t)enmMaxPlayerMoney);
		break;
		case enmUpdateAssetOperate_Reduce:
		if(nLeftMoney < nMoney)
		{
			WRITE_ERROR_LOG("player left money is not enough!\
													roleid=%d, leftmoney=%u, nMoneyNeed=%u, transid=%s\n",
					roleid, nLeftMoney, nMoney, szTransID);
			return E_LEFTMONEY_NOT_ENOUGH;
		}
		sprintf(szSql, "update vdc_user.user_base_info set `Money`=`Money`-%u where `RoleID`=%d and `Money`-%u <= %u",
				nMoney, roleid, nMoney, (uint32_t)enmMaxPlayerMoney);
		break;
		default:
		WRITE_WARNING_LOG( "undefined asset update opmode! opmode=%d\n", opmod);
		return S_OK;
	}

	uint64_t nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG("Error: execute update on role asset failed! roleid=%d, errorcode=0x%08X, transid=%s, sql:%s\n",
				roleid, ret, szTransID, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "execute update on role asset success! roleid=%d, transid=%s, sql=%s\n",roleid, szTransID, szSql);

	if(0 == nAffectedRows)
	{
		WRITE_ERROR_LOG("Error: execute update role asset sql success but 0 rows affected! transid=%s, roleid=%d, sql=%s\n",
				szTransID, roleid, szSql);
		return E_UPDATE_ZERO_ROW_AFFETCTED;
	}
	//WRITE_DEBUG_LOG( "execute update role asset role asset sql success! szTransID=%s, roleid=%d, sql=%s\n",szTransID, roleid, szSql);

	return S_OK;
}

int32_t CFromItemUpdateEvent::OnMessageSetAchieve(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver get RoleInfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CUpdateIdentityNoti* pTmpBody = dynamic_cast<CUpdateIdentityNoti*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	ret = InsertUserAchievement(*pTmpBody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "insert set user achievement failed! errorcode=0x%08X, msgid=0x%08X\n",
				ret, MSGID_ISOS_UPDATEIDENTITY_NOTI);
		return ret;
	}
	//WRITE_DEBUG_LOG( "insert set user achievement success! threadindex=%d\n");

	return S_OK;
}
int32_t CFromItemUpdateEvent::InsertUserAchievement(const CUpdateIdentityNoti& msgbody)
{
	int32_t ret = S_OK;

	char szSql[enmMaxSqlStringLength] = {0};
	uint64_t nAffectedRows = 0;

	for(int8_t i=0; i<msgbody.nCount; ++i)
	{
		//memset(szSql, 0, sizeof(szSql));
		sprintf(szSql, "delete from vdc_user.user_achievement where achievement = %d", msgbody.nIdentityType[i]);
		ret = MYSQLREADENGINE.ExecuteDelete(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "delete previous lucky star failed! errorcode=0x%08X, sql=%s\n",
					ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "execute delete previous lucky star success!, sql=%s\n", szSql);

		//memset(szSql, 0, sizeof(szSql));
		sprintf(szSql, "insert into vdc_user.user_achievement(`roleid`, `achievement`) values(%d, %d)",
				msgbody.nRoleID[i], msgbody.nIdentityType[i]);
		ret = MYSQLREADENGINE.ExecuteInsert(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "insert lucky star failed! errorcode=0x%08X, sql=%s\n",
					ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "execute insert lucky start success!, sql=%s\n", szSql);
	}

	return S_OK;
}

int32_t CFromItemUpdateEvent::OnMessageSetJackpot(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: itemserver batch update asset!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CSetItemJackpotInfoReq* pTmpBody = dynamic_cast<CSetItemJackpotInfoReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	//	CBatchUpdateAssetResp item_batchupdateasset;
	//
	//	MessageHeadSS resphead;
	//	ObtainRespHead(pMsgHead, resphead,MSGID_DBIS_BATCHUPDATEASSET_RESP);

	ret = ProcessItemSetJackpot(*pTmpBody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process itemserver batch update asset request failed! errorcode=0x%08X\n",
				ret);
		return ret;
	}
	WRITE_DEBUG_LOG( "process itemserver batch update asset success! \n");

	//	item_batchupdateasset.nResult = S_OK;
	//	g_Frame.PostMessage(&resphead, &item_batchupdateasset, nOptionLen, (const char*)pOptionData);
	//	DumpMessage("", &resphead, &item_batchupdateasset, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromItemUpdateEvent::ProcessItemSetJackpot(const CSetItemJackpotInfoReq& reqbody)
{
	int32_t ret = S_OK;

	//更新物品奖池
	for(int32_t i=0; i<reqbody.nItemMultipeCount; ++i)
	{
		//更新当前item的奖池数据
		ret = UpdateItemJacketPot(reqbody.nItemID, reqbody.arrItemMultipe[i], reqbody.arrItemRatio[i],
				reqbody.arrItemRatioJackpot[i], reqbody.arrItemProbability[i], reqbody.arrItemProbabilityJackpot[i],
				reqbody.arrItemFlag[i]);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "update item jacket pot failure! errorcode=0x%08X, itemid=%d, itemmultiple=%d, itemjacketpot=%d, flag=%d\n",
					ret,
					reqbody.nItemID,
					reqbody.arrItemMultipe[i],
					reqbody.arrItemRatioJackpot[i],
					reqbody.arrItemFlag[i]);
			return ret;
		}
//		WRITE_DEBUG_LOG( "update item jacket pot success! itemid=0x%04X, itemmultiple=%d, itemjackepot=%d, flag=%d\n",
//				reqbody.nItemID,
//				reqbody.arrItemMultipe[i],
//				reqbody.arrItemJackpot[i],
//				reqbody.arrItemFlag[i]);
	}//end of for, index by j

//	WRITE_DEBUG_LOG( "update item jacketpot and store pool success! itemid=0x%08X, multiplecount=%d\n",
//			reqbody.nItemID, reqbody.nItemMultipeCount);

	WRITE_DEBUG_LOG( "update all item jacketpot and store pool success!\n");
	return S_OK;
}
int32_t CFromItemUpdateEvent::UpdateItemJacketPot(const ItemID itemid, const int32_t multiple,
		const int32_t ratio, const int32_t jacketpotAmount, const int32_t probability,
		const int32_t probabilityjackpot, const int8_t flag)
{
	int32_t ret = S_OK;

	//首先将该item的所有奖池的开关设置为关闭状态
	char szSql[enmMaxSqlStringLength] = {0};
	uint64_t nAffectedRows = 0;
	//sprintf(szSql, "update vdc_item.item_jackpot set `Flag`=%d where `ItemID`=%d", enmJackpotFlag_Close, itemid);
	//ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	//if(0 > ret)
	//{
	//	WRITE_ERROR_LOG( "error: execute update of set item jackpot flag to close status failed! errorcode=0x%08Xsql=%s\n",
	//		ret, szSql);
	//	return ret;
	//}
	//WRITE_DEBUG_LOG( "execute update of set item jackpot flag to close status success!sql=%s\n", szSql);

	sprintf(szSql, "insert into vdc_item.item_jackpot(`ItemID`, `Multipe`, `Ratio`, `RatioJackpot`, "
			"`Probability`, `ProbabilityJackpot`, `Flag`) "
			"values(%d, %d, %d, %d, %d, %d, %d) on duplicate key update `Ratio` =%d, `RatioJackpot`=%d,"
			"`Probability`=%d, `ProbabilityJackpot`=%d, `Flag`=%d",
			itemid,
			multiple,
			ratio,
			jacketpotAmount,
			probability,
			probabilityjackpot,
			flag,
			ratio,
			jacketpotAmount,
			probability,
			probabilityjackpot,
			flag);


	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "error: execute update item jacketpot failure! errorcode=0x%08X, sql=%s\n",
				ret,
				szSql);
		return ret;
	}
	WRITE_DEBUG_LOG( "execute update item jacketpot success! sql=%s\n", szSql);

	return S_OK;
}

int32_t CFromItemUpdateEvent::OnMessageSetSofa(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver get RoleInfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CSetSofaNoti* pTmpBody = dynamic_cast<CSetSofaNoti*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}

	ret = ProcessItemSetSofaNotice(*pTmpBody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "insert set user achievement failed! errorcode=0x%08X, msgid=0x%08X\n",
				ret, MSGID_ISOS_UPDATEIDENTITY_NOTI);
		return ret;
	}
	WRITE_DEBUG_LOG( "insert set user achievement success! threadindex=%d\n");

	return S_OK;
}
int32_t CFromItemUpdateEvent::ProcessItemSetSofaNotice(const CSetSofaNoti& reqbody)
{
	int32_t ret = S_OK;

	if(reqbody.nType == enmOperateSofa_Invalid)
	{
		WRITE_ERROR_LOG( "invalid operate room sofa type! type=%d\n",
				reqbody.nType);
		return E_INVALID_ARGUMENTS;
	}

	if(reqbody.nType == enmOperateSofa_Update)
	{
		if(0 >= reqbody.nRoleID)
		{
			WRITE_ERROR_LOG( "invalid room sofa roleid! roleid=%d\n",
					reqbody.nRoleID);
			return E_INVALID_ARGUMENTS;
		}

		if(0 >= reqbody.nRoomID)
		{
			WRITE_ERROR_LOG( "invalid room sofa roomid! roomid=%d\n",
					reqbody.nRoomID);
			return E_INVALID_ARGUMENTS;
		}

		if(reqbody.nSofaPos > (uint8_t)ROOM_SOFACOUNT)
		{
			//WRITE_DEBUG_LOG( "invalid room sofa position! position=%d\n",reqbody.nSofaPos);
			return E_INVALID_ARGUMENTS;
		}

		if(0 >= reqbody.nUpdateTime)
		{
			WRITE_ERROR_LOG( "invalid room sofa update time! updatetime=%d\n",
					reqbody.nUpdateTime);
			return E_INVALID_ARGUMENTS;
		}
	}

	ret = UpdateRoomSofa(reqbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update room sofa failed! errorcode=0x%08X\n", ret);
		return ret;
	}
	WRITE_DEBUG_LOG( "update room sofa success!\n");

	return S_OK;
}
int32_t CFromItemUpdateEvent::UpdateRoomSofa(const CSetSofaNoti& reqbody)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};
	uint64_t nAffectedRows = 0;

	CDateTime dt;
	char szDateTime[enmMaxDateTimeStringLength] = {0};

	if(reqbody.nType == enmOperateSofa_ClearAll)
	{
		//memset(szSql, 0, sizeof(szSql));
		sprintf(szSql, "update vdc_room.room_sofa set TicketCount = 0, RoleID=0, UpdateTime=0");
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "clear room sofa failed! errorcode=0x%08X, sql=%s\n",
					ret, szSql);
			return ret;
		}
		WRITE_DEBUG_LOG( "clear room sofa success!, sql=%s\n", szSql);

		return S_OK;
	}

	//memset(szSql, 0, sizeof(szSql));
	dt = CDateTime(reqbody.nUpdateTime * US_PER_SECOND);
	dt.ToDateTimeString(szDateTime);
	sprintf(szSql, "insert into vdc_room.room_sofa(`RoomID`, `SofaPosition`, `TicketCount`, `RoleID`, `UpdateTime`) "
			"values(%d, %d, %d, %d, '%s') on duplicate key update `TicketCount`=%d, `RoleID`=%d, `UpdateTime`='%s'",
			reqbody.nRoomID,
			reqbody.nSofaPos,
			reqbody.nSofaTicket,
			reqbody.nRoleID,
			szDateTime,
			reqbody.nSofaTicket,
			reqbody.nRoleID,
			szDateTime);

	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update room sofa info failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	WRITE_DEBUG_LOG( "update room sofa success!, sql=%s\n", szSql);

	return S_OK;
}

int32_t CFromItemUpdateEvent::OnMessageSetStorepool(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen , const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: itemserver set store pool!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CSetStorePoolInfoReq* pTmpBody = dynamic_cast<CSetStorePoolInfoReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CSetStorePoolInfoResp item_batchupdateasset;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBIS_SETSTOREPOOLINFO_RESP);

	ret = UpdateItemStorePool(0, pTmpBody->nStorePool);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process itemserver set store pool request failed! errorcode=0x%08X\n",
				ret);
		item_batchupdateasset.nResult = ret;
		g_Frame.PostMessage(&resphead, &item_batchupdateasset, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &item_batchupdateasset, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process itemserver set store pool success! \n");

	item_batchupdateasset.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &item_batchupdateasset, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &item_batchupdateasset, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromItemUpdateEvent::UpdateItemStorePool(const ItemID itemid, const int32_t storeAmount)
{
	//因为储备池与itemid无关，所以只填itemid=0，别的就非法
	if(itemid != enmInvalidItemID)
	{
		WRITE_ERROR_LOG( "invalide itemid on update item store pool! itemid=0x%08X\n", itemid);
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "itemid right on update item store pool! itemid=%d\n", itemid);

	int32_t ret = S_OK;

	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "insert into vdc_item.item_storepool(ItemID, StorePool) values(%d, %d) on duplicate key update `StorePool`=%d",
			itemid,
			storeAmount,
			storeAmount);

	uint64_t nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "error: execute update item storepool failure! errorcode=0x%08X, sql=%s\n",
				ret,
				szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "execute update item storepool success! affected rows=%d, sql=%s\n", nAffectedRows, szSql);

	return S_OK;
}

int32_t CFromItemUpdateEvent::OnMessageSetUpdateItem(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: itemserver update item!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CUpdateItemReq* pTmpBody = dynamic_cast<CUpdateItemReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CUpdateItemResp item_batchupdateasset;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBIS_UPDATEITEM_RESP);



	ret = ProcessItemUpdateItemReq(*pTmpBody, item_batchupdateasset);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process itemserver update item request failed! errorcode=0x%08X\n",
				ret);
		item_batchupdateasset.nResult = ret;
		g_Frame.PostMessage(&resphead, &item_batchupdateasset, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &item_batchupdateasset, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	//WRITE_DEBUG_LOG( "process itemserver update item success! \n");

	item_batchupdateasset.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &item_batchupdateasset, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &item_batchupdateasset, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromItemUpdateEvent::ProcessItemUpdateItemReq(const CUpdateItemReq& reqbody, CUpdateItemResp& respbody)
{
	//WRITE_DEBUG_LOG( "now begin to process itemserver update user item request!\n");

	//账单引擎对象
	CBillEngine& billEngine = GET_BILLENGINE_INSTANCE();
	//char szBillTransID[enmMaxBillTransIDLen] = {0};

	//时间类
	CDateTime dt = CDateTime::CurrentDateTime();
	char szDateTime[enmMaxTimeStringLength] = {0};
	dt.ToDateTimeString(szDateTime);

	int32_t ret = S_OK;
	if(reqbody.nItemCount >= enmMaxItemClassCount)
	{
		WRITE_ERROR_LOG( "error: one player cannot own so many item species! itemcount=%d, maxItemClassCount=%d\n",
				reqbody.nItemCount, enmMaxItemClassCount);
		return E_MAXCOUNT;
	}
	//WRITE_DEBUG_LOG( "update itemcount has no problem! update item count is %d\n", reqbody.nItemCount);

	//无须改写缓存，因此先更新数据库，然后写账单即可
	for(int32_t i=0; i<reqbody.nItemCount; ++i)
	{
		ret = UpdateUserItem(reqbody.nRoleID, reqbody.arrUpdateOperate[i], reqbody.arrItemUnit[i]);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "update user item info failed! roleid=%d, itemid=%d, errorcode=0x%08X\n",
					reqbody.nRoleID,
					reqbody.arrItemUnit[i].nItemID,

					ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "update user item info success! roleid=%d, itemid=%d\n",	reqbody.nRoleID,reqbody.arrItemUnit[i].nItemID);

		////写账单
		billEngine.WriteItemUpdateBill(reqbody.strTransID.GetString(), reqbody.nRoleID, szDateTime, reqbody.arrCauseID[i], reqbody.arrItemUnit[i]);

		//更新响应结构中的更新成功的物品的个数
		memcpy(&respbody.arrItemUnit[respbody.nItemCount], &reqbody.arrItemUnit[i], sizeof(reqbody.arrItemUnit[i]));
		respbody.arrUpdateOperate[respbody.nItemCount] = reqbody.arrUpdateOperate[i];
		respbody.nItemCount++;
	}

	if(respbody.nItemCount <= 0)
	{
		WRITE_ERROR_LOG( "no user item has been updated successful! roleid=%d\n", reqbody.nRoleID);
		respbody.nResult = E_UPDATE_USER_ITEMINFO;
		respbody.nRoleID = reqbody.nRoleID;
		return E_UPDATE_USER_ITEMINFO;
	}
//	WRITE_DEBUG_LOG( "update user item info success! total request itemcount=%d, real update count=%d, roleid=%d\n",
//			reqbody.nItemCount,
//			respbody.nItemCount,
//			reqbody.nRoleID);

	respbody.nResult = S_OK;
	respbody.nRoleID = reqbody.nRoleID;
	respbody.strTransID = reqbody.strTransID;

	return S_OK;
}
int32_t CFromItemUpdateEvent::UpdateUserItem(const RoleID roleid, const int32_t opmode, const ItemUnit& itemunit)
{
	char szSql[enmMaxSqlStringLength] = {0};
	switch(opmode)
	{
		case enmUpdateItemOperate_Add:
		case enmUpdateItemOperate_Update:
		sprintf(szSql, "insert into vdc_user.user_item(`RoleID`, `ItemID`, `UseMeans`, `Quantity`, `OwnTick`) values(%d, %d, %d, %d, %d) on duplicate key update `UseMeans`=%d, `Quantity`=%d, `OwnTick`=%d",
				roleid,
				itemunit.nItemID,
				itemunit.nUseMeans,
				itemunit.nQuantity,
				itemunit.nOwnTick,
				itemunit.nUseMeans,
				itemunit.nQuantity,
				itemunit.nOwnTick);
		WRITE_DEBUG_LOG( "add or update item! roleid=%d, itemid=%d, opmode=%d, quantity=%d\n",
				roleid,
				itemunit.nItemID,
				opmode,
				itemunit.nQuantity);
		break;
		case enmUpdateItemOperate_Del:
		sprintf(szSql, "delete from vdc_user.user_item where `RoleID`=%d and `ItemID`=%d", roleid, itemunit.nItemID);
		WRITE_DEBUG_LOG( "delete item! roleid=%d, itemid=%d, opmode=%d, quantity=%d\n",
				roleid,
				itemunit.nItemID,
				opmode,
				itemunit.nQuantity);
		break;
		default:
		WRITE_WARNING_LOG("warning: unknown user item update mode! operation mode=%d\n", opmode);
		return S_OK;
	}

	int32_t ret = S_OK;
	uint64_t nAffectedRows = 0;

	if(opmode == enmUpdateItemOperate_Add || opmode == enmUpdateItemOperate_Update)
	{
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	}
	else
	{
		ret = MYSQLREADENGINE.ExecuteDelete(szSql, nAffectedRows);
	}

	if(0 > ret)
	{
		WRITE_ERROR_LOG( "error: update user item db operation failed! roleid=%d, itemid=%d, opmode=%d, quantity=%d, errorcode=0x%08X, sql=%s\n",
				roleid,
				itemunit.nItemID,
				opmode,
				itemunit.nQuantity,
				ret,

				szSql);
		return ret;
	}
	WRITE_DEBUG_LOG( "update user item db operation success! roleid=%d, itemid=%d, opmode=%d, quantity=%d, sql=%s\n",
			roleid,
			itemunit.nItemID,
			opmode,
			itemunit.nQuantity,

			szSql);

	return S_OK;
}

int32_t CFromItemUpdateEvent::OnMessageSetLuckyuser(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver get RoleInfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CSetLuckyUserNoti* pTmpBody = dynamic_cast<CSetLuckyUserNoti*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	ret = UpdateLuckyUser(*pTmpBody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "insert set user achievement failed! errorcode=0x%08X, msgid=0x%08X\n",
				ret, MSGID_ISOS_UPDATEIDENTITY_NOTI);
		return ret;
	}
	//WRITE_DEBUG_LOG( "insert set user achievement success! threadindex=%d\n");

	return S_OK;
}
int32_t CFromItemUpdateEvent::UpdateLuckyUser(const CSetLuckyUserNoti& reqbody)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	uint64_t nAffectedRows = 0;

	for(int32_t i = 0; i < reqbody.nCount; ++i)
	{
		//memset(szSql, 0, sizeof(szSql));
		if(reqbody.arrLuckyCount[ i ] == 0)
		{
			sprintf(szSql,"delete from vdc_item.`lucky_user` where `ItemID`=%d and `ItemPrice`=%d and `RoleID`=%d and `LuckyMul`=%d",
					reqbody.arrItemID[ i ],
					reqbody.arrPrice[ i ],
					reqbody.arrRoleID[ i ],
					reqbody.arrLuckyTime[ i ]
					);
		}
		else
		{
			sprintf(szSql, "insert into vdc_item.`lucky_user`(`RoleID`, `ItemID`, `ItemPrice`, `LuckyMul`, `LuckyCount`) "
					"values(%d, %d, %d, %d, %d) on duplicate key update `LuckyCount`=%d ",
					reqbody.arrRoleID[i],
					reqbody.arrItemID[i],
					reqbody.arrPrice[i],
					reqbody.arrLuckyTime[i],
					reqbody.arrLuckyCount[i],
					reqbody.arrLuckyCount[i]);
		}
		//WRITE_DEBUG_LOG("update song list sql:: %s",szSql);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("update lucky user  failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
			return ret;
		}
	}
	return S_OK;
}

int32_t CFromItemUpdateEvent::OnMessageSetPriceJackpot(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: set price jackpot!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CSetJackpotMoneyNoti* pTmpBody = dynamic_cast<CSetJackpotMoneyNoti*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	ret = UpdatePriceJackpot(*pTmpBody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "insert set price jack pot failed! errorcode=0x%08X\n",
				ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "insert set price jack pot success! threadindex=%d\n");

	return S_OK;
}
int32_t CFromItemUpdateEvent::UpdatePriceJackpot(const CSetJackpotMoneyNoti& reqbody)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	uint64_t nAffectedRows = 0;

	for(int32_t i = 0; i < reqbody.nCount; ++i)
	{
		//memset(szSql, 0, sizeof(szSql));
		sprintf(szSql,"insert into vdc_item.price_jackpot(`ItemPrice`, `Multipe`, `JackPotMoney`)"
				"values(%d, %d, %d) on duplicate key update `JackPotMoney`=%d",
				reqbody.arrnPrice[i],
				reqbody.arrMultipe[i],
				reqbody.arrnMoney[i],
				reqbody.arrnMoney[i]) ;
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "update price_jackpot  failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
			return ret;
		}
	}
	return S_OK;
}


FRAME_DBPROXY_NAMESPACE_END
