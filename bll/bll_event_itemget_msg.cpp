/*
 * bll_event_itemget_msg.cpp
 *
 *  Created on: 2013-3-5
 *      Author: liufl
 */

#include "bll_event_itemget_msg.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

int32_t CFromItemGetEvent::OnMessage_getiteminfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: itemserver get role item Info!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetRoleItemInfoReq* pTmpBody = dynamic_cast<CGetRoleItemInfoReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetRoleItemInfoResp get_roleiteminfo;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBIS_GETROLEITEMINFO_RESP);

	ret = ProcessItemGetItemInfoReq(*pTmpBody, get_roleiteminfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process itemserver get role item Info request failed! errorcode=0x%08X\n",
				ret);
		get_roleiteminfo.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process itemserver get role item Info success! \n");

	get_roleiteminfo.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromItemGetEvent::ProcessItemGetItemInfoReq(const CGetRoleItemInfoReq& reqbody, CGetRoleItemInfoResp& respbody)
{
	if(reqbody.nUserCount <= 0)
	{
		WRITE_ERROR_LOG( "user count in get role item info request is invalid! usercount=%d\n",
				reqbody.nUserCount);
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "user count in get role item info request is right! usercount=%d\n",reqbody.nUserCount);

	int32_t ret = S_OK;

	respbody.nUserCount = 0;
	memset(respbody.arrRoleID, 0, sizeof(respbody.arrRoleID));
	memset(respbody.arrItemUnit, 0, sizeof(respbody.arrItemUnit));
	memset(respbody.arrItemCount, 0, sizeof(respbody.arrItemCount));

	RoleID nTmpRoleID = 0;
	ItemUnit stArrTmpItemUnit[MaxOnUserItemCount];
	memset(stArrTmpItemUnit, 0, sizeof(stArrTmpItemUnit));
	uint16_t usTmpItemCount = 0;

	for(int32_t i=0; i<reqbody.nUserCount; ++i)
	{
		nTmpRoleID = reqbody.arrRoleID[i];
		usTmpItemCount = 0;
		memset(stArrTmpItemUnit, 0, sizeof(stArrTmpItemUnit));

		//数据库查询玩家物品
		ret = QueryRoleItemInfo(nTmpRoleID, usTmpItemCount, stArrTmpItemUnit);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "query role item info from db failed! roleid=%d, errorcode=0x%08X\n",
					nTmpRoleID,
					ret);
			continue;
		}
		//WRITE_DEBUG_LOG( "query role item info from db success! roleid=%d\n", nTmpRoleID);

		if(usTmpItemCount >= MaxOnUserItemCount)
		{
			WRITE_ERROR_LOG( "one player cannot own so many item species!roleid=%d, user item count=%d, maxitemcount one user can own is %d\n",
					nTmpRoleID,
					usTmpItemCount,
					MaxOnUserItemCount);
			continue;
		}
		//WRITE_DEBUG_LOG( "this player has %d species of items, roleie=%d, own item count=%d\n",usTmpItemCount,nTmpRoleID,usTmpItemCount);

		//给响应结构中的字段赋值
		respbody.arrRoleID[respbody.nUserCount] = nTmpRoleID;
		respbody.arrItemCount[respbody.nUserCount] = usTmpItemCount;
		for(uint16_t i=0; i<usTmpItemCount; ++i)
		{
			memcpy(&respbody.arrItemUnit[respbody.nUserCount][i], &stArrTmpItemUnit[i], sizeof(stArrTmpItemUnit[i]));
		}
		respbody.nUserCount++;
	}

	if(respbody.nUserCount <= 0)
	{
		WRITE_WARNING_LOG( "itemserver get user item info from db success! but we get no result! usercount=%d\n",
				respbody.nUserCount);
		//return E_ITEM_GET_USERITEMINFO;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "itemserver get user item info from db success! usercount=%d\n",respbody.nUserCount);

	return S_OK;
}
int32_t CFromItemGetEvent::QueryRoleItemInfo(const RoleID roleid, uint16_t& itemcount, ItemUnit arrItem[])
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `ItemID`, `UseMeans`, `Quantity`, `OwnTick` from vdc_user.user_item where `RoleID`=%d", roleid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQueryBegin(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: db operation on query role item info failed! roleid=%d, errorcode=0x%08X, sql=%s\n",
				roleid, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query role item info success! roleid=%d, sql=%s\n",roleid, szSql);

	//查询到的记录的条数
	itemcount = nRowCount;

	//取出所有查询到的记录（多条记录）
	for(uint32_t i=0; i<nRowCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "error: db operation of fetch row on query role item info failed! roleid=%d\n", roleid);
			return ret;
		}
		//WRITE_DEBUG_LOG( "db operation of fetch row on query role item success! roleid=%d\n", roleid);

		arrItem[i].nItemID = atoi(arrRecordSet[0]);
		arrItem[i].nUseMeans = atoi(arrRecordSet[1]);
		arrItem[i].nQuantity = atoi(arrRecordSet[2]);
		arrItem[i].nOwnTick = atoi(arrRecordSet[3]);
	}

	return S_OK;
}

