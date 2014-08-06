/*
 * bll_event_clientmsg.cpp
 *
 *  Created on: 2013-2-26
 *      Author: liufl
 */

#include "bll_event_clientmsg.h"

FRAME_DBPROXY_NAMESPACE_BEGIN
//获取粉丝榜数据信息
int32_t CFromClientEvent::OnMessage_get_fans(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: client get fans!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CClientGetFansReq* pGetFansReq = dynamic_cast<CClientGetFansReq*>(pMsgBody);
	if(pGetFansReq == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CClientGetFansResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_CLIENTDB_GETFANS_RESP);

	ret = QueryFansInfo(pGetFansReq->nRoleID, respbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query artist fans info failed!{ artistroleid=%d, errorcode=0x%08X}\n",
				pGetFansReq->nRoleID, ret);

		respbody.nResult = ret;

		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

		return ret;
	}
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;

}
int32_t CFromClientEvent::QueryFansInfo(const RoleID roleid, CClientGetFansResp& respbody)
{
	int32_t ret = S_OK;

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	//查询日粉丝榜
	CDateTime dt = CDateTime::CurrentDateTime();
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select "
			"base.roleid, rolename, sum(itemprice * itemcount) as sumconsume "
			"from "
			"vdc_bill.user_item_record_%.4d_%.2d_%.2d as record, vdc_user.user_base_info as base, vdc_user.user_account as acc "
			"where "
			"base.roleid = record.srcroleid and base.roleid = acc.179uin "
			"and "
			"dstroleid=%d and dstroleid != srcroleid "
			"and "
			"RoleIDentity>=0 "
			"and "
			"RoleIDentity <=4 "
			"group by(record.srcroleid) "
			"order by sumconsume desc, VipLevel desc, MagnateLevel desc, 179ID asc limit %d",
			dt.Year(),
			dt.Month(),
			dt.Day(),
			roleid,
			MaxFansCountPerType);

	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("db operation on query player day fans failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG("db operation on query player day fans success! rowcount=%d, sql=%s\n",
	//		nRowCount, szSql);

	respbody.nCountDay = nRowCount;

	if(respbody.nCountDay > MaxFansCountPerType)
	{
		respbody.nCountDay = MaxFansCountPerType;
	}

	char szName[MaxRoleNameLength] = {0};
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));
	for(uint32_t i=0; i<nRowCount && i<MaxFansCountPerType; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "fetch current day fans failed! errorcode=0x%08X\n", ret);
			return ret;
		}
		respbody.arrDayRoleID[i] = (RoleID)atoi(arrRecordSet[0]);
		memset(szName, 0, sizeof(szName));
		strcpy_safe(szName, MaxRoleNameLength, arrRecordSet[1], strlen(arrRecordSet[1]));
		respbody.arrDayRoleName[i] = szName;
		respbody.arrDayMoney[i] = (uint32_t)atoi(arrRecordSet[2]);
	}
	//WRITE_DEBUG_LOG( "fetch current day fans success!\n");

	//查询月粉丝榜
	//memset(szSql, 0, sizeof(szSql));
	nFieldCount = 0;
	nRowCount = 0;
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));
	sprintf(szSql, "select "
			"base.roleid, rolename, Amount "
			"from "
			"vdc_user.user_base_info as base, vdc_user.month_fans as fans, vdc_user.user_account as acc "
			"where "
			"base.roleid = fans.srcroleid and base.roleid = acc.179uin "
			"and `Month`=%d "
			"and "
			"`DstRoleID`=%d and dstroleid != srcroleid "
			"and "
			"RoleIDentity >= 0 "
			"and "
			"RoleIDentity <= 4 "
			"order by `Amount` desc, VipLevel desc, MagnateLevel desc, 179ID asc limit %d",
			dt.Year()*100 + dt.Month(),
			roleid, MaxFansCountPerType);
	MYSQLREADENGINE.ExecuteQueryEnd();
