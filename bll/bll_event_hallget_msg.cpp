/*
 * bll_event_hallget_msg.cpp
 *
 *  Created on: 2013-2-28
 *      Author: liufl
 */

#include "bll_event_hallget_msg.h"
#include "../memcache_dbproxy/vdc_memcache.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

#define GET_ACCOUNTID_COUNT		100

int32_t CFromHallGetEvent::OnMessage_HallGetRoleInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: hallserver get RoleInfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CHallGetRoleLoginInfoReq_Public* pTmpBody = dynamic_cast<CHallGetRoleLoginInfoReq_Public*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CHallGetRoleInfoResp_Public hall_get_rolebaseinfo_resp;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBHS_GETROLELOGININFO_RESP);

	ret = ProcessHallLoginGetRolebaseinfoReq(pTmpBody, hall_get_rolebaseinfo_resp);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process hallserver login get roleinfo request failed! errorcode=0x%08X, roleid=%d\n",
				ret, pTmpBody->nRoleID);
		hall_get_rolebaseinfo_resp.nResult = ret;
		g_Frame.PostMessage(&resphead, &hall_get_rolebaseinfo_resp, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &hall_get_rolebaseinfo_resp, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process hallserver login get rolebaseinfo success! \n");

	hall_get_rolebaseinfo_resp.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &hall_get_rolebaseinfo_resp, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &hall_get_rolebaseinfo_resp, nOptionLen, (const char*)pOptionData);

	return S_OK;
}

AccountID CFromHallGetEvent::GetNormalAccount()
{
	AccountID nAccountID = enmInvalidAccountID;
	char szSql[enmMaxSqlStringLength] = {0};

	int32_t nAccountIDCount = 0;
	AccountID arrAccountID[GET_ACCOUNTID_COUNT];
	sprintf(szSql, "select `accountid` from vdc_account.normal_account where appointtime = 0 limit %d", GET_ACCOUNTID_COUNT);
	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "Error: DB operation failed on query role baseinfo for sql=%s\n", szSql);
		return nAccountID;
	}
	WRITE_DEBUG_LOG( "db operation success on query role baseinfo for sql=%s\n", szSql);

	if(0 == nRowCount)
	{
		WRITE_ERROR_LOG("no more accountid in normal_account!\n");

		return nAccountID;
	}

	for(int32_t i = 0; i < nRowCount; ++i)
	{
		arrAccountID[nAccountIDCount++] = (AccountID)atoi(arrRecordSet[i]);
	}

	while(true)
	{
		if(nAccountIDCount <= 0)
		{
			break;
		}

		int32_t nIndex = Random(nAccountIDCount);
		nAccountID = arrAccountID[nIndex];
		arrAccountID[nIndex] = arrAccountID[nAccountIDCount - 1];
		arrAccountID[nAccountIDCount - 1] = enmInvalidAccountID;
		--nAccountIDCount;

		sprintf(szSql, "update vdc_account.normal_account set `appointtime` = 1  where `accountid` = %d and `appointtime` != 1", nAccountID);

		uint64_t nAffectedRows = 0;
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(ret < 0)
		{
			nAccountID = enmInvalidAccountID;
			continue;
		}

		if(0 == nAffectedRows)
		{
			nAccountID = enmInvalidAccountID;
			continue;
		}

		break;
	}

	return nAccountID;
}

bool CFromHallGetEvent::AddUserBaseInfo(const char *szAccountName, AccountID nAccountID, RoleID nRoleID, VipLevel nVipLevel, int32_t nOSVersion,
		const char *szMacAddr, const char *szRegIP)
{
	char szSql[enmMaxSqlStringLength] = {0};
	char arrTime[64] = {0};
	CDateTime::CurrentDateTime().ToDateTimeString(arrTime);

	sprintf(szSql, "insert into vdc_user.user_base_info(`RoleID`, `RoleName`,`AccountName`, `AccountID`, `VipLevel`, `CreateTime`, "
			"`OSVersion`, `MacAddr`, `RegIP`) values (%d, '%s', '%s', %d, %d, '%s', %d, '%s', '%s')", nRoleID, szAccountName,
			szAccountName, nAccountID, nVipLevel, arrTime, nOSVersion, szMacAddr, szRegIP);

	uint64_t nAffectedRows = 0;
	int32_t nRet = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(nRet < 0)
	{
		WRITE_ERROR_LOG("add user base info error{sql=%s}\n", szSql);
		return false;
	}

	return true;
}

