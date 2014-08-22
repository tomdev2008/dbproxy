/*
 * bll_event_roomget_msg.cpp
 *
 *  Created on: 2013-3-4
 *      Author: liufl
 */

#include "bll_event_roomget_msg.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

int32_t CFromRoomGetEvent::OnMessageGetRoleInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver get RoleInfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CRoomGetRoleInfoReq* pTmpBody = dynamic_cast<CRoomGetRoleInfoReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CRoomGetRoleInfoResp get_roleinfo;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBRS_GETROLEINFO_RESP);

	ret = ProcessRoomGetRoleInfo(*pTmpBody, get_roleinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process roomserver login get roleinfo request failed! errorcode=0x%08X, roleid=%d\n",
				ret, pTmpBody->nRoleID);
		get_roleinfo.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_roleinfo, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_roleinfo, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process roomserver get roleinfo success! \n");

	get_roleinfo.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_roleinfo, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_roleinfo, nOptionLen, (const char*)pOptionData);

	return S_OK;

}
int32_t CFromRoomGetEvent::ProcessRoomGetRoleInfo(const CRoomGetRoleInfoReq& reqbody, CRoomGetRoleInfoResp& respbody)
{
	int32_t ret = S_OK;

	//写缓存必须的参数声明
	char szKey[enmMaxMemcacheKeyLen] = {0};
	size_t keylen = 0;
	char szVal[enmMaxMemcacheValueLen] = {0};
	size_t vallen = 0;
	char* pVal = NULL;
	uint32_t offset = 0;

	//玩家基本信息结构
	RoleBaseInfo rolebaseinfo;
	CVDCUserBaseInfo::VDCUserBaseInfoInit(rolebaseinfo);

	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();

	//构造key
	GenerateMemcacheKey(szKey,enmMaxMemcacheKeyLen, keylen, enmStoreType_RoleID, reqbody.nRoleID);

	//首先查找cache中是否存在
	ret = cacheobj.MemcacheIfKeyExist(cacheobj.m_memc, szKey, keylen );
	if(0 > ret)
	{
		//cache中不存在此人
		WRITE_NOTICE_LOG( "notice: this role is not exist in cache! we need to query its role info from db! roleid=%d \n",
				reqbody.nRoleID );

		//首先从数据库中查询
		ret = QueryRoleBaseInfo(reqbody.nRoleID, rolebaseinfo );
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "Error: role is not existed in cache, and query its info from db failed! roleid=%d, errorcode=0x%08X \n",
					reqbody.nRoleID, ret );
			return ret;
		}
		//WRITE_DEBUG_LOG( "role is not existed in cache, and query role base info from db success! rolied=%d \n",reqbody.nRoleID );

		ret = CVDCUserBaseInfo::VDCUserBaseInfoEncode((uint8_t *)szVal, enmMaxMemcacheValueLen, offset, rolebaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "Error: encode role base info failed! roleid=%d, errorcode=0x%08X \n",
					reqbody.nRoleID, ret );
			return ret;
		}
		//WRITE_DEBUG_LOG( "encode role base info success! roleid=%d \n",reqbody.nRoleID);

		ret = cacheobj.MemcacheSet(cacheobj.m_memc, szKey, keylen, szVal, offset, 0);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "set role base info struct to cache failed! key=%s, keylen=%d, errorcode=0x%08X \n",
					szKey, keylen, ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "set role base info struct to cache success! key=%s \n", szKey);
	}
	else
	{
		//缓存中存在此人
		ret = cacheobj.MemcacheGet(cacheobj.m_memc, szKey, keylen, pVal, vallen);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "this role is exists in cache, but fetch it from cache failed! roleid=%d, errorcode=0x%08X \n",
					reqbody.nRoleID, ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "this role is exist in cache, fetch it success! key=%s \n", szKey);

		offset = 0;
		ret = CVDCUserBaseInfo::VDCUserBaseInfoDecode((uint8_t *)pVal, enmMaxMemcacheValueLen, offset, rolebaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "decode role base info get from cache failed! roleid=%d, errorcode=0x%08X ",
					reqbody.nRoleID, ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "decode role base info get from cache success! roleid=%d \n",reqbody.nRoleID);
	}

	//填充响应结构
	respbody.nResult = S_OK;
	respbody.nRoleID = reqbody.nRoleID;
	respbody.nPlayerGender = rolebaseinfo.ucGender;
	respbody.nVipLevel = rolebaseinfo.ucVipLevel;
	respbody.nMoney = rolebaseinfo.nMoney;
	respbody.nIdentityType = rolebaseinfo.uIdentity;
	respbody.strRoleName = rolebaseinfo.szRoleName;
	respbody.nLoginTimes = rolebaseinfo.nLoginTimes;
	respbody.nAccountID = rolebaseinfo.nAccountID;