//	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("db operation on query player month fans failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG("db operation on query player month fans success! rowcount=%d, sql=%s\n",
	//		nRowCount, szSql);

	respbody.nCountMonth = nRowCount;
	if(respbody.nCountMonth > MaxFansCountPerType)
	{
		respbody.nCountMonth = MaxFansCountPerType;
	}

	//memset(arrRecordSet, 0, sizeof(arrRecordSet));
	for(uint32_t i=0; i<nRowCount && i<MaxFansCountPerType; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "fetch current month fans failed! errorcode=0x%08X\n", ret);
			return ret;
		}
		respbody.arrMonthRoleID[i] = (RoleID)atoi(arrRecordSet[0]);
		memset(szName, 0, sizeof(szName));
		strcpy_safe(szName, MaxRoleNameLength, arrRecordSet[1], strlen(arrRecordSet[1]));
		respbody.arrMonthRoleName[i] = szName;
		respbody.arrMonthMoney[i] = (uint32_t)atoi(arrRecordSet[2]);
	}
	//WRITE_DEBUG_LOG("fetch current month fans success!\n");

	//查询超级粉丝榜
	int32_t yearmonth = 0;
	yearmonth = dt.Year()*100 + dt.Month();
	//memset(szSql, 0, sizeof(szSql));
	nFieldCount = 0;
	nRowCount = 0;
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));
	sprintf(szSql, "select "
			"base.roleid, rolename, amount "
			"from "
			"vdc_user.user_base_info as base, vdc_user.super_fans as fans, vdc_user.user_account as acc "
			"where "
			"base.roleid = fans.srcroleid and base.roleid = acc.179uin "
			"and "
			"`DstRoleID`=%d and dstroleid != srcroleid "
			"and "
			"RoleIDentity >= 0 "
			"and "
			"RoleIDentity <= 4 "
			"order by `Amount` desc, VipLevel desc, MagnateLevel desc, 179ID asc limit %d",
			roleid, MaxFansCountPerType);

	ret = S_OK;

	MYSQLREADENGINE.ExecuteQueryEnd();
//	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("db operation on query room channel failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG("db operation on query room channel success! rowcount=%d, sql=%s\n",
	//		nRowCount, szSql);

	respbody.nCountAll = nRowCount;
	if(respbody.nCountAll > MaxFansCountPerType)
	{
		respbody.nCountAll = MaxFansCountPerType;
	}

	//memset(arrRecordSet, 0, sizeof(arrRecordSet));
	for(uint32_t i=0; i<nRowCount && i<MaxFansCountPerType; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("fetch current super fans failed! errorcode=0x%08X\n", ret);
			return ret;
		}
		respbody.arrAllRoleID[i] = (RoleID)atoi(arrRecordSet[0]);
		memset(szName, 0, sizeof(szName));
		strcpy_safe(szName, MaxRoleNameLength, arrRecordSet[1], strlen(arrRecordSet[1]));
		respbody.arrAllRoleName[i] = szName;
		respbody.arrAllMoney[i] = (uint32_t)atoi(arrRecordSet[2]);
	}
	//WRITE_DEBUG_LOG( "fetch current super fans success!\n");
	//WRITE_DEBUG_LOG("query player day, month, and super fans success!\n");

	return S_OK;
}