int32_t CFromHallGetEvent::ProcessHallLoginGetRolebaseinfoReq(const CHallGetRoleLoginInfoReq_Public* reqbody, CHallGetRoleInfoResp_Public& respbody)
{
	//枷锁:貌似不需要， 因为在获取处理器时已经枷锁了
	//DB_MUTEX_GUARD(cf_mutex, m_dbcrit);

	int32_t ret = S_OK;

	//写缓存必须的参数声明
	char szKey[enmMaxMemcacheKeyLen] = {0};
	size_t keylen = 0;
	char szVal[enmMaxMemcacheValueLen] = {0};
	size_t vallen = 0;
	char* pVal = NULL;
	uint32_t offset = 0;

	//CDBEngine  m_dbEngine;

	//玩家基本信息结构
	RoleBaseInfo rolebaseinfo;


	CVDCUserBaseInfo::VDCUserBaseInfoInit(rolebaseinfo);

	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();

	//构造key
	GenerateMemcacheKey(szKey,enmMaxMemcacheKeyLen, keylen, enmStoreType_RoleID, reqbody->nRoleID);

	//首先查找cache中是否存在
	ret = cacheobj.MemcacheIfKeyExist(cacheobj.m_memc, szKey, keylen );
	if(0 > ret)
	{
		//cache中不存在此人
		WRITE_NOTICE_LOG("notice: this role is not exist in cache! we need to query its role info from db! roleid=%d \n",
				reqbody->nRoleID );

		//首先从数据库中查询
		ret = QueryRoleBaseInfo(reqbody->nRoleID, rolebaseinfo );
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "Error: role is not existed in cache, and query its info from db failed! roleid=%d, errorcode=0x%08X \n",
					reqbody->nRoleID, ret );
			return ret;
		}
		//WRITE_DEBUG_LOG( "role is not existed in cache, and query role base info from db success! rolied=%d \n",
		//		reqbody->nRoleID );

		if(rolebaseinfo.nAccountID == enmInvalidAccountID)
		{
			AccountID nAccountID = GetNormalAccount();
			if(nAccountID == enmInvalidAccountID)
			{
				WRITE_ERROR_LOG("get a normal account failed!{accountname=%s, roleid=%d}\n", reqbody->strAccountName.GetString(), reqbody->nRoleID);
				return 0;
			}

			rolebaseinfo.ucVipLevel = enmVipLevel_Regist;
			rolebaseinfo.nAccountID = nAccountID;
			strncpy(rolebaseinfo.szAccountName, reqbody->strAccountName.GetString(), reqbody->strAccountName.GetStringLength());
			strncpy(rolebaseinfo.szRoleName, rolebaseinfo.szAccountName, strlen(rolebaseinfo.szAccountName));

			if(!AddUserBaseInfo(reqbody->strAccountName.GetString(), nAccountID, reqbody->nRoleID, rolebaseinfo.ucVipLevel, reqbody->nOSVersion,
					reqbody->strMacAddr.GetString(), inet_ntoa_f(reqbody->nIpAddr)))
			{
				WRITE_ERROR_LOG("add user base info failed!{accountname=%s, roleid=%d}\n", reqbody->strAccountName.GetString(), reqbody->nRoleID);
				return 0;
			}
		}

		ret = CVDCUserBaseInfo::VDCUserBaseInfoEncode((uint8_t *)szVal, enmMaxMemcacheValueLen, offset, rolebaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "Error: encode role base info failed! roleid=%d, errorcode=0x%08X \n",
					reqbody->nRoleID, ret );
			return ret;
		}
		//WRITE_DEBUG_LOG( "encode role base info success! roleid=%d \n",
		//		reqbody->nRoleID );

		ret = cacheobj.MemcacheSet(cacheobj.m_memc, szKey, keylen, szVal, offset, 0 );
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "set role base info struct to cache failed! key=%s, keylen=%d, errorcode=0x%08X \n",
					szKey, keylen, ret );
			return ret;
		}
		WRITE_DEBUG_LOG( "set role base info struct to cache success! key=%s \n", szKey );
	}
	else
	{
		//缓存中存在此人
		ret = cacheobj.MemcacheGet(cacheobj.m_memc, szKey, keylen, pVal, vallen );
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "this role is exists in cache, but fetch it from cache failed! roleid=%d, errorcode=0x%08X \n",
					reqbody->nRoleID, ret );
			return ret;
		}
		WRITE_DEBUG_LOG( "this role is exist in cache, fetch it success! key=%s \n", szKey );

		offset = 0;
		ret = CVDCUserBaseInfo::VDCUserBaseInfoDecode((uint8_t *)pVal, enmMaxMemcacheValueLen, offset, rolebaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "decode role base info get from cache failed! roleid=%d, errorcode=0x%08X ",
					reqbody->nRoleID, ret );
			return ret;
		}
		WRITE_DEBUG_LOG( "decode role base info get from cache success! roleid=%d \n",
				reqbody->nRoleID );
	}

	respbody.nRoleID = reqbody->nRoleID;
	respbody.strAccountName = rolebaseinfo.szAccountName;
	respbody.nAccountID = rolebaseinfo.nAccountID;
	respbody.nLastVersion = 0;
	respbody.nLastLoginTime = rolebaseinfo.nLastLoginTime;
	respbody.nSystemTime = (uint64_t)time(NULL);
	respbody.nGender = rolebaseinfo.ucGender;
	respbody.strRoleName = rolebaseinfo.szRoleName;
	respbody.nVipLevel = rolebaseinfo.ucVipLevel;
	respbody.nUserLevel = rolebaseinfo.ucUserLevel;
	respbody.nLeftMoney = rolebaseinfo.nMoney;
	respbody.nLastLogoutTime = rolebaseinfo.nLastLogoutTime;
	respbody.nExperience = rolebaseinfo.nExperience;
	respbody.uRoleIdentity = rolebaseinfo.uIdentity;
	respbody.nBecomeVipTime = rolebaseinfo.nBecomeVipTime;
	respbody.nAccountSafeType = rolebaseinfo.nAccountSafeType;
	respbody.nRmbUserType = rolebaseinfo.nRmbUserType;

	//财富等级
	respbody.nMagnatelevel = rolebaseinfo.nMagnateLevel;
	respbody.nConsume = rolebaseinfo.nConsume;
	respbody.nNextlevelMoney = rolebaseinfo.nNextLevelConusme;

	//查询该玩家是那些房间的管理员以及在这些房间中的管理权限
	ret = QueryRoleRoomMember(reqbody->nRoleID, respbody.nAdminCount, respbody.arrRoomIDS, respbody.arrRoleRank, MaxBeAdminPerPlayer );
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query role room member failed! errorcode=0x%08X, roleid=%d\n",
				ret, reqbody->nRoleID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query role room member success! role room admin count=%d\n",
	//		respbody.nAdminCount);

	//查询玩家的179ID