//	//查询用户账户表，得到其179id
//	ret = QueryRole179ID(reqbody.nRoleID, respbody.nAccountID);
//	if(0 > ret)
//	{
//		WRITE_ERROR_LOG( "query role 179id from user_account failed! roleid=%d, errorcode=0x%08X \n",
//				reqbody.nRoleID, ret);
//		return ret;
//	}
	//WRITE_DEBUG_LOG( "query role 179id  from user_account success! roleid=%d \n",reqbody.nRoleID);

	//查询在线时间总表，获得nOnlineTime字段
	ret = QueryUserTotalOnlineTime(reqbody.nRoleID, respbody.nOnlineTime);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query user total online time failed! errorcode=0x%08X, roleid=%d\n", ret, reqbody.nRoleID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query user total online time success! roleid=%d\n", reqbody.nRoleID);

	//查询该玩家总的在麦时长
	ret = QueryUserTotalOnMikeTime(reqbody.nRoleID, respbody.nOnMicTime);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query user total onmike time failure! errorcode=0x%08X, roleid=%d\n", ret, reqbody.nRoleID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query user total onmike time success! roleid=%d\n", reqbody.nRoleID);

	//查询玩家的自身设置
	ret = QueryRoleSelfSetInfo(reqbody.nRoleID, respbody.nSelfSetting);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query user selfsetting failed! roleid=%d, errorcode=0x%08X\n",
				reqbody.nRoleID, ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query user selfsetting success! roleid=%d\n", reqbody.nRoleID);

	//查询玩家的成就
	int32_t achieve_count = 0;
	int32_t achieve[MaxAchieveCount];
	memset(achieve, 0, sizeof(achieve));
	ret = QueryAchievement(reqbody.nRoleID, achieve_count, achieve, MaxAchieveCount);
	if(achieve_count > 0)
	{
		for(int32_t i=0; i<achieve_count; ++i)
		{
			respbody.nIdentityType |= achieve[i];
		}
	}
	//查询玩家管理身份信息
	int32_t admcount = 0;
	ret = QueryRoleRoomMember(reqbody.nRoleID, admcount, respbody.arrRoomID, respbody.arrRoleRank, MaxBeAdminPerPlayer);
	if(0 > ret)
	{
		return ret;
	}
	respbody.nRoomCount = (uint16_t)admcount;
	//查询玩家收藏房间信息
	int32_t cllcount = 0;
	ret = QueryRolecollection(reqbody.nRoleID, cllcount, respbody.arrCollectRoomList, MaxCollectCount);
	if(0 > ret)
	{
		return ret;
	}
	respbody.nCollectCount = cllcount;
	return S_OK;
}
int32_t CFromRoomGetEvent::QueryUserTotalOnlineTime(const RoleID roleid, uint32_t& total_online_time)
{
	if(0 > roleid)
	{
		WRITE_WARNING_LOG( "invalide roleid in query user total online time! roleid=%d\n", roleid);
		total_online_time = 0;
		return S_OK;
	}

	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select  `TotalOnlineTime` from vdc_time.`user_totaltime` where `RoleID`=%d", roleid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query role total online time failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query role total online time success! rowcount=%d, sql=%s\n", nRowCount, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in db of total online time table! roleid=%d, sql=%s\n", roleid, szSql);
		total_online_time = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist of total online time table! roleid=%d\n", roleid);

	total_online_time = (uint32_t)atoi(arrRecordSet[0]);

	return S_OK;
}
int32_t CFromRoomGetEvent::QueryUserTotalOnMikeTime(const RoleID roleid, uint32_t& total_onmike_time)
{
	if(0 > roleid)
	{
		WRITE_WARNING_LOG( "invalide roleid in query user total onmike time! roleid=%d\n", roleid);
		total_onmike_time = 0;
		return S_OK;
	}

	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select  sum(`OnMikeTime`) from vdc_time.`user_room_time_total` where `RoleID`=%d", roleid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query role total onmike time failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query role total onmike time success! rowcount=%d, sql=%s\n", nRowCount, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in db of total onmike time table! roleid=%d, sql=%s\n", roleid, szSql);
		total_onmike_time = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist of total onmike time table! roleid=%d\n", roleid);

	total_onmike_time = (uint32_t)atoi(arrRecordSet[0]);

	return S_OK;
}
int32_t CFromRoomGetEvent::QueryRoleSelfSetInfo(const RoleID roleid, uint32_t& nUserInfoSelfSet)
{
	if(enmInvalidRoleID == roleid)
	{
		WRITE_ERROR_LOG( "roleid(179uin) on query user self set info is invalid! roleid=%d\n", roleid);
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "roleid(179uin) on query user self set info is right! roleid=%d\n", roleid);

	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `UserInfoSelfSet` from vdc_user.`user_config_info` where `RoleID`=%d", roleid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query role self set info failed! errorcode=0x%08X, roleid(179uin)=%d, sql=%s\n",
				ret, roleid, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query role self set info success! roleid=%d, sql=%s\n",roleid, szSql);

	if(nRowCount == 0)
	{
		WRITE_WARNING_LOG( "unknown error happened while query user self set info! no such record in table user_config_info! roleid=%d\n", roleid);
		nUserInfoSelfSet = 0;
		return S_OK;
	}
	nUserInfoSelfSet = (uint32_t)atoi(arrRecordSet[0]);
	//WRITE_DEBUG_LOG( "db operation on query user self set info success! roleid=%d, nUserInfoSelfSet=%d\n", roleid, nUserInfoSelfSet);

	return S_OK;
}
int32_t CFromRoomGetEvent::QueryAchievement(const RoleID roleid, int32_t& achieve_count, int32_t achieve[], const int32_t max_achieve_count)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `achievement` from vdc_user.user_achievement where `RoleID`=%d", roleid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: db operation on query role achievement failed! roleid=%d, errorcode=0x%08X, sql=%s\n",
				roleid, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query role achievement success! roleid=%d, sql=%s\n",roleid, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG("this player has no achievement yet! roleid=%d\n", roleid);
		achieve_count = 0;
		return S_OK;
	}

	//查询到的记录的条数
	if(nRowCount >= (uint32_t)max_achieve_count)
	{
		achieve_count = max_achieve_count;
	}
	else
	{
		achieve_count = nRowCount;
	}

	//取出所有查询到的记录（多条记录）
	for(int32_t i=0; i<achieve_count; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "error: db operation of fetch row on query role achievement failed! roleid=%d\n", roleid);
			return ret;
		}
		//WRITE_DEBUG_LOG( "db operation of fetch row on query role achievement success! roleid=%d\n", roleid);

		achieve[i] = atoi(arrRecordSet[0]);
	}

	return S_OK;
}
int32_t CFromRoomGetEvent::QueryRoleRoomMember(const RoleID roleid, int32_t& admincount, RoomID arrRoomID[], RoleRank arrRoleRank[], const int32_t max_admin_per_user)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `RoomID`, `RoleRank` from vdc_room.`room_member`  where `RoleID`=%d and `RoleRank`>%d", roleid, enmRoleRank_None);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;

	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: db operation on query role admin info failed! roleid=%d, errorcode=0x%08X, sql=%s\n",
			roleid, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query role admin info success! roleid=%d, sql=%s\n", roleid, szSql);

	//查询到的记录的条数
	if(0 == nRowCount)
	{
		//WRITE_DEBUG_LOG("this user is not admin in any room! roleid=%d, sql=%s\n", roleid, szSql);
		admincount = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG("this user is admin in some room! we will fetch it below! roleid=%d, sql=%s\n",roleid, szSql);

	admincount = (int32_t)nRowCount;

	//取出所有查询到的记录（多条记录）
	for(uint32_t i=0; i<nRowCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("error: db operation of fetch row on query role roomadmin info failed! roleid=%d\n", roleid);
			return ret;
		}
		//WRITE_DEBUG_LOG("db operation of fetch row on query role roomadmin success! roleid=%d\n", roleid);

		arrRoomID[i] = (RoomID)atoi(arrRecordSet[0]);
		arrRoleRank[i] = (RoleRank)atoi(arrRecordSet[1]);
	}

	return S_OK;
}
int32_t CFromRoomGetEvent::QueryRolecollection(const RoleID roleid, int32_t& cllcount, RoomID arrRoomID[], const int32_t max_collection_count)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `RoomID` from vdc_web.`room_collection`  where `RoleID`=%d", roleid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;

	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: db operation on query role collection room info failed! roleid=%d, errorcode=0x%08X, sql=%s\n",
			roleid, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query role admin info success! roleid=%d, sql=%s\n", roleid, szSql);

	//查询到的记录的条数
	if(0 == nRowCount)
	{
		//WRITE_DEBUG_LOG("this user is not admin in any room! roleid=%d, sql=%s\n", roleid, szSql);
		cllcount = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG("this user is admin in some room! we will fetch it below! roleid=%d, sql=%s\n",roleid, szSql);



	cllcount = (int32_t)nRowCount < max_collection_count ? (int32_t)nRowCount : max_collection_count;

	//取出所有查询到的记录（多条记录）
	for(int32_t i=0; i< cllcount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("error: db operation of fetch row on query role room collection info failed! roleid=%d\n", roleid);
			return ret;
		}
		//WRITE_DEBUG_LOG("db operation of fetch row on query role roomadmin success! roleid=%d\n", roleid);

		arrRoomID[i] = (RoomID)atoi(arrRecordSet[0]);
	}

	return S_OK;
}