//写入音、视频数据包信息
int32_t CFromClientEvent::OnMessage_netinfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: count net info package!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	CRoomCollectInfoNotice* pCollectInfo = dynamic_cast<CRoomCollectInfoNotice*>(pMsgBody);
	if(pCollectInfo == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}

	ret = InsertRoomCollectionInfo(pCollectInfo, pMsgHead->nRoleID);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "insert room collection info failed! errorcode=0x%08X, roleid=%d, roomid=%d, msgid=0x%08X\n",
				ret, pMsgHead->nRoleID, pMsgHead->nRoomID, MSGID_TSDB_ROOM_COLLECTION_INF0_NOTICE);
		return ret;
	}
	//WRITE_DEBUG_LOG("insert room collection info success! \n");

	return S_OK;
}
int32_t CFromClientEvent::InsertRoomCollectionInfo(CRoomCollectInfoNotice* msgbody, const RoleID roleid)
{
	char szSql[enmMaxSqlStringLength] = {0};
	uint64_t nAffectedRows = 0;

	char key[MaxRoomCollectionKeyLen] = {0};
	char content[MaxRoomCollectionContentLen] = {0};

	//依次为： nettype,  media,  ip, lost_ratio
	char res[MaxFieldCountInNetStatus][MaxFieldLenInNetStatus];
	int32_t field_count = 0;

	char szIP[enmMaxIPAddressLength] = {0};
	char szNetType[MaxNetTypeLen] = {0};
	int32_t nRatio = 0;

	//视频上传丢包率， 视频下载丢包率， 音频上传丢包率， 音频下载丢包率
	int32_t nLost[4] = {-1, -1, -1, -1};

	//需要写的sql语句的条数
	//int32_t sqlCount = 0;

	memset(res, 0, sizeof(res));

	if(0 >= msgbody->nCount)
	{
		WRITE_WARNING_LOG("key count is zero in client net status statistics!\n");
		return S_OK;
	}

	//视频上传，视频下载，音频上传，音频下载， 总共最多4个key，超过4，则不予处理
	if(4 < msgbody->nCount)
	{
		WRITE_WARNING_LOG( "key count is more than 4 in client net status statistics!\n");
		return S_OK;
	}

	int32_t ret = S_OK;
	CDateTime dt = CDateTime::CurrentDateTime();
	char szDatetime[enmMaxDateTimeStringLength] = {0};
	dt.ToDateTimeString(szDatetime);

	//从数据包中获取ip和网络类型
	char szContent0[MaxRoomCollectionContentLen] = {0};
	strcpy(szContent0, msgbody->strCollectionContent[0].GetString());
	get_ip_nettype(szContent0, res, MaxFieldCountInNetStatus, field_count, szIP, szNetType);

	char szSqlProcess[enmMaxSqlStringLength] = {0};

	memset(res, 0, sizeof(res));

	for(uint16_t i=0; i<msgbody->nCount; ++i)
	{
		//memset(szSql, 0, sizeof(szSql));
		nAffectedRows = 0;
		memset(key, 0, MaxRoomCollectionKeyLen);
		memset(content, 0, MaxRoomCollectionContentLen);
		memset(res, 0, sizeof(res));
		nRatio = 0;
		if(MaxRoomCollectionKeyLen <= msgbody->strCollectionKey[i].GetStringLength())
		{
			return E_INVALID_ARGUMENTS;
		}
		strncpy(key, msgbody->strCollectionKey[i].GetString(), msgbody->strCollectionKey[i].GetStringLength());

		if(MaxRoomCollectionContentLen <= msgbody->strCollectionContent[i].GetStringLength())
		{
			return E_INVALID_ARGUMENTS;
		}
		strncpy(content, msgbody->strCollectionContent[i].GetString(), msgbody->strCollectionContent[i].GetStringLength());

		if(strcmp(szCollectionKeyUpPackLost, key) == 0)
		{
			analysis_content(content, res, MaxFieldCountInNetStatus, field_count, enmTransDirection_Up, nLost);
		}

		if(strcmp(szCollectionKeyDownPackLost, key) == 0)
		{
			analysis_content(content, res, MaxFieldCountInNetStatus, field_count, enmTransDirection_Down, nLost);
		}

		if(strcmp(szProcess, key) == 0)
		{
			memset(szSqlProcess, 0, sizeof(szSqlProcess));
			sprintf(szSqlProcess, "insert into vdc_stats.player_client_process(`RoleID`, `ProcessName`, `StatsTime`) values(%d, '%s', '%s')",
					roleid, content, szDatetime);
		}
	}

	//如果视频上传和下载其中一个不等于-1， 则有视频信息
	if( (nLost[0] != -1) || (nLost[1] != -1) )
	{
		//memset(szSql, 0, sizeof(szSql));
		sprintf(szSql, "insert into vdc_stats.room_collection_info_%.4d_%.2d_%.2d(`roleid`, `keytype`, `nettype`, `mediatype`, `ip`, `uplost_ratio`, `downlost_ratio`, `collect_time`)"
				"values(%d, '%s', '%s', '%s', '%s', %d, %d, %d)", dt.Year(), dt.Month(), dt.Day(), roleid, "PackLost", szNetType, "Video", szIP, nLost[0], nLost[1], (int32_t)dt.Seconds());
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "execute insert video lost info failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "execute insert video lost info success!, sql=%s\n", szSql);
	}

	//如果音频上传和下载其中有一个不等于-1， 则有音频信息
	if( (nLost[2] != -1) || (nLost[3] != -1) )
	{
		//memset(szSql, 0, sizeof(szSql));
		sprintf(szSql, "insert into vdc_stats.room_collection_info_%.4d_%.2d_%.2d(`roleid`, `keytype`, `nettype`, `mediatype`, `ip`, `uplost_ratio`, `downlost_ratio`, `collect_time`)"
				"values(%d, '%s', '%s', '%s', '%s', %d, %d, %d)", dt.Year(), dt.Month(), dt.Day(), roleid, "PackLost", szNetType, "Audio", szIP, nLost[2], nLost[3], (int32_t)dt.Seconds());
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("execute insert audio lost info failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG("execute insert audio lost info success!, sql=%s\n", szSql);
	}

	if(0 >= strlen(szSqlProcess))
	{
		WRITE_WARNING_LOG("this player has no other process like 9158, guagua, etc! roleid=%d\n", roleid);
		return S_OK;
	}

	nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSqlProcess, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("execute insert player client process info failed! errorcode=0x%08X, roleid=%d\n",
				ret, roleid);
		return ret;
	}
	//WRITE_DEBUG_LOG("execute insert player client process info success! roleid=%d\n", roleid);

	return S_OK;
}