//	ret = QueryRole179ID(reqbody->nRoleID, respbody.nAccountID );
//	if(0 > ret)
//	{
//		WRITE_ERROR_LOG( "query user 179id failed! roleid=%d, errorcode=0x%08X\n",
//				reqbody->nRoleID, ret);
//		return ret;
//	}
	//WRITE_DEBUG_LOG( "queyr user 179id success! roleid=%d, 179id=%d\n",
	//		reqbody->nRoleID, respbody.n179ID);

	//查询玩家总在线时长
	uint32_t nTempTotalOnlineTime = 0;
	ret = QueryUserTotalOnlineTime(reqbody->nRoleID, nTempTotalOnlineTime );
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query user total online time failed! errorcode=0x%08X, roleid=%d\n",
				ret, reqbody->nRoleID);
		respbody.nResult = ret;
	}
	respbody.nOnlineTime = (uint64_t)nTempTotalOnlineTime;
	//WRITE_DEBUG_LOG( "query user total online time success! roleid=%d\n", reqbody->nRoleID);

	//查询玩家自身设置
	ret = QueryRoleSelfSetInfo(reqbody->nRoleID, respbody.nUserInfoSelfSet );
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query user self set info failed! errorcode=0x%08X, roleid=%d\n",
				ret, reqbody->nRoleID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query user self set info success! roleid=%d\n", reqbody->nRoleID);

	//查询玩家账号封锁信息，默认为没有被封锁
	char szLockReason[MaxLockPlayerReasonLength] = {0};
	ret = QueryUserAccountLock(reqbody->nRoleID, respbody.nTimelock, respbody.nTimeUnlock, szLockReason );
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query user account lock table failed! errorcode=0x%08X, roleid=%d\n",
				ret, reqbody->nRoleID);
		return ret;
	}
	respbody.strAccountLockReason = szLockReason;
	//WRITE_DEBUG_LOG( "query user account lock table success! roleid=%d\n", reqbody->nRoleID);

	//查询封IP相关信息
	char strIP[enmMaxIPAddressLength];
	char* ptrIP = NULL;
	ptrIP = inet_ntoa_s((int32_t)reqbody->nIpAddr, strIP);
	memset(szLockReason, 0, sizeof(szLockReason));
	ret = QueryLockIPInfo(ptrIP, respbody.nLockIPStartTime, respbody.nLockIPEndTime, szLockReason );
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query lock ip end time failed! roleid=%d, errorcode=0x%08X\n",
				reqbody->nRoleID, ret);
		return ret;
	}
	respbody.nIpAddr = reqbody->nIpAddr;
	respbody.strLockIpReason = szLockReason;
	//WRITE_DEBUG_LOG( "query lock ip end time success! roleid=%d, lockip=%s\n",
	//		reqbody->nRoleID, IPTrans_from_int_to_string((int32_t)reqbody->nIpAddr));

	//查询封mac相关信息
	memset(szLockReason, 0, sizeof(szLockReason));
	ret = QueryLockMacInfo(reqbody->strMacAddr.GetString(), respbody.nLockMacStartTime, respbody.nLockMacEndTime, szLockReason );
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query lock mac end time failed! roleid=%d, errorcode=0x%08X\n",
				reqbody->nRoleID, ret);
		return ret;
	}
	respbody.strMacAddr = reqbody->strMacAddr;
	respbody.strLockMacReason = szLockReason;
	//WRITE_DEBUG_LOG( "query lock mac endtime success! roleid=%d, macaddr=%s\n",
	//		reqbody->nRoleID, reqbody->strMacAddr.GetString());

	//查询玩家的成就
	int32_t achieve_count = 0;
	int32_t achieve[MaxAchieveCount];
	memset(achieve, 0, sizeof(achieve));
	ret = QueryAchievement(reqbody->nRoleID, achieve_count, achieve, MaxAchieveCount );
	if(achieve_count > 0)
	{
		for(int32_t i=0; i<achieve_count; ++i)
		{
			respbody.uRoleIdentity |= achieve[i];
		}
	}

	//登陆渠道号原值返回
	respbody.nChannelID = reqbody->nChannelID;

	respbody.nResult = S_OK;
	return S_OK;
}
int32_t CFromHallGetEvent::QueryRoleRoomMember(const RoleID roleid, int32_t& admincount, RoomID arrRoomID[], RoleRank arrRoleRank[], const int32_t max_admin_per_user)
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
		WRITE_DEBUG_LOG( "this user is not admin in any room! roleid=%d, sql=%s\n", roleid, szSql);
		admincount = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "this user is admin in some room! we will fetch it below! roleid=%d, sql=%s\n",
	//		roleid, szSql);

	admincount = (int32_t)nRowCount;

	//取出所有查询到的记录（多条记录）
	for(uint32_t i=0; i<nRowCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "error: db operation of fetch row on query role roomadmin info failed! roleid=%d\n", roleid);
			return ret;
		}
		WRITE_DEBUG_LOG( "db operation of fetch row on query role roomadmin success! roleid=%d\n", roleid);

		arrRoomID[i] = (RoomID)atoi(arrRecordSet[0]);
		arrRoleRank[i] = (RoleRank)atoi(arrRecordSet[1]);
	}

	return S_OK;
}
int32_t CFromHallGetEvent::QueryUserTotalOnlineTime(const RoleID roleid, uint32_t& total_online_time)
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
	//WRITE_DEBUG_LOG("db operation on query role total online time success! rowcount=%d, sql=%s\n", nRowCount, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in db of total online time table! roleid=%d, sql=%s\n", roleid, szSql);
		total_online_time = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG("record exist of total online time table! roleid=%d\n", roleid);

	total_online_time = (uint32_t)atoi(arrRecordSet[0]);

	return S_OK;
}
int32_t CFromHallGetEvent::QueryRoleSelfSetInfo(const RoleID roleid, uint32_t& nUserInfoSelfSet)
{
	if(enmInvalidRoleID == roleid)
	{
		WRITE_ERROR_LOG("roleid(179uin) on query user self set info is invalid! roleid=%d\n", roleid);
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG("roleid(179uin) on query user self set info is right! roleid=%d\n", roleid);

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
		WRITE_ERROR_LOG("db operation on query role self set info failed! errorcode=0x%08X, roleid(179uin)=%d, sql=%s\n",
				ret, roleid, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query role self set info success! roleid=%d, sql=%s\n",		roleid, szSql);

	if(nRowCount == 0)
	{
		WRITE_WARNING_LOG("unknown error happened while query user self set info! no such record in table user_config_info! roleid=%d\n", roleid);
		nUserInfoSelfSet = 0;
		return S_OK;
	}
	nUserInfoSelfSet = (uint32_t)atoi(arrRecordSet[0]);
	//WRITE_DEBUG_LOG("db operation on query user self set info success! roleid=%d, nUserInfoSelfSet=%d\n", roleid, nUserInfoSelfSet);

	return S_OK;
}
int32_t CFromHallGetEvent::QueryUserAccountLock(const RoleID roleid, uint64_t& nTimeLock, uint64_t& nTimeUnlock, char szLockReason[])
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select  `StartLockTime`, `EndLockTime`, `LockReason` from vdc_user.`user_account_lock` where `RoleID`=%d",
			roleid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query role account lock info failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query role role account lock info success! rowcount=%d, sql=%s\n", nRowCount, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in db of role account lock info table!sql=%s\n",szSql);
		nTimeLock = 0;
		nTimeUnlock = 0;
		memset(szLockReason, 0, sizeof(szLockReason));
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist of role account lock info table! roleid=%d\n", roleid);

	nTimeLock = (uint64_t)atoi(arrRecordSet[0]);
	nTimeUnlock = (uint64_t)atoi(arrRecordSet[1]);

	memset(szLockReason, 0, MaxLockPlayerReasonLength);
	strcpy_safe(szLockReason, MaxLockPlayerReasonLength, arrRecordSet[2], strlen(arrRecordSet[2]));

	return S_OK;
}
int32_t CFromHallGetEvent::QueryLockIPInfo(const char* ip, uint64_t& start_lock_time, uint64_t& end_lock_time, char* lock_reason)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `StartLockTime`, `EndLockTime`, `LockReason`  from vdc_user.`ip_lock` where `IPAddr`='%s'", ip);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query ip lock endtime failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query ip lock endtime success! rowcount=%d, sql=%s\n", nRowCount, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in table ip_lock!, sql=%s\n",szSql);
		start_lock_time = 0;
		end_lock_time = 0;
		lock_reason = NULL;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist in table ip_lock\n");

	CDateTime dt;
	dt = CDateTime(arrRecordSet[0]);
	start_lock_time = dt.Seconds();
	dt = CDateTime(arrRecordSet[1]);
	end_lock_time = dt.Seconds();
	//strcpy(lock_reason, arrRecordSet[2]);
	strcpy_safe(lock_reason, MaxLockPlayerReasonLength, arrRecordSet[2], strlen(arrRecordSet[2]));

	return S_OK;
}
int32_t CFromHallGetEvent::QueryLockMacInfo(const char* mac, uint64_t& start_lock_time, uint64_t& end_lock_time, char* lock_reason)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select  `StartLockTime`, `EndLockTime`, `LockReason`  from vdc_user.`mac_lock` where `MacAddr`='%s'", mac);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query mac lock endtime failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query mac lock endtime success! rowcount=%d, sql=%s\n", nRowCount, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in table mac_lock!, sql=%s\n",szSql);
		start_lock_time = 0;
		end_lock_time = 0;
		lock_reason = NULL;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist in table mac_lock\n");

	CDateTime dt;
	dt = CDateTime(arrRecordSet[0]);
	start_lock_time = dt.Seconds();
	dt = CDateTime(arrRecordSet[1]);
	end_lock_time = dt.Seconds();
	//strcpy(lock_reason, arrRecordSet[2]);
	strcpy_safe(lock_reason, MaxLockPlayerReasonLength, arrRecordSet[2], strlen(arrRecordSet[2]));

	return S_OK;
}
int32_t CFromHallGetEvent::QueryAchievement(const RoleID roleid, int32_t& achieve_count, int32_t achieve[], const int32_t max_achieve_count)
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
	//WRITE_DEBUG_LOG( "db operation on query role achievement success! roleid=%d, sql=%s\n", roleid, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "this player has no achievement yet! roleid=%d\n", roleid);
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