int32_t CFromRoomGetEvent::OnMessageGetRoomInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver get roominfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CRoomGetRoomInfoReq* pTmpBody = dynamic_cast<CRoomGetRoomInfoReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CRoomGetRoomInfoResp get_roominfo;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBRS_GETROOMINFO_RESP);

	ret = ProcessRoomGetRoomInfoReq(*pTmpBody, get_roominfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process from getroominfo request failed! errorcode=0x%08X, roomid=%d\n",
				ret, pTmpBody->nRoomID);
		get_roominfo.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_roominfo, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_roominfo, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process roomserver get room info success! \n");

	get_roominfo.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_roominfo, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_roominfo, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromRoomGetEvent::ProcessRoomGetRoomInfoReq(const CRoomGetRoomInfoReq& reqbody, CRoomGetRoomInfoResp& respbody)
{
	//WRITE_DEBUG_LOG( "begin to process roomserver get roominfo request! threadindex=%d\n");

	int32_t ret = S_OK;

	//缓存操作必须的参数声明
	char szKey[enmMaxMemcacheKeyLen] = {0};
	size_t keylen = 0;
	char szVal[enmMaxMemcacheValueLen] = {0};
	size_t vallen = 0;
	char* pVal = NULL;
	uint32_t offset = 0;

	RoomBaseInfo roombaseinfo;

	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();

	//构造key
	GenerateMemcacheKey(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoomID, reqbody.nRoomID);

	//首先查找cache中是否存在
	ret = cacheobj.MemcacheIfKeyExist(cacheobj.m_memc, szKey, keylen);
	if(0>ret)
	{
		//该房间在缓存中 不存在
		WRITE_NOTICE_LOG("notice: this room is not existed in cache! roomid=%d\n",
				reqbody.nRoomID);

		//从数据库中获取房间基本信息
		ret = QueryRoomBaseInfo(reqbody.nRoomID, roombaseinfo);
		if(0>ret)
		{
			if((int32_t)E_NOT_EXIST_IN_DB == ret)
			{
				WRITE_WARNING_LOG("this room is not exists in db! roomid=%d\n", reqbody.nRoomID);
			}
			else
			{
				WRITE_ERROR_LOG("error: query room base info from db failed! roomid=%d, errorcode=0x%08X\n",
						reqbody.nRoomID, ret);
			}

			return ret;
		}
		//WRITE_DEBUG_LOG("query room base info from db success! roomid=%d\n",				reqbody.nRoomID);

		//将该房间信息写入缓存
		vallen = 0;
		offset = 0;
		ret = CVDCRoomBaseInfo::VDCRoomBaseInfoEncode((uint8_t *)szVal, enmMaxMemcacheValueLen, offset, roombaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("error: encode room base info into tmp buf failed! roomid=%d, errorcode=0x%08X \n",
					reqbody.nRoomID, ret);
			return ret;
		}
		//WRITE_DEBUG_LOG("encode room base info into tmp buffer success! roomid=%d\n",reqbody.nRoomID);
		vallen = (size_t)offset;

		ret = cacheobj.MemcacheSet(cacheobj.m_memc, szKey, keylen, szVal, vallen, 0);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("error: write room base info into cache failed! roomid=%d, key=%s, ret=0x%08X\n",
					reqbody.nRoomID, szKey, ret);
			return ret;
		}
		//WRITE_DEBUG_LOG("write room base info into cache success! roomid=%d, key=%s\n",reqbody.nRoomID, szKey);
	}
	else
	{
		//该房间在缓存中存在
		WRITE_NOTICE_LOG("notice: this room is existed in cache! roomid=%d\n",
				reqbody.nRoomID);

		pVal = NULL;
		vallen = 0;
		ret = cacheobj.MemcacheGet(cacheobj.m_memc, szKey, keylen, pVal, vallen);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("error: room is existed in cache but fetch from cache failed! roomid=%d, errorcode=0x%08X\n",
					reqbody.nRoomID, ret);
			return ret;
		}
		//WRITE_DEBUG_LOG("room exists in cache and get room base info from cache success! roomid=%d\n",reqbody.nRoomID);

		vallen = 0;
		offset = 0;
		ret = CVDCRoomBaseInfo::VDCRoomBaseInfoDecode((uint8_t *)pVal, enmMaxMemcacheValueLen, offset, roombaseinfo);
		if(0>ret)
		{
			WRITE_ERROR_LOG("error: decode room base info buffer get from memcache failed! roomid=%d, errorcode=0x%08X\n",
					reqbody.nRoomID, ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "decode room base info buffer get from memcache success! roomid=%d\n",reqbody.nRoomID);
	}

	respbody.nResult = S_OK;
	respbody.nRegionType = roombaseinfo.ucRegionType;
	respbody.nRegionID = roombaseinfo.usRegionID;
	respbody.strRegionName = roombaseinfo.szRegionName;
	respbody.nChannelType = roombaseinfo.ucChannelType;
	respbody.nChannelID = roombaseinfo.usChannelID;
	respbody.strChannelName = roombaseinfo.szChannelName;
	respbody.nRoomType = roombaseinfo.ucRoomType;
	respbody.nRoomID = roombaseinfo.uRoomID;
	respbody.strRoomName = roombaseinfo.szRoomName;
	respbody.nRoomFund = roombaseinfo.nRoomFund;
	respbody.nRoomOption = roombaseinfo.nOption;
	respbody.nMicroTime = roombaseinfo.nMicroTime;
	respbody.nMaxShowUserLimit = roombaseinfo.nRoomShowCapacity;
	respbody.nMaxRealUserLimit = roombaseinfo.nRoomRealCapacity;
	respbody.strPassword = roombaseinfo.szRoomPassword;
	respbody.strRoomNotice = roombaseinfo.szRoomNotice;
	respbody.strRoomSignature = roombaseinfo.szRoomTheme;
	respbody.strRoomWelcome = roombaseinfo.szWelcomewords;
	respbody.nRobotPercent = roombaseinfo.nRobotPercent;
	respbody.nSendPercent = roombaseinfo.nSendPercent;

	//查询房间成员表找到室主的roleid
	ret = QueryRoomHostRoleID(reqbody.nRoomID, respbody.nHostID);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query room host roleid from db failed! roomid=%d, errorcode=0x%08X",
				reqbody.nRoomID, ret);
		respbody.nResult = ret;
		return ret;
	}
	//WRITE_DEBUG_LOG( "query room host roleid from db success! roomid=%d, room host roleid=%d \n",reqbody.nRoomID, respbody.nHostID);

	//玩家基本信息结构
	RoleBaseInfo rolebaseinfo;

	//首先从数据库中查询
	ret = QueryRoleBaseInfo(respbody.nHostID, rolebaseinfo );
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: role is not existed in cache, and query its info from db failed! roleid=%d, errorcode=0x%08X \n",
				respbody.nHostID, ret );
		return ret;
	}
	respbody.strHostName = rolebaseinfo.szRoleName;
	respbody.nHostGender = rolebaseinfo.ucGender;

	//		//从用户基本信息表中查询室主的昵称
	//		char szHostNicName[enmMaxRoleNameLength] = {0};
	//		//memset(szHostNicName, 0,sizeof(szHostNicName));
	//		ret = QueryRoleNickName(respbody.nHostID, szHostNicName);
	//		if(0 > ret)
	//		{
	//			WRITE_ERROR_LOG( "error: query room host nick name from db failed! roleid=%d, errorcode=0x%08X \n",
	//				respbody.nHostID, ret);
	//			respbody.nResult = ret;
	//			return ret;
	//		}
	//		respbody.strHostName = szHostNicName;
	//		WRITE_DEBUG_LOG( "query room host nick name from db success! host roleid=%d, hostname=%s \n",
	//			respbody.nHostID, szHostNicName);
	//
	//		//查询室主性别：从用户基本信息表中查询
	//		ret = QueryRoleGender(respbody.nHostID, respbody.nHostGender);
	//		if(0 > ret)
	//		{
	//			WRITE_ERROR_LOG( "error: query role gender from user_base_info table failed! roleid=%d, errorcode=0x%08X \n",
	//				respbody.nHostID, ret);
	//			respbody.nResult = ret;
	//			return ret;
	//		}
	//		WRITE_DEBUG_LOG( "query role gender from user_base_info success! roleid=%d, rolegender=%d \n",
	//			respbody.nHostID, respbody.nHostGender);

	//查询室主的179id， 从用户账户表中查询
	ret = QueryRole179ID(respbody.nHostID, respbody.nHost179ID);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "error: query role 179id from db failed! roleid=%d, errorcode=0x%08X \n",
				respbody.nHostID, ret);
		respbody.nResult = ret;
		return ret;
	}
	//WRITE_DEBUG_LOG( "query role 179id from user_account success! roleid=%d, 179id=%d \n",respbody.nHostID, respbody.nHost179ID);

	//从房间成员表中查询管理员的个数，管理级别，在房间时长
	ret = QueryRoomMember(reqbody.nRoomID, respbody.nCurMemberCount,MaxAdminCountPerRoom,
			respbody.arrRoleID, respbody.arrRoleRank, respbody.arrTotalTime);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "error: query rolerank, onlinetime, onmictime on room_member failed! roomid=%d, errorcode=0x%08X \n",
				reqbody.nRoomID, ret);
		respbody.nResult = ret;
		return ret;
	}
	//WRITE_DEBUG_LOG( "query rolerank, in room time, on mic time from room_member success! roomid=%d \n",reqbody.nRoomID);

	//从在麦时间总表中查出在麦时长(某个玩家在某个房间的总共在麦时长)
	for(uint16_t i=0; i<respbody.nCurMemberCount; ++i)
	{
		ret = QueryUserTotalOnMikeTimeInOneRoom(reqbody.nRoomID, respbody.arrRoleID[i], respbody.arrMicTime[i]);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "query user total onmike time in one room failed! errorcode=0x%08X, roleid=%d, roomid=%d\n",
					ret, respbody.arrRoleID[i], reqbody.nRoomID);
			respbody.nResult = ret;
			return ret;
		}
		//WRITE_DEBUG_LOG( "query user total onmike time in one room success! roomid=%d, roleid=%d\n",reqbody.nRoomID, respbody.arrRoleID[i]);
	}

	//查询在该房间的黑名单中的玩家
	ret = QueryRoomBlackList(reqbody.nRoomID, respbody.nInBlackPlayerCount, respbody.arrBlackRoleID);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query room black from db failed! roomid=%d, errorcode=0x%08X",
				reqbody.nRoomID, ret);
		respbody.nResult = ret;
		return ret;
	}
	//WRITE_DEBUG_LOG( "query room black list from db success! roomid=%d, blacklist player count=%d\n",reqbody.nRoomID, respbody.nInBlackPlayerCount);

	//查询房间封停信息
	char szLockReason[MaxLockRoomReasonLength] = {0};
	ret = QueryRoomLockInfo(reqbody.nRoomID, respbody.nTimeLock, respbody.nTimeUnLock, szLockReason);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query room lock info from db failed! errorcode=0x%08X, roomid=%d\n",
				ret, reqbody.nRoomID);
		respbody.nResult = ret;
		return ret;
	}
	respbody.strLockReason = szLockReason;
	//WRITE_DEBUG_LOG( "query room lock info from db success! roomid=%d\n", reqbody.nRoomID);

	respbody.nResult = S_OK;
	return S_OK;
}