//处理金币兑换请求
int32_t CFromClientEvent::OnMessage_gold_exchange(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: user exchange gold!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	CSvrGameCurrencyExchangeResp respbody;
	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead, MESGIID_SVRGAME_CURRENCY_EXCHANGE_RESP);
	//进入房间获取角色信息消息体
	CGameSvrCurrencyExchangeReq* pExchangeReq = dynamic_cast<CGameSvrCurrencyExchangeReq*>(pMsgBody);
	if(pExchangeReq == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}

	ret = UpdateRoleExchange(pExchangeReq, respbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("update player exchange info failed! errorcode=0x%08X\n",
				ret);
		respbody.nResult = ret;

		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update player exchange info success!\n");

	RoleBaseInfo rolebaseinfo;
	//memset(&rolebaseinfo, 0, sizeof(rolebaseinfo));
	ret = QueryRoleBaseInfo(pExchangeReq->nRoleID, rolebaseinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query player base info failed! errorcode=0x%08X\n", ret);
		respbody.nResult = ret;

		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query player base info success!\n");

	respbody.nResult = S_OK;
	respbody.nRoleID = pExchangeReq->nRoleID;
	respbody.n179Coin = rolebaseinfo.nMoney;
//	respbody.nGoldBean = rolebaseinfo.nGoldBean;

	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromClientEvent::UpdateRoleExchange(const CGameSvrCurrencyExchangeReq* reqbody, CSvrGameCurrencyExchangeResp& respbody)
{
	if(reqbody->nRoleID == enmInvalidRoleID)
	{
		WRITE_ERROR_LOG("roleid is invalid!\n");
		return E_INVALID_ARGUMENTS;
	}

	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};
	uint64_t nAffectedRows = 0;

	CDateTime dt = CDateTime::CurrentDateTime();

	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "insert into vdc_client_game.exchange_log(`RoleID`, `ItemID`, `ItemCount`, `ItemPrice`, `Amount`, `optime`) "
			"values(%d, %d, %d, %d, %d, %d)",
			reqbody->nRoleID,
			reqbody->nItemID,
			reqbody->nNumber,
			reqbody->nPrice,
			reqbody->nAmount,
			(int32_t)dt.Seconds());
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("insert exchange log failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG("insert exchage log success!, sql=%s\n", szSql);

	return S_OK;
}

//处理获取兑换记录的请求
int32_t CFromClientEvent::OnMessage_get_exchangelog(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: client get fans!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	CSvrGameGetExchangeLogResp respbody;
	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MESGIID_SVRGAME_GET_EXCHANGE_LOG_RESP);

	CGameSvrGetExchangeLogReq* pReqBody = dynamic_cast<CGameSvrGetExchangeLogReq*>(pMsgBody);
	if(pReqBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}

	ret = QueryExchangeLog(pReqBody, respbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("query player exchange log failed! errorcode=0x%08X\n",
				ret);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}

	ret = QueryTotalExchangeLogCount(pReqBody, respbody.nTotalCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("query total exchange log total count is failed! errorcode=0x%08X\n",
				ret);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG("query total exchange log count success! msgid=0x%08X, totalcount=%d\n",
			MESGIID_SVRGAME_GET_EXCHANGE_LOG_RESP, respbody.nTotalCount);
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromClientEvent::QueryExchangeLog(const CGameSvrGetExchangeLogReq* reqbody, CSvrGameGetExchangeLogResp& respbody)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};
	int32_t nStartIndex = (reqbody->nPage - 1) * reqbody->nPageSize;

	if(reqbody->nRoleID == enmInvalidRoleID)
	{
		sprintf(szSql, "select `179id`, `optime`, `itemid`, `itemcount`, `amount` from vdc_client_game.`exchange_log` as log, vdc_user.user_account as acc "
				"where "
				"log.roleid = acc.179uin and optime >= %d and optime <= %d order by optime desc limit %d, %d",
				reqbody->nBeginTime, reqbody->nEndTime, nStartIndex, reqbody->nPageSize);

	}

	if(reqbody->nRoleID > 0)
	{
		sprintf(szSql, "select `179id`, `optime`, `itemid`, `itemcount`, `amount` from vdc_client_game.`exchange_log` as log, vdc_user.user_account as acc "
				"where `RoleID`=%d "
				"and log.roleid = acc.179uin "
				"and optime >= %d and optime <= %d order by optime desc limit %d, %d",
				reqbody->nRoleID, reqbody->nBeginTime, reqbody->nEndTime, nStartIndex, reqbody->nPageSize);

	}
	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));


	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: db operation failed! roleid=%d, errorcode=0x%08X, sql=%s\n",
				reqbody->nRoleID, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG("db operation success! roleid=%d, sql=%s\n", reqbody->nRoleID, szSql);

	//查询到的记录的条数
	if(0 == nRowCount)
	{
		WRITE_DEBUG_LOG("this user is not admin in any room! roleid=%d, sql=%s\n", reqbody->nRoleID, szSql);
		respbody.nCount = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG("this user is admin in some room! we will fetch it below! roleid=%d, sql=%s\n",
	//		reqbody->nRoleID, szSql);

	respbody.nCount = 0;
	//取出所有查询到的记录（多条记录）
	for(uint32_t i=0; i<nRowCount; ++i)
	{
		//ret = mysqlguard.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "error: db operation of fetch row on query role roomadmin info failed! roleid=%d\n",
					reqbody->nRoleID);
			continue;
		}
		//WRITE_DEBUG_LOG("db operation of fetch row on query role roomadmin success! roleid=%d\n",
		//		reqbody->nRoleID);

		respbody.arr179ID[respbody.nCount] = (int32_t)atoi(arrRecordSet[0]);
		respbody.arrExchangeTime[respbody.nCount] = (int32_t)atoi(arrRecordSet[1]);
		respbody.arrItemID[respbody.nCount] = (int32_t)atoi(arrRecordSet[2]);
		respbody.arrExchangeNum[respbody.nCount] = (int32_t)atoi(arrRecordSet[3]);
		respbody.arrAmount[respbody.nCount] = (int32_t)atoi(arrRecordSet[4]);

		respbody.nCount++;
	}

	ret = QueryTotalExchangeLogCount(reqbody, respbody.nTotalCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("query total exchange log count failed! errorcode=0x%08X\n",
				ret);
		return ret;
	}
	WRITE_DEBUG_LOG("query total exchange log count success! msgid=0x%08X, roleid=%d, totalcount=%d\n",
			MESGIID_SVRGAME_GET_EXCHANGE_LOG_RESP, reqbody->nRoleID, respbody.nTotalCount);

	return S_OK;
}
int32_t CFromClientEvent::QueryTotalExchangeLogCount(const CGameSvrGetExchangeLogReq* reqbody, int32_t& totalcount)
{
	char szSql[enmMaxSqlStringLength] = {0};
	//memset(szSql, 0, sizeof(szSql));
	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	if(reqbody->nRoleID == enmInvalidRoleID)
	{
		sprintf(szSql, "select count(*) from vdc_client_game.exchange_log where optime >=%d and optime<=%d",
				reqbody->nBeginTime, reqbody->nEndTime);
	}
	else
	{
		sprintf(szSql, "select count(*) from vdc_client_game.exchange_log where roleid=%d and optime>=%d and optime<=%d",
				reqbody->nRoleID, reqbody->nBeginTime, reqbody->nEndTime);
	}

	int32_t ret = S_OK;

	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("db operation of query total exchange log count failed! roleid=%d, errorcode=0x%08X, sql=%s\n",
				reqbody->nRoleID, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG("db operation of query total exchange log count success! roleid=%d, sql=%s\n",
	//		reqbody->nRoleID, szSql);

	if(arrRecordSet[0] == NULL)
	{
		totalcount = 0;
	}
	else
	{
		totalcount = (int32_t)atoi(arrRecordSet[0]);
	}

	//WRITE_DEBUG_LOG("totalcount is %d, sql=%s\n", totalcount, szSql);

	return S_OK;
}

//取玩家基本信息
int32_t CFromClientEvent::OnMessage_get_rolebaseinfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: client get fans!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGameSvrGetRoleInfoReq *pGetRoleInfoReq = dynamic_cast<CGameSvrGetRoleInfoReq*>(pMsgBody);
	if(pGetRoleInfoReq == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CSvrGameGetRoleBaseInfoResp respbody;
	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_SVRGAME_GET_ROLE_BASEINFO_RESP);

	if(enmInvalidRoleID == pGetRoleInfoReq->nRoleID)
	{
		WRITE_ERROR_LOG( "invalid roleid!\n");
		return E_INVALID_ARGUMENTS;
	}

	RoleBaseInfo baseinfo;
	//memset(&baseinfo, 0, sizeof(baseinfo));

	ret = QueryRoleBaseInfo(pGetRoleInfoReq->nRoleID, baseinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query player base info failed! roleid=%d, errorcode=0x%08X\n",
				pGetRoleInfoReq->nRoleID, ret);
		respbody.nResult = ret;

		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}

	uint32_t n179ID = 0;
	ret = QueryRole179ID(pGetRoleInfoReq->nRoleID, n179ID);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("query player 179id failed! roleid=%d, errorcode=0x%08X\n",
				pGetRoleInfoReq->nRoleID, ret);
		respbody.nResult = ret;

		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}

	respbody.n179ID = (int32_t)n179ID;
	respbody.nRoleID = pGetRoleInfoReq->nRoleID;
	respbody.strName = baseinfo.szRoleName;
	respbody.nVipLevel = baseinfo.ucVipLevel;
	respbody.nMagnateLevel = baseinfo.nMagnateLevel;
	respbody.nMoney = baseinfo.nMoney;