int32_t CFromItemGetEvent::OnMessage_getstorepool(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: itemserver get store pool!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetStorePoolInfoReq* pTmpBody = dynamic_cast<CGetStorePoolInfoReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetStorePoolInfoResp get_storepool;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBIS_GETSTOREPOOLINFO_RESP);

	ret = ProcessItemGetStorePool(*pTmpBody, get_storepool);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process itemserver get store pool request failed! errorcode=0x%08X\n",
				ret);
		get_storepool.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_storepool, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_storepool, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process itemserver get store pool success! \n");

	get_storepool.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_storepool, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_storepool, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromItemGetEvent::ProcessItemGetStorePool(const CGetStorePoolInfoReq& reqbody, CGetStorePoolInfoResp& respbody)
{
	if(enmInvalidItemID != reqbody.nItemID)
	{
		WRITE_WARNING_LOG( "itemid is not right! itemid=%d\n", reqbody.nItemID);
		respbody.nResult = E_INVALID_ARGUMENTS;
		return E_INVALID_ARGUMENTS;
	}

	//根据itemid查询storepool
	int32_t ret = S_OK;
	ret = QueryStorePool(reqbody.nItemID, respbody.nStorePool);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("process itemserver get storepool request failed! itemid=0x%04X, errorcode=0x%08X\n",
				reqbody.nItemID, ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "process itemserver get storepool request success! itemid=0x%04X\n",reqbody.nItemID);

	return S_OK;
}
int32_t CFromItemGetEvent::QueryStorePool(const ItemID itemid, int32_t& storepool)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select  `StorePool`  from  vdc_item.`item_storepool`");

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("db operation on query storepool failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query storepool success! rowcount=%d, sql=%s\n", nRowCount, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in db of storepool!, sql=%s\n",szSql);
		storepool = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist of storepool\n");

	storepool = (int32_t)atoi(arrRecordSet[0]);

	return S_OK;
}