int32_t CFromRoomGetEvent::QueryRoomMember(const RoomID roomid, uint16_t& room_admin_count, const int32_t max_admin_per_room, RoleID arrAdminRoleID[],
		RoleRank arrRoleRank[], uint32_t arrTotalInRoomTime[])
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `RoleID`, `RoleRank`  from vdc_room.room_member where `RoomID`=%d and `RoleRank` > %d", roomid, enmRoleRank_None);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;

	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: db operation on query room member info failed! roomid=%d, errorcode=0x%08X, sql=%s\n",
				roomid, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query room member info success! roomid=%d, sql=%s\n", roomid, szSql);

	if(nRowCount == 0)
	{
		WRITE_WARNING_LOG( "warning: this room has no admin yet! roomid=%d\n", roomid);
		room_admin_count = 0;
		memset(arrAdminRoleID, 0, sizeof(RoleID) * max_admin_per_room);
		memset(arrRoleRank, 0, sizeof(RoleRank) * max_admin_per_room);
		memset(arrTotalInRoomTime, 0, sizeof(int32_t) * max_admin_per_room);
		return S_OK;
	}
	room_admin_count = nRowCount;
	//WRITE_DEBUG_LOG( "this room has some admins, admin count is %d\n", nRowCount);

	//取出所有查询到的记录（多条记录）
	for(uint16_t i=0; i<room_admin_count; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "error: fetch row of query room member failed! roomid=%d, errorcode=0x%08X\n", roomid, ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "fetch row of query room member success! roomid=%d, membercount=%d\n", roomid, room_admin_count);

		arrAdminRoleID[i] = atoi(arrRecordSet[0]);
		arrRoleRank[i] = atoi(arrRecordSet[1]);
	}

	//查询用户在某个房间的总时长[循环查询]
	for(uint16_t i=0; i<room_admin_count; ++i)
	{
		//memset(szSql, 0, sizeof(szSql));
		sprintf(szSql, "select `InRoomTime`  from vdc_time.`user_room_time_total` where `RoomID`=%d and `RoleID`=%d", roomid, arrAdminRoleID[i]);
		nFieldCount = 0;
		nRowCount = 0;
		//memset(arrRecordSet, 0, sizeof(arrRecordSet));
		ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "execute query player in room time failed!errorcode=0x%08X, roleid=%d, roomid=%d, sql=%s\n",
					ret, arrAdminRoleID[i], roomid, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "execute query player in room time success! roleid=%d, roomid=%d, sql=%s\n",arrAdminRoleID[i], roomid, szSql);

		if(0 == nRowCount)
		{
			arrTotalInRoomTime[i] = 0;
			continue;
		}
		arrTotalInRoomTime[i] = (uint32_t)atoi(arrRecordSet[0]);
	}
	//WRITE_DEBUG_LOG( "query all player in room time success! roomid=%d, sql=%s\n", roomid, szSql);

	return S_OK;
}
int32_t CFromRoomGetEvent::QueryUserTotalOnMikeTimeInOneRoom(const RoomID roomid, const RoleID roleid, uint32_t& total_onmike_time)
{
	if(0 > roleid)
	{
		WRITE_WARNING_LOG( "invalide roleid in query user total onmike time! roleid=%d\n", roleid);
		total_onmike_time = 0;
		return S_OK;
	}

	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select  `OnMikeTime` from vdc_time.`user_room_time_total` where `RoleID`=%d and `RoomID`=%d",
			roleid, roomid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query role total time onmike in one room failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query role total onmike time in one room success! rowcount=%d, sql=%s\n", nRowCount, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in db of total onmike time table!sql=%s\n",szSql);
		total_onmike_time = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist of total onmike time table! roleid=%d\n", roleid);

	total_onmike_time = (uint32_t)atoi(arrRecordSet[0]);

	return S_OK;
}
int32_t CFromRoomGetEvent::QueryRoomBlackList(const RoomID roomid, uint32_t& room_black_list_player_count, RoleID arrBlackPlayerRoleID[])
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `RoleID` from vdc_room.room_black_list where `RoomID`=%d", roomid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: db operation on query room black list failed! roomid=%d, errorcode=0x%08X , sql=%s\n",
				roomid, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query room black list success! roomid=%d , sql=%s\n",roomid, szSql);

	if(nRowCount == 0)
	{
		WRITE_WARNING_LOG( "db operation on query room black list success! but there is no black list player in this room! roomid=%d , sql=%s\n",
				roomid, szSql);
		room_black_list_player_count = 0;
		return S_OK;
	}
	room_black_list_player_count = nRowCount;
	//WRITE_DEBUG_LOG( "this room has some black players, black player count is %d, sql=%s\n", nRowCount, szSql);

	//取出所有查询到的记录（多条记录）
	for(uint32_t i=0; i<nRowCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "error: fetch row of query room black list player failed! roomid=%d, errorcode=0x%08X \n", roomid, ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "fetch row of query room black list player success! roomid=%d, black list player count=%d\n", roomid, room_black_list_player_count);

		arrBlackPlayerRoleID[i] = atoi(arrRecordSet[0]);
	}

	return S_OK;
}
int32_t CFromRoomGetEvent::QueryRoomLockInfo(const RoomID roomid, uint32_t& startLockTime, uint32_t& endLockTime, char szLockReason[])
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select  `StartLockTime`, `EndLockTime`, `LockReason`  from vdc_room.`room_lock`  where `RoomID`=%d",
			roomid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query room account lock info failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query role room account lock info success! rowcount=%d, sql=%s\n",nRowCount, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in db of room lock info table!sql=%s\n",szSql);
		startLockTime = 0;
		endLockTime = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist of room lock info table! roomid=%d\n", roomid);

	startLockTime = (uint32_t)atoi(arrRecordSet[0]);
	endLockTime = (uint32_t)atoi(arrRecordSet[1]);
	strcpy_safe(szLockReason, MaxLockRoomReasonLength, arrRecordSet[2], enmMaxFieldValueLength);

	return S_OK;
}