//	respbody.nGold = baseinfo.nGoldBean;

	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}

//客户端小游戏的玩家钱币更新
int32_t CFromClientEvent::OnMessage_update_asset(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: user exchange gold!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	CSvrGameUpdateAssetResp respbody;
	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead, MSGID_SVRGAME_UPDATE_ASSET_RESP);

	CGameSvrUpdateAssetReq* pUpdateAssetReq = dynamic_cast<CGameSvrUpdateAssetReq*>(pMsgBody);
	if(pUpdateAssetReq == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}

	if(enmAssetType_GoldCoin != pUpdateAssetReq->nAssetType)
	{
		WRITE_WARNING_LOG( "this assettype is not goldbean, just return!\n");
		respbody.nResult = S_OK;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return S_OK;
	}

	ret = UpdateGameAsset(*pUpdateAssetReq);
	respbody.nResult = ret;
	if(0 > ret)
	{
		WRITE_ERROR_LOG("update player asset failed! errorcode=0x%08X\n",
				ret);

		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
	//WRITE_DEBUG_LOG( "update player asset success!\n");
	return S_OK;
}

int32_t CFromClientEvent::UpdateGameAsset(const CGameSvrUpdateAssetReq& reqbody)
{
	int32_t ret = S_OK;

	if(reqbody.nAssetType != enmAssetType_GoldCoin)
	{
		WRITE_WARNING_LOG( "this function is used to update gold bean! not other type! assettype=%d\n",
				reqbody.nAssetType);
		return S_OK;
	}

	uint32_t nGoldBean = 0;
	ret = QueryPlayerGoldBean(reqbody.nRoleID, nGoldBean);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query player gold bean failed! errorcode=0x%08X\n", ret);
		return ret;
	}

	ret = UpdatePlayerGoldBean(reqbody, nGoldBean);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update player gold bean failed! errorcode=0x%08X\n", ret);
		return ret;
	}

	return S_OK;
}