int32_t CFromItemGetEvent::OnMessage_getjackpot(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: itemserver get role jackpot!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetItemJackpotInfoReq* pTmpBody = dynamic_cast<CGetItemJackpotInfoReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetItemJackpotInfoResp get_roleiteminfo;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBIS_GETITEMJACKPOTINFO_RESP);

	ret = ProcessItemGetJackpot(*pTmpBody, get_roleiteminfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process itemserver get role jackpot request failed! errorcode=0x%08X\n",
				ret);
		get_roleiteminfo.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process itemserver get role jackpot success! \n");

	get_roleiteminfo.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromItemGetEvent::ProcessItemGetJackpot(const CGetItemJackpotInfoReq& reqbody, CGetItemJackpotInfoResp& respbody)
{
	if(reqbody.nStartIndex < 0 || reqbody.nWantCount <= 0)
	{
		WRITE_ERROR_LOG( "invalid argument on CGetItemJackpotInfoReq! nStartIndex=%d, nWantedCount=%d\n",
				reqbody.nStartIndex, reqbody.nWantCount);
		respbody.nResult = E_INVALID_ARGUMENTS;
		return E_INVALID_ARGUMENTS;
	}

	int32_t ret = S_OK;
	ret = QueryItemJacketpot(reqbody, respbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query item jacket pot failed! errorcode=0x%08X, startindex=%d, wantedcount=%d\n",
				ret, reqbody.nStartIndex, reqbody.nWantCount);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query item jacketpot info success!\n");

	respbody.nResult = S_OK;
	return S_OK;
}
int32_t CFromItemGetEvent::QueryItemJacketpot(const CGetItemJackpotInfoReq& reqbody, CGetItemJackpotInfoResp& respbody)
{
	if(reqbody.nStartIndex < 0 || reqbody.nWantCount < 0)
	{
		WRITE_ERROR_LOG( "invalid argument while query item jacketpot! startindex=%d, wantedcount=%d\n",
				reqbody.nStartIndex, reqbody.nWantCount);
		respbody.nResult = E_INVALID_ARGUMENTS;
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "argument all right while query item jacketpot!\n");

	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select DISTINCT `ItemID`  from vdc_item.`item_jackpot` limit %d, %d",
			reqbody.nStartIndex, reqbody.nWantCount);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;

	//首先查询itemid字段
	ret = MYSQLREADENGINE.ExecuteQueryBegin(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query item jacketpot storepool failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query item jacketpot storepool success! sql=%s\n", szSql);

	respbody.nItemCount = (int32_t)nRowCount;

	for(uint32_t i=0; i<nRowCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "fetch row of query item jacketpot info failure! errorcode=0x%08X\n", ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "fetch row of query item jacketpot info success!\n");

		respbody.arrItemID[i] = (ItemID)atoi(arrRecordSet[0]);

		//WRITE_DEBUG_LOG( "fetch item jacketpot info success! itemcount=%d\n",respbody.nItemCount);
	}

	//memset(szSql, 0, sizeof(szSql));
	for(int32_t i=0; i<respbody.nItemCount; ++i)
	{
		//memset(szSql, 0, sizeof(szSql));
		sprintf(szSql, "select `Multipe`, `Ratio`, `RatioJackpot`, `Probability`, `ProbabilityJackpot`,"
				" `Flag` from vdc_item.`item_jackpot` where `ItemID`=%d\n",
				respbody.arrItemID[i]);
		ret = MYSQLREADENGINE.ExecuteQueryBegin(szSql, nFieldCount, nRowCount);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "db operation on query item jacketpot failed! errorcode=0x%08X, sql=%s\n",
					ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "db operation on query item jacketpot success! sql=%s\n", szSql);

		respbody.arrItemMultipeCount[i] = nRowCount;

		for(uint32_t j = 0; j < nRowCount; ++j)
		{
			//memset(arrRecordSet, 0, sizeof(arrRecordSet));
			ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "fetch row of query item jacketpot failed! errorcode=0x%08X, sql=%s\n",
						ret, szSql);
				return ret;
			}
			//WRITE_DEBUG_LOG( "fetch row of query item jacketpot success! sql=%s\n", szSql);

			respbody.arrItemMultipe[i][j] = (uint16_t)atoi(arrRecordSet[0]);
			respbody.arrItemRatio[i][j] = (int32_t)atoi(arrRecordSet[1]);
			respbody.arrItemRatioJackpot[i][j] = (int32_t)atoi(arrRecordSet[2]);
			respbody.arrItemProbability[i][j] = (int32_t)atoi(arrRecordSet[3]);
			respbody.arrItemProbabilityJackpot[i][j] = (int32_t)atoi(arrRecordSet[4]);
			respbody.arrItemFlag[i][j] = (int8_t)atoi(arrRecordSet[5]);
		}
		//WRITE_DEBUG_LOG( "query item jacketpot of itemid=%d success!\n", respbody.arrItemID[i]);
	}

	//WRITE_DEBUG_LOG( "query all item jacketpot success! itemcount=%d\n", respbody.nItemCount);

	return S_OK;
}