int32_t CFromRoomGetEvent::OnMessageGetSongList(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver get RoleInfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetSongListFromDBReq* pTmpBody = dynamic_cast<CGetSongListFromDBReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetSongListFromDBResp get_roleinfo;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead, MSGID_DBRS_GETSONGLISTFROMDB_RESP);

	ret = QuerySongList(*pTmpBody, get_roleinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process roomserver login get roleinfo request failed! errorcode=0x%08X\n",
				ret);
		get_roleinfo.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_roleinfo, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_roleinfo, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process roomserver get roleinfo success! \n");

	get_roleinfo.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_roleinfo, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_roleinfo, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromRoomGetEvent::QuerySongList(const CGetSongListFromDBReq& reqbody, CGetSongListFromDBResp& respbody)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	sprintf(szSql, "select `SongIndex`, `SongName`, `SongAttr` from vdc_requestsong.song_list "
			"where `SongerRoleID` = %d",
			reqbody.nSongerRoleID
			);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: db operation failed! roleid=%d, errorcode=0x%08X, sql=%s\n",
			reqbody.nSongerRoleID, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG("db operation success! roleid=%d, sql=%s\n", reqbody.nSongerRoleID, szSql);

	//查询到的记录的条数
	if(0 == nRowCount)
	{
		//WRITE_DEBUG_LOG("this user is not admin in any room! roleid=%d, sql=%s\n", reqbody.nSongerRoleID, szSql);
		respbody.nSongCount = 0;
		return S_OK;
	}

	respbody.nSongCount = 0;
	//取出所有查询到的记录（多条记录）
	char szName[MaxSongNameLength];
	for(uint32_t i=0; i<nRowCount; ++i)
	{
		//ret = mysqlguard.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("error: db operation of fetch row on query role roomadmin info failed! roleid=%d\n",
					reqbody.nSongerRoleID);
			continue;
		}
		//WRITE_DEBUG_LOG("db operation of fetch row on query role roomadmin success! roleid=%d\n",reqbody.nSongerRoleID);

		respbody.arrSongIndex[ respbody.nSongCount ] = (uint16_t)atoi(arrRecordSet[0]);
		memset(szName, 0, sizeof(szName));
		strcpy_safe(szName, MaxSongNameLength, arrRecordSet[1], strlen(arrRecordSet[1]));
		respbody.arrSongName[ respbody.nSongCount ]  = szName;
		respbody.arrSongAttr[ respbody.nSongCount ] = (uint32_t)atoi(arrRecordSet[2]);

		respbody.nSongCount++;
		if(respbody.nSongCount >= 50) //超过50首直接返回
		{
			return S_OK;
		}
	}

	return S_OK;
}