int32_t CFromHallGetEvent::OnMessage_HallGetRoomAdmcount(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: hallserver get roomadmincount!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CHallGetRoomAdminCountReq* pTmpBody = dynamic_cast<CHallGetRoomAdminCountReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CHallGetRoomAdminCountResp get_roomadmincount;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBHS_GETROOM_ADMIN_COUNT_RESP);

	ret = ProcessHallGetRoomAdmCountReq(*pTmpBody, get_roomadmincount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process hallserver login get roomadmincount request failed! errorcode=0x%08X\n",
				ret);
		get_roomadmincount.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_roomadmincount, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_roomadmincount, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	//WRITE_DEBUG_LOG( "process hallserver login get roomadmincount success! \n");

	get_roomadmincount.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_roomadmincount, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_roomadmincount, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromHallGetEvent::ProcessHallGetRoomAdmCountReq(const CHallGetRoomAdminCountReq& reqbody, CHallGetRoomAdminCountResp& respbody)
{
	int32_t ret = S_OK;
	ret = QueryRoomAdminCount(reqbody.uRoomID, reqbody.usRoleRank, respbody.nAdminCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query room admin count from db failed! errorcode=0x%08X, roomid=%d, rolerank=%d\n",
				ret,
				reqbody.uRoomID,
				reqbody.usRoleRank);
		return ret;
	}
	WRITE_DEBUG_LOG( "query room admin count from db success! threadindex=%d, roomid=%d, rolerank=%d, admincount=%d\n",
			reqbody.uRoomID,
			reqbody.usRoleRank,
			respbody.nAdminCount);

	respbody.uRoomID = reqbody.uRoomID;
	respbody.usRoleRank = reqbody.usRoleRank;

	ret = QueryRoomShowCapacity(reqbody.uRoomID, respbody.nRoomShowCapacity);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query room show capacity failed! errorcode=0x%08X, roomid=%d\n",
				ret, reqbody.uRoomID);
		respbody.nResult = ret;
		return ret;
	}
	WRITE_DEBUG_LOG( "query room show capacity success! roomid=%d, room show capacity %d\n",
			reqbody.uRoomID, respbody.nRoomShowCapacity);

	respbody.nResult = S_OK;

	return S_OK;
}
int32_t CFromHallGetEvent::QueryRoomAdminCount(const RoomID roomid, const RoleRank rolerank, int32_t& admincount)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select count(*) from vdc_room.room_member where `RoomID`=%d and `RoleRank`=%d", roomid, rolerank);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	//WRITE_DEBUG_LOG( "query roomadmin count sql: %s\n", szSql);
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query room admin of specific rolerank failed! errorcode=0x%08X, roomid=%d, rolerank=%d, sql=%s\n",
				ret,
				roomid,
				rolerank,
				szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on queyr room admin count success! roomid=%d, sql=%s\n", roomid, szSql);

	if(NULL == arrRecordSet[0])
	{
		WRITE_WARNING_LOG( "no record exist in db of roomadmin count! roomid=%d, sql=%s\n",
				roomid, szSql);
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist in db of roomadmin count! roomid=%d, sql=%s\n", roomid, szSql);

	admincount = atoi(arrRecordSet[0]);
	if(0 > admincount)
	{
		WRITE_ERROR_LOG( "record set result is below zero! atoi(arrRecordSet[0])=%d\n", atoi(arrRecordSet[0]));
		return E_RECORDSET_BELOW_ZERO;
	}
	WRITE_DEBUG_LOG( "db operation on query room admin of specific rolerank success! roomid=%d, rolerank=%d, admincount=%d\n",
			roomid,
			rolerank,
			admincount);

	return S_OK;
}
int32_t CFromHallGetEvent::QueryRoomShowCapacity(const RoomID roomid, int32_t& roomShowCapacity)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `RoomShowCapacity` from vdc_room.room_base_info where `RoomID`=%d", roomid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query room realcapacity failed! errorcode=0x%08X, roomid=%d, sql=%s\n",
				ret,
				roomid,
				szSql);
		return ret;
	}
	roomShowCapacity = atoi(arrRecordSet[0]);
	//WRITE_DEBUG_LOG( "db operation on query room real capacity success! roomid=%d, roomRealCapacity=%d, sql=%s\n",roomid,roomShowCapacity,szSql);

	return S_OK;
}