int32_t CFromItemGetEvent::OnMessage_getPlayerMoney(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: itemserver get player money!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetOnlinePlayerMoneyReq* pTmpBody = dynamic_cast<CGetOnlinePlayerMoneyReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetOnlinePlayerMoneyResp get_roleiteminfo;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_ISDB_GETONLINEPLAYERMONEY_RESP);

	ret = ProcessItemGetOnlinePlayerMoney(*pTmpBody, get_roleiteminfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process itemserver get player money request failed! errorcode=0x%08X\n",
				ret);
		get_roleiteminfo.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process itemserver get player money success! \n");

	get_roleiteminfo.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromItemGetEvent::ProcessItemGetOnlinePlayerMoney(const CGetOnlinePlayerMoneyReq& reqbody, CGetOnlinePlayerMoneyResp& respbody)
{
	if(0 == reqbody.nCount)
	{
		WRITE_WARNING_LOG( "player count is zero in item get online player money request!\n");
		return S_OK;
	}

	if(0 > reqbody.nCount || reqbody.nCount > MaxGetOnlinePlayerLefMoneyCount)
	{
		WRITE_ERROR_LOG( "invalid argument! count=%d\n", reqbody.nCount);
		return E_INVALID_ARGUMENTS;
	}

	int32_t ret = S_OK;
	int32_t nLeftMoney = 0;
	MagnateLevel nMagnateLevel = 0;
	uint64_t nConsumeAmt;
	uint64_t nNextLevelConsume;
	respbody.nCount = 0;
	for(int32_t i=0; i<reqbody.nCount; ++i)
	{
		nMagnateLevel = 0;
		nConsumeAmt = 0;
		nNextLevelConsume = 0;

		//首先从memcache中获取
		ret = GetPlayerMoneyFromMemcache(reqbody.nRoleID[i], nLeftMoney, nMagnateLevel, nConsumeAmt, nNextLevelConsume);
		if(0 > ret) //memcache查询失败则尝试从DB获取
		{
			WRITE_WARNING_LOG( "get player money from memcache failed! roleid=%d, errrorcode=0x%08X\n",
					reqbody.nRoleID[i], ret);

			//失败继续从DB查
			ret = QueryRoleAsset(reqbody.nRoleID[i], nLeftMoney, nMagnateLevel, nConsumeAmt, nNextLevelConsume);

			//失败则进行下一个
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "get player money from DB failed! roleid=%d, errorcode=0x%08X\n",
						reqbody.nRoleID[i], ret);
				continue;
			}
			else
			{
				//db查询成功则增加一个成功的记录
				respbody.nRoleID[respbody.nCount] = reqbody.nRoleID[i];
				respbody.nMoney[respbody.nCount] = (uint32_t)nLeftMoney;
				respbody.nMagnateLevel[respbody.nCount] = nMagnateLevel;
				respbody.nConsumeAmt[respbody.nCount] = nConsumeAmt;
				respbody.nNextLevelConsume[respbody.nCount] = nNextLevelConsume;
				if((uint32_t)respbody.nMoney[respbody.nCount] >= enmMaxPlayerMoney)
				{
					return E_MAX_MONEY_AMOUNT;
				}
				respbody.nCount++;
			}
		}
		else //memcache查询成功则进行下一个
		{
			respbody.nRoleID[respbody.nCount] = reqbody.nRoleID[i];
			respbody.nMoney[respbody.nCount] = (uint32_t)nLeftMoney;
			respbody.nMagnateLevel[respbody.nCount] = nMagnateLevel;
			respbody.nConsumeAmt[respbody.nCount] = nConsumeAmt;
			respbody.nNextLevelConsume[respbody.nCount] = nNextLevelConsume;

			if((uint32_t)respbody.nMoney[respbody.nCount] >= enmMaxPlayerMoney)
			{
				return E_MAX_MONEY_AMOUNT;
			}
			respbody.nCount++;
		}
	}

	if(respbody.nCount <= 0)
	{
		respbody.nResult = E_ZERO_COUNT;
		WRITE_ERROR_LOG( "get none of online player money from both memcache and db!\n");
		return respbody.nResult;
	}

	return S_OK;
}
int32_t CFromItemGetEvent::GetPlayerMoneyFromMemcache(const RoleID roleid, int32_t& nLeftMoney, MagnateLevel& magnatelevel, uint64_t& consumeamt, uint64_t& nextlevelconsume)
{
	int32_t ret = S_OK;

	//写缓存必须的参数声明
	char szKey[enmMaxMemcacheKeyLen] = {0};
	size_t keylen = 0;
	size_t vallen = 0;
	char* pVal = NULL;
	uint32_t offset = 0;

	//玩家基本信息结构
	RoleBaseInfo rolebaseinfo;
	CVDCUserBaseInfo::VDCUserBaseInfoInit(rolebaseinfo);

	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();

	//构造key
	GenerateMemcacheKey(szKey,enmMaxMemcacheKeyLen, keylen, enmStoreType_RoleID, roleid);

	//首先查找cache中是否存在
	ret = cacheobj.MemcacheIfKeyExist(cacheobj.m_memc, szKey, keylen);
	if(0 > ret)
	{
		//cache中不存在此人
		WRITE_NOTICE_LOG( "notice: this role is not exist in cache! we need to query its role info from db! roleid=%d\n",
				roleid);
		return E_NOT_EXIST_IN_MEMCACHE;
	}

	ret = cacheobj.MemcacheGet(cacheobj.m_memc, szKey, keylen, pVal, vallen);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "get value from memcache failed! errorcode=0x%08X\n", ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "get value from memcache success! roleid=%d\n", roleid);

	//解析val，得到userbaseinfo结构
	offset = 0;
	ret = CVDCUserBaseInfo::VDCUserBaseInfoDecode((uint8_t *)pVal, enmMaxMemcacheValueLen, offset, rolebaseinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "decode role base info get from cache failed! roleid=%d, errorcode=0x%08X",
				roleid, ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "decode role base info get from cache success! roleid=%d\n",roleid);

	nLeftMoney = rolebaseinfo.nMoney;
	magnatelevel = rolebaseinfo.nMagnateLevel;
	consumeamt = rolebaseinfo.nConsume;
	nextlevelconsume = rolebaseinfo.nNextLevelConusme;

	if((uint32_t)nLeftMoney >= (uint32_t)enmMaxPlayerMoney)
	{
		WRITE_ERROR_LOG( "money amount is filled out! max than %u !\n", enmMaxPlayerMoney);
		return E_MAX_MONEY_AMOUNT;
	}

	return S_OK;
}
int32_t CFromItemGetEvent::QueryRoleAsset(const RoleID roleid, int32_t& nLeftAsset, MagnateLevel& magnatelevel, uint64_t& consumeamt, uint64_t& nextlevelconsume)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `Money`, `MagnateLevel`, `ConsumeAmt`, `NextLevelConsume`  from vdc_user.user_base_info where `RoleID` = %d", roleid);
	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	//ret = m_mysqlEngine.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0>ret)
	{
		//WRITE_ERROR_LOG("Error: DB operation failed on query role asset for roleid=%d!\n", roleid);
		WRITE_ERROR_LOG( "error: DB operation failed on query role asset, roleid=%d, sql=%s\n",
				roleid, szSql);
		return E_QUERY_ROLE_ASSET;
	}
	//WRITE_DEBUG_LOG( "db operation success on query role asset,roleid=%d, sql=%s\n",roleid, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such a role in db, rolied=%d\n", roleid);
		nLeftAsset = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "this role is exists in db! roleid=%d\n", roleid);

	nLeftAsset = atoi(arrRecordSet[0]);
	magnatelevel = (MagnateLevel)atoi(arrRecordSet[1]);
	consumeamt = (uint64_t)atol(arrRecordSet[2]);
	nextlevelconsume = (uint64_t)atol(arrRecordSet[3]);

	if((uint32_t)nLeftAsset >= enmMaxPlayerMoney)
	{
		WRITE_ERROR_LOG( "player money is filled out! more than max amount %u \n", enmMaxPlayerMoney);
		return E_MAX_MONEY_AMOUNT;
	}

	return S_OK;
}