int32_t CFromRoomGetEvent::OnMessageGetRoomArtistList(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver get RoleInfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetRoomArtistReq* pTmpBody = dynamic_cast<CGetRoomArtistReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetRoomArtistResp get_artistlist;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead, MSGID_RSDB_GETROOMARTIST_RESP);

	ret = QueryArtistList(*pTmpBody, get_artistlist);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process roomserver login get roleinfo request failed! errorcode=0x%08X\n",
				ret);
		get_artistlist.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_artistlist, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_artistlist, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process roomserver get artist list success! \n");

	get_artistlist.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_artistlist, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_artistlist, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromRoomGetEvent::QueryArtistList(const CGetRoomArtistReq& reqbody, CGetRoomArtistResp& respbody)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	sprintf(szSql, "select a.`RoleID`,b.`Gender`,c.`179ID`,b.`RoleName`,b.`VipLevel`,"
			"a.RoleRank,b.`RoleIdentity`,b.`OSVersion`,b.`Magnatelevel` from vdc_room.room_member a left join "
			"vdc_user.user_base_info b on a.RoleID=b.RoleID left join vdc_user.user_account c on b.RoleID=c.179Uin "
			"where a.`RoomID` = %d and a.RoleRank > 1 and b.RoleIdentity in (1,2,4)",
			reqbody.nRoomID
			);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: db operation failed! roomid=%d, errorcode=0x%08X, sql=%s\n",
			reqbody.nRoomID, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG("db operation success! roleid=%d, sql=%s\n", reqbody.nSongerRoleID, szSql);

	//查询到的记录的条数
	if(0 == nRowCount)
	{
		//WRITE_DEBUG_LOG("this user is not admin in any room! roleid=%d, sql=%s\n", reqbody.nSongerRoleID, szSql);
		respbody.nCount = 0;
		return S_OK;
	}

	respbody.nCount = 0;

	char szName[MaxRoleNameLength];
	for(uint32_t i=0; i<nRowCount; ++i)
	{
		//ret = mysqlguard.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("error: db operation of fetch row on query role artist list failed! RoomID=%d\n",
					reqbody.nRoomID);
			continue;
		}
		//WRITE_DEBUG_LOG("db operation of fetch row on query role roomadmin success! roleid=%d\n",reqbody.nSongerRoleID);

		respbody.arrRoleID[ respbody.nCount ] = (uint32_t)atoi(arrRecordSet[0]);
		respbody.arrGender[ respbody.nCount ] = (uint16_t)atoi(arrRecordSet[1]);
		respbody.arr179ID[ respbody.nCount ] = (uint32_t)atoi(arrRecordSet[2]);
		memset(szName, 0, sizeof(szName));
		strcpy_safe(szName, MaxSongNameLength, arrRecordSet[3], strlen(arrRecordSet[3]));
		respbody.strRoleName[ respbody.nCount ]  = szName;
		respbody.arrVipLevel[ respbody.nCount ] = (uint16_t)atoi(arrRecordSet[4]);
		respbody.arrRoleRank[ respbody.nCount ] = (uint16_t)atoi(arrRecordSet[5]);
		respbody.arrIdentityType[ respbody.nCount ] = (uint32_t)atoi(arrRecordSet[6]);
		respbody.arrClientInfo[ respbody.nCount ] = (uint32_t)atoi(arrRecordSet[7]);
		respbody.arrMagnateLevel[ respbody.nCount ] = (uint32_t)atoi(arrRecordSet[8]);

		respbody.nCount++;
	}
	return S_OK;
}