int32_t CFromClientEvent::QueryPlayerGoldBean(const RoleID roleid, uint32_t& goldbean)
{
	char szSql[enmMaxSqlStringLength] = {0};
		uint32_t nFieldCount = 0;
		uint32_t nRowCount = 0;
		char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
		//memset(arrRecordSet, 0, sizeof(arrRecordSet));

		//首先查询余额
		sprintf(szSql, "select `GoldBean` from vdc_user.user_base_info where `RoleID`=%d", roleid);

		int32_t ret = S_OK;

		ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "db operation of query player money failed! roleid=%d, errorcode=0x%08X, sql=%s\n",
					roleid, ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "db operation of query player goldbean success! roleid=%d, sql=%s\n",
		//		roleid, szSql);

		if(nRowCount == 0)
		{
			goldbean = 0;
		}
		else
		{
			goldbean = (uint32_t)atoi(arrRecordSet[0]);
		}

		//WRITE_DEBUG_LOG( "user goldbean amount is %d, sql=%s\n", goldbean, szSql);

		return S_OK;
}

int32_t CFromClientEvent::UpdatePlayerGoldBean(const CGameSvrUpdateAssetReq& reqbody, const uint32_t left_goldbean)
{
	if(reqbody.nAssetType != enmAssetType_GoldCoin)
		{
			WRITE_WARNING_LOG( "asset type is not goldbean!\n");
			return S_OK;
		}

		char szSql[enmMaxSqlStringLength] = {0};
		char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
		//memset(arrRecordSet, 0, sizeof(arrRecordSet));

		//memset(szSql, 0, sizeof(szSql));
		switch(reqbody.nOpMode)
		{
			case enmUpdateAssetOperate_Increase:
			if((left_goldbean + reqbody.nAmount) > (uint32_t)enmMaxPlayerMoney)
			{
				WRITE_ERROR_LOG( "player left money plus nMoney is more than max player money limit! \
													roleid=%d, left goldbean=%u, nAmount=%u, plusresult=%u\n",
						reqbody.nRoleID, left_goldbean, reqbody.nAmount, left_goldbean + reqbody.nAmount);
				return E_OVERRUN_MAX_PLAYER_MONEY;
			}
			sprintf(szSql, "update vdc_user.user_base_info set GoldBean=GoldBean + %u where RoleID=%d and GoldBean + %u < %u",
					reqbody.nAmount, reqbody.nRoleID, reqbody.nAmount, enmMaxPlayerMoney);
			break;
			case enmUpdateAssetOperate_Reduce:
			if((uint32_t)left_goldbean < reqbody.nAmount)
			{
				WRITE_ERROR_LOG( "player left money is not enough!\
													roleid=%d, left goldbean=%u, nGoldBeanNeed=%u\n",
						reqbody.nRoleID, left_goldbean, reqbody.nAmount);
				return E_LEFTMONEY_NOT_ENOUGH;
			}
			sprintf(szSql, "update vdc_user.user_base_info set `GoldBean`=`GoldBean`-%u where `RoleID`=%d and `GoldBean`-%u <= %u",
					reqbody.nAmount, reqbody.nRoleID, reqbody.nAmount, (uint32_t)enmMaxPlayerMoney);
			break;
			default:
			WRITE_WARNING_LOG( "undefined asset update opmode! opmode=%d\n", reqbody.nOpMode);
			return S_OK;
		}

		uint64_t nAffectedRows = 0;
		int32_t ret = S_OK;
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0>ret)
		{
			WRITE_ERROR_LOG( "Error: execute update on role goldbean failed! roleid=%d, errorcode=0x%08X, sql:%s\n",
					reqbody.nRoleID, ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "execute update on role goldbean success! roleid=%d, sql=%s\n",
		//		reqbody.nRoleID, szSql);

		if((1 != nAffectedRows) && (2 != nAffectedRows))
		{
			WRITE_ERROR_LOG( "Error: execute update role goldbean sql success but 0 rows affected!roleid=%d, sql=%s\n",
					reqbody.nRoleID, szSql);
			return E_UNKNOWN;
		}
		//WRITE_DEBUG_LOG( "execute update role asset role goldbean sql success! roleid=%d, sql=%s\n",
		//		reqbody.nRoleID, szSql);

		return S_OK;
}


int32_t CFromClientEvent::OnMessage_GetRecharge(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: when  gameserver get user recharge gold!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	CGameSvrGetRechargeMsgResp respbody;
	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead, MESGIID_SVRGAME_CURRENCY_EXCHANGE_RESP);
	//进入房间获取角色信息消息体
	CGameSvrGetRechargeMsgReq* pRecharge = dynamic_cast<CGameSvrGetRechargeMsgReq *>(pMsgBody);
	if(pRecharge == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}

	ret = QueryUserRecharge(*pRecharge, respbody);
	respbody.nResult = ret;
	if(0 > ret)
	{
		WRITE_ERROR_LOG("get user recharge info failed! errorcode=0x%08X\n",
				ret);

		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "update player recharge info success!\n");
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
	return S_OK;
}

int32_t CFromClientEvent::QueryUserRecharge(const CGameSvrGetRechargeMsgReq& reqbody, CGameSvrGetRechargeMsgResp& respbody)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};
	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql,"select sum(RechargeAmount) from vdc_record.user_each_recharge where `RechargeTime` > %d and `RechargeTime` < %d and RoleID = %d",
			reqbody.nBeginTime, reqbody.nEndTime, reqbody.nRoleID);

	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query user recharge failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "query  NULL!, sql=%s\n",szSql);
		return S_OK;
	}
	respbody.nRechargeAmount = (int32_t)atoi(arrRecordSet[0]);
	return S_OK;
}


FRAME_DBPROXY_NAMESPACE_END