int32_t CFromHallGetEvent::OnMessage_HallGetRoomInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen , const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: hallserver get roominfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CHallGetRoomInfoReq_Public* pTmpBody = dynamic_cast<CHallGetRoomInfoReq_Public*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CHallGetRoomInfoResp_Public get_roominfo;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBHS_GETROOMINFO_RESP);

	ret = ProcessHallGetRoomInfoReq(*pTmpBody, get_roominfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process hallserver get roominfo request failed! errorcode=0x%08X\n",
				ret);
		get_roominfo.nResult = ret;
		g_Frame.PostMessage(&resphead, &get_roominfo, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &get_roominfo, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process hallserver  get roominfo success! \n");

	get_roominfo.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &get_roominfo, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &get_roominfo, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromHallGetEvent::ProcessHallGetRoomInfoReq(const CHallGetRoomInfoReq_Public& reqbody, CHallGetRoomInfoResp_Public& respbody)
{
	if(reqbody.nRoomID <= 0)
	{
		WRITE_ERROR_LOG( "room id is invalid on hall login get room info request! roomid=%d\n",
				reqbody.nRoomID);
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "room id is right on hall login get room info request! roomid=%d\n",reqbody.nRoomID);

	char szKey[enmMaxMemcacheKeyLen] = {0};
	size_t nKeyLen = 0;
	size_t nValueLen = 0;
	char* pValPointer = NULL;

	//dump玩家的基本信息时的缓冲区及其长度
	char szRoomBaseInfoBuf[enmMaxRoomBaseInfoBufLen] = {0};
	uint32_t offset = 0;

	CVDCMemcache& memcacheObj = GET_VDCMEMCACHE_INSTANCE();

	RoomBaseInfo roombaseinfo;
	//memset(&roombaseinfo, 0, sizeof(roombaseinfo));

	//构造key
	GenerateMemcacheKey(szKey, enmMaxMemcacheKeyLen, nKeyLen, enmStoreType_RoomID, reqbody.nRoomID);

	int32_t ret = S_OK;

	//查询在缓存中是否存在
	ret = memcacheObj.MemcacheIfKeyExist(memcacheObj.m_memc, szKey, nKeyLen);
	if(0 > ret)
	{
		//在缓存中不存在，打日志
		WRITE_INFO_LOG( "this room is  not exist in memcache! roomid=%d, key=%s\n",
				reqbody.nRoomID,
				szKey);

		//从数据库中获取该房间信息
		ret = QueryRoomBaseInfo(reqbody.nRoomID, roombaseinfo);
		if(0 > ret)
		{
			if((int32_t)E_NOT_EXIST_IN_DB == ret)
			{
				WRITE_WARNING_LOG( "room not exist in cache and query room info from db failed! roomid=%d, errorcode=0x%08X\n",
						reqbody.nRoomID,
						ret);
			}
			else
			{
				WRITE_ERROR_LOG( "room not exist in cache and query room info from db failed! roomid=%d, errorcode=0x%08X\n",
						reqbody.nRoomID,
						ret);
			}

			return ret;
		}
		//WRITE_DEBUG_LOG( "query room base info from db success! roomid=%d\n", reqbody.nRoomID);

		//将房间基本信息编码
		offset = 0;
		ret = CVDCRoomBaseInfo::VDCRoomBaseInfoEncode((uint8_t*)szRoomBaseInfoBuf, enmMaxRoomBaseInfoBufLen, offset, roombaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "encode room base info into temp buffer failed! roomid=%d, errorcode=0x%08X\n",
					reqbody.nRoomID,
					ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "encode room base info into temp buffer success! roomide=%d, encoded length=%d\n",reqbody.nRoomID,offset);

		//将房间基本信息写入缓存
		ret = memcacheObj.MemcacheSet(memcacheObj.m_memc, szKey, nKeyLen, szRoomBaseInfoBuf, (size_t)offset, 0);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "write room base info into memcache failed! roomid=%d, key=%s, errorcode=0x%08X\n",
					reqbody.nRoomID,
					szKey,
					ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "writet room base info into memcache success! roomid=%d, key=%s\n",reqbody.nRoomID,szKey);

		respbody.nResult = S_OK;
		respbody.nRegionType = roombaseinfo.ucRegionType;
		respbody.nRegionID = roombaseinfo.usRegionID;
		respbody.strRegionName = roombaseinfo.szRegionName;
		respbody.nChannelType = roombaseinfo.ucChannelType;
		respbody.nChannelID = roombaseinfo.usChannelID;
		respbody.strChannelName = roombaseinfo.szChannelName;
		respbody.nRoomType = roombaseinfo.ucRoomType;
		respbody.nRoomID = reqbody.nRoomID;
		respbody.strRoomName = roombaseinfo.szRoomName;

		return S_OK;
	}


	//从缓存中获取基本信息
	ret = memcacheObj.MemcacheGet(memcacheObj.m_memc, szKey, nKeyLen, pValPointer, nValueLen);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "get room base from memcache failed! key=%s\n",
				szKey);
		return ret;
	}
	//WRITE_DEBUG_LOG( "get room base info from memcache success! key=%s\n",szKey);

	//解码
	offset = 0;
	ret = CVDCRoomBaseInfo::VDCRoomBaseInfoDecode((uint8_t*)pValPointer, (uint32_t)nValueLen, offset, roombaseinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "decode room base info get from cache failed! roomid=%d, errorcode=0x%08X\n",
				reqbody.nRoomID,
				ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "decode room base info from memcache success! roomid=%d\n",reqbody.nRoomID);

	respbody.nResult = S_OK;
	respbody.nRegionType = roombaseinfo.ucRegionType;
	respbody.nRegionID = roombaseinfo.usRegionID;
	respbody.strRegionName = roombaseinfo.szRegionName;
	respbody.nChannelType = roombaseinfo.ucChannelType;
	respbody.nChannelID = roombaseinfo.usChannelID;
	respbody.strChannelName = roombaseinfo.szChannelName;
	respbody.nRoomType = roombaseinfo.ucRoomType;
	respbody.nRoomID = reqbody.nRoomID;
	respbody.strRoomName = roombaseinfo.szRoomName;

	return S_OK;
}

FRAME_DBPROXY_NAMESPACE_END