int32_t CFromRoomGetEvent::OnMessageGetArtistInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver get RoleInfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetOfflineArtistInfoReq* pTmpBody = dynamic_cast<CGetOfflineArtistInfoReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetOfflineArtistInfoResp get_artistlist;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead, MSGID_DBRS_GETROLEIDENTITY_RESP);

	ret = QueryArtistInfo(*pTmpBody, get_artistlist);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process roomserver login get roleinfo request failed! errorcode=0x%08X\n",
				ret);
		get_artistlist.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_artistlist, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_artistlist, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process roomserver get artist info success! \n");

	get_artistlist.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_artistlist, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_artistlist, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromRoomGetEvent::QueryArtistInfo(const CGetOfflineArtistInfoReq& reqbody, CGetOfflineArtistInfoResp& respbody)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select a.`RoleID`,b.`Gender`,c.`179ID`,b.`RoleName`,b.`VipLevel`,"
			"a.RoleRank,b.`RoleIdentity`,b.`OSVersion`,b.`Magnatelevel` from vdc_room.room_member a left join "
			"vdc_user.user_base_info b on a.RoleID=b.RoleID left join vdc_user.user_account c on b.RoleID=c.179Uin "
			"where b.`RoleID`=%d",
			reqbody.nRoleID
			);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query artist info failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query role room account lock info success! rowcount=%d, sql=%s\n",nRowCount, szSql);

	if(0 == nRowCount)
	{
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist of room lock info table! roomid=%d\n", roomid);
	char szName[MaxRoleNameLength];

	respbody.nRoleID = (uint32_t)atoi(arrRecordSet[0]);
	respbody.nGender  = (uint16_t)atoi(arrRecordSet[1]);
	respbody.n179ID  = (uint32_t)atoi(arrRecordSet[2]);
	memset(szName, 0, sizeof(szName));
	strcpy_safe(szName, MaxSongNameLength, arrRecordSet[3], strlen(arrRecordSet[3]));
	respbody.strRoleName   = szName;
	respbody.nVipLevel  = (uint16_t)atoi(arrRecordSet[4]);
	respbody.nRoleRank  = (uint16_t)atoi(arrRecordSet[5]);
	respbody.nIdentityType  = (uint32_t)atoi(arrRecordSet[6]);
	respbody.nClientInfo  = (uint32_t)atoi(arrRecordSet[7]);
	respbody.nMagnateLevel  = (uint32_t)atoi(arrRecordSet[8]);

	return S_OK;
}

FRAME_DBPROXY_NAMESPACE_END