int32_t CFromItemGetEvent::OnMessage_getSofa(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: itemserver get sofa!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetSofaReq* pTmpBody = dynamic_cast<CGetSofaReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetSofaResp get_roleiteminfo;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_ISDB_GETSOFA_RESP);

	ret = QueryRoomSofa(*pTmpBody, get_roleiteminfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process itemserver get sofa request failed! errorcode=0x%08X\n",
				ret);
		get_roleiteminfo.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process itemserver get sofa success! \n");

	get_roleiteminfo.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromItemGetEvent::QueryRoomSofa(const CGetSofaReq& reqbody, CGetSofaResp& respbody)
{
	int32_t ret = S_OK;

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));
	char szSql[enmMaxSqlStringLength] = {0};

	respbody.nRoomID = reqbody.nRoomID;

	//memset(szSql, 0, sizeof(szSql));
	nFieldCount = 0;
	nRowCount = 0;
	sprintf(szSql, "select "
			"SofaPosition, TicketCount, base.RoleID, 179id, Gender, VipLevel, MagnateLevel, RoleName, unix_timestamp(UpdateTime) "
			"from "
			"vdc_room.room_sofa as sofa, vdc_user.user_base_info as base, vdc_user.user_account as acc "
			"where sofa.roleid = base.roleid and sofa.roleid = acc.179uin "
			"and roomid=%d", reqbody.nRoomID);

	CMysqlGuard mysqlguard(MYSQLREADENGINE);
	ret = mysqlguard.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query room sofa failed! errrocode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query room sofa success! sql=%s\n", szSql);

	respbody.nRoomID = reqbody.nRoomID;

	if(nRowCount == 0)
	{
		WRITE_WARNING_LOG( "this room has no sofa data yet! roomid=%d\n",
				reqbody.nRoomID);
		return S_OK;
	}

	char szName[MaxRoleNameLength] = {0};
	for(uint32_t i=0; i<nRowCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "fetch room sofa failed! roomid=%d, errorcode=0x%08X\n",
					reqbody.nRoomID, ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "fetch room sofa success!\n");

		respbody.nSofaPos[i] = (uint8_t)atoi(arrRecordSet[0]);
		respbody.nSofaTicket[i] = (uint32_t)atoi(arrRecordSet[1]);
		respbody.nRoleID[i] = (RoleID)atoi(arrRecordSet[2]);
		respbody.nAccountID[i] = (AccountID)atoi(arrRecordSet[3]);
		respbody.nGender[i] = (Gender)atoi(arrRecordSet[4]);
		respbody.nVipLevel[i] = (VipLevel)atoi(arrRecordSet[5]);
		respbody.nMagnateLevel[i] = (MagnateLevel)atoi(arrRecordSet[6]);
		memset(szName, 0, sizeof(szName));
		strcpy_safe(szName, MaxRoleNameLength, arrRecordSet[7], strlen(arrRecordSet[7]));
		respbody.arrStrRoleName[i] = szName;
		respbody.nUpdateTime[i] = (uint32_t)atoi(arrRecordSet[8]);
	}

	return S_OK;
}

int32_t CFromItemGetEvent::OnMessage_getStarPlayer(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: itemserver get starplayer!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	//CGetStarPlayerReq* pTmpBody = dynamic_cast<CGetStarPlayerReq*>(pMsgBody);
	CGetStarPlayerResp get_roleiteminfo;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_ISDB_GETSTARPLAYER_RESP);

	ret = QueryUserAchievement(get_roleiteminfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process itemserver get starplayer request failed! errorcode=0x%08X\n",
				ret);
		get_roleiteminfo.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process itemserver get starplayer success! \n");

	get_roleiteminfo.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromItemGetEvent::QueryUserAchievement(CGetStarPlayerResp& respbody)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `roleid`, `achievement`  from vdc_user.user_achievement");

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;

	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: db operation on query user achievement failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query user achievement success!, sql=%s\n", szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "this player has no user achievement record yet!\n");
		respbody.nCount = 0;
		return S_OK;
	}

	//查询到的记录的条数
	if(nRowCount >= (uint32_t)MaxIdentityCount)
	{
		respbody.nCount = MaxIdentityCount;
	}
	else
	{
		respbody.nCount = nRowCount;
	}

	//取出所有查询到的记录（多条记录）
	for(int32_t i=0; i<respbody.nCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "error: db operation of fetch row on query room max spend once failed!\n");
			return ret;
		}
		//WRITE_DEBUG_LOG( "db operation of fetch row on query room max spend once success! roleid=%d\n");

		respbody.nRoleID[i] = atoi(arrRecordSet[0]);
		respbody.nIdentity[i] = atoi(arrRecordSet[1]);
	}

	return S_OK;
}

int32_t CFromItemGetEvent::OnMessage_getluckyuser(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: itemserver get luckyuser!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetLuckyUserReq* pTmpBody = dynamic_cast<CGetLuckyUserReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetLuckyUserResp get_roleiteminfo;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBIS_GETLUCKYUSER_RESP);

	ret = QueryLuckyUser(*pTmpBody, get_roleiteminfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process itemserver get luckyuser request failed! errorcode=0x%08X\n",
				ret);
		get_roleiteminfo.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process itemserver get luckyser success! \n");

	get_roleiteminfo.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromItemGetEvent::QueryLuckyUser(const CGetLuckyUserReq& reqbody, CGetLuckyUserResp& respbody)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	sprintf(szSql, "select `RoleID`, `ItemID`, `ItemPrice`, `LuckyMul`, `LuckyCount` from vdc_item.`lucky_user` "
			"limit %d,%d",
			reqbody.nStartIndex,
			reqbody.nWantCount
			);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: db operation failed!  errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation success!  sql=%s\n", szSql);

	//查询到的记录的条数
	if(0 == nRowCount)
	{
		//WRITE_DEBUG_LOG( "this user is not admin in any room! sql=%s\n", szSql);
		respbody.nCount = 0;
		return S_OK;
	}

	respbody.nCount = 0;
	//取出所有查询到的记录（多条记录）
	for(uint32_t i=0; i<nRowCount; ++i)
	{
		//ret = mysqlguard.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "error: db operation of fetch row on query role roomadmin info failed! \n");
			continue;
		}
		//WRITE_DEBUG_LOG( "db operation of fetch row on query role roomadmin success!\n");

		respbody.arrRoleID[ respbody.nCount ] = (int32_t)atoi(arrRecordSet[0]);
		respbody.arrItemID[ respbody.nCount ] = (int32_t)atoi(arrRecordSet[1]);
		respbody.arrPrice[ respbody.nCount ] = (int32_t)atoi(arrRecordSet[2]);
		respbody.arrLuckyTime[ respbody.nCount ] = (int32_t)atoi(arrRecordSet[3]);
		respbody.arrLuckyCount[ respbody.nCount ] = (int32_t)atoi(arrRecordSet[4]);


		respbody.nCount++;
	}

	return S_OK;
}

int32_t CFromItemGetEvent::OnMessage_getpricejackpot(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: itemserver get price jackpot!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetJackpotMoneyReq* pTmpBody = dynamic_cast<CGetJackpotMoneyReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetJackpotMoneyResp get_roleiteminfo;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead, MSGID_DBIS_GETJACKPOTMONEY_RESP);

	ret = QueryPriceJackpot(*pTmpBody, get_roleiteminfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process itemserver get price jackpot request failed! errorcode=0x%08X\n",
				ret);
		get_roleiteminfo.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	//WRITE_DEBUG_LOG( "process itemserver get price jackpot success! \n");

	get_roleiteminfo.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_roleiteminfo, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromItemGetEvent::QueryPriceJackpot(const CGetJackpotMoneyReq& reqbody, CGetJackpotMoneyResp& respbody)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	sprintf(szSql, "select `ItemPrice`,`Multipe`, `JackPotMoney` from vdc_item.`price_jackpot` ");

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: db operation failed!  errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG("db query pricejackpot operation success!  sql=%s\n", szSql);

	//查询到的记录的条数
	if(0 == nRowCount)
	{
		//WRITE_DEBUG_LOG("no price jackpot msg! sql=%s\n", szSql);
		respbody.nCount = 0;
		return S_OK;
	}

	respbody.nCount = 0;
	//取出所有查询到的记录（多条记录）
	for(uint32_t i=0; i<nRowCount; ++i)
	{
		//ret = mysqlguard.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "error: db operation of fetch row on query price jackpot info failed! \n");
			continue;
		}
		respbody.arrPrice[ respbody.nCount ] = (int32_t)atoi(arrRecordSet[0]);
		respbody.arrMultipe[ respbody.nCount ] = (int32_t)atoi(arrRecordSet[1]);
		respbody.arrMoney[ respbody.nCount ] = (int32_t)atoi(arrRecordSet[2]);

		respbody.nCount++;
	}

	return S_OK;
}

FRAME_DBPROXY_NAMESPACE_END
