/*
 * bll_event_hallset_msg.cpp
 *
 *  Created on: 2013-3-6
 *      Author: liufl
 */

#include "bll_event_hallset_msg.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

int32_t CFromHallSetEvent::OnMessage_OnlineNotice(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver get RoleInfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	uint32_t offset = 0;

	//	int32_t threadindex =         需要获取线程ID
	COnlineNoti* pTmpBody = dynamic_cast<COnlineNoti*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}

	//更新数据库中的lasgLoginIP
	ret = UpdateLastLoginIP(pTmpBody->nRoleID);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update player last login ip failed! errorcode=0x%08X, roleid=%d\n",
				ret, pTmpBody->nRoleID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update player last login ip success! roleid=%d\n", pTmpBody->nRoleID);

	//更新数据库中的LoginIP
	char strIP[enmMaxIPAddressLength];
	ret = UpdateLoginIP(pTmpBody->nRoleID, inet_ntoa_s(pTmpBody->nConnUin.nAddress , strIP));
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update player login ip failed! errorcode=0x%08X, roleid=%d\n",
				ret, pTmpBody->nRoleID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update player login ip success! roleid=%d\n", pTmpBody->nRoleID);

	//更新数据表中的lastLoginTime字段： LastLoginTime
	ret = UpdateLastLoginTime(pTmpBody->nRoleID, pTmpBody->nLoginTime);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update player last login time failed! errorcode=0x%08X, roleid=%d\n",
				ret, pTmpBody->nRoleID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update player last login time success! roleid=%d\n", pTmpBody->nRoleID);

	//更新登陆次数，每收到通知则加1
	ret = UpdateLoginTimes(pTmpBody->nRoleID);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update player login times failed! errorcode=0x%08X, roleid=%d\n",
				ret, pTmpBody->nRoleID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update player login times success! roleid=%d\n", pTmpBody->nRoleID);

	//玩家基本信息结构
	RoleBaseInfo rolebaseinfo;
	CVDCUserBaseInfo::VDCUserBaseInfoInit(rolebaseinfo);

	//写缓存必须的参数声明
	char szKey[enmMaxMemcacheKeyLen] = {0};
	size_t keylen = 0;
	//	char szVal[enmMaxMemcacheValueLen] = {0};
	char* pVal = NULL;
	size_t vallen = 0;

	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();

	//获取key值
	GenerateMemcacheKey(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoleID, pTmpBody->nRoleID);

	//首先查找cache中是否存在
	ret = cacheobj.MemcacheIfKeyExist(cacheobj.m_memc, szKey, keylen );
	if(ret >= 0)
	{
		//取出缓存信息并更新
		ret = cacheobj.MemcacheGet(cacheobj.m_memc, szKey, keylen, pVal, vallen );
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "this role is exists in cache, but fetch it from cache failed! roleid=%d, errorcode=0x%08X \n",
					pTmpBody->nRoleID, ret );
			return ret;
		}
		WRITE_DEBUG_LOG( "this role is exist in cache, fetch it success! key=%s \n", szKey );

		offset = 0;
		ret = CVDCUserBaseInfo::VDCUserBaseInfoDecode((uint8_t *)pVal, enmMaxMemcacheValueLen, offset, rolebaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "decode role base info get from cache failed! roleid=%d, errorcode=0x%08X ",
					pTmpBody->nRoleID, ret );
			return ret;
		}
		WRITE_DEBUG_LOG( "decode role base info get from cache success! roleid=%d \n",
				pTmpBody->nRoleID );

		rolebaseinfo.nRoleID = pTmpBody->nRoleID;
		strcpy(rolebaseinfo.szLastLoginIP , rolebaseinfo.szLoginIP);
		strcpy(rolebaseinfo.szLoginIP, strIP);
		rolebaseinfo.nLastLoginTime = pTmpBody->nLoginTime;
		rolebaseinfo.nLoginTimes += 1;
	}
	else
	{
		//获取用户基本信息
		ret = QueryRoleBaseInfo(pTmpBody->nRoleID, rolebaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "query player base info failed! errorcode=0x%08X, roleid=%d\n",
					ret, pTmpBody->nRoleID);
			return ret;
		}
		//WRITE_DEBUG_LOG( "query player base info success! roleid=%d\n", pTmpBody->nRoleID);

		rolebaseinfo.nRoleID = pTmpBody->nRoleID;

	}

	UpdateMemcache(rolebaseinfo);

	return S_OK;

}
int32_t CFromHallSetEvent::UpdateLastLoginIP(const RoleID roleid)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	uint64_t nAffectedRows = 0;
	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "update vdc_user.user_base_info set `LastLoginIP`=`LoginIP`  where `RoleID`=%d", roleid);

	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "Error: execute update last login ip failed! errorcode=0x%08X, roleid=%d, sql=%s\n",
				ret, roleid, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "execute update last login ip success! roleid=%d, sql=%s\n",roleid, szSql);

	return S_OK;
}
int32_t CFromHallSetEvent::UpdateLoginIP(const RoleID roleid, const char* szLoginIP)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	uint64_t nAffectedRows = 0;
	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "update vdc_user.user_base_info set `LoginIP`='%s' where `RoleID`=%d", szLoginIP, roleid);

	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update player loginIP failed! errorcode=0x%08X, roleid=%d, sql=%s\n",
				ret, roleid, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update player loginIP success! roleid=%d, sql=%s\n",roleid, szSql);

	return S_OK;
}
int32_t CFromHallSetEvent::UpdateLastLoginTime(const RoleID roleid, const int64_t lastlogintime)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	uint64_t nAffectedRows = 0;
	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "update vdc_user.user_base_info set `LastLoginTime`=%ld where `RoleID`=%d",
			lastlogintime, roleid);

	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update player last login time failed! errorcode=0x%08X, roleid=%d, sql=%s\n",
				ret, roleid, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update player last login time success! roleid=%d, sql=%s\n",roleid, szSql);

	return S_OK;
}
int32_t CFromHallSetEvent::UpdateLoginTimes(const RoleID roleid)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	uint64_t nAffectedRows = 0;
	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "update vdc_user.user_base_info set `LoginTimes`=`LoginTimes` + 1 where `RoleID`=%d", roleid);

	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update player logintimes failed! errorcode=0x%08X, roleid=%d, sql=%s\n",
				ret, roleid, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update player logintimes success! roleid=%d, sql=%s\n", roleid, szSql);

	return S_OK;
}

int32_t CFromHallSetEvent::OnMessage_SetLoginInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver get RoleInfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	uint32_t offset = 0;

	//	int32_t threadindex =         需要获取线程ID
	CSetLoginInfoNoti* pTmpBody = dynamic_cast<CSetLoginInfoNoti*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	//更新上次登录的时间
	ret = UpdateLastLoginTime(pTmpBody->nRoleID, pTmpBody->nLoginTime);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update player last logintime failed! roleid=%d, errorcode=0x%08X\n",
				pTmpBody->nRoleID, ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update player last logintime success! roleid=%d\n", pTmpBody->nRoleID);

	//更新上次登录IP
	ret = UpdateLastLoginIP(pTmpBody->nRoleID);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update player last login ip failed! roleid=%d, errorcode=0x%08X\n",
				pTmpBody->nRoleID, ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update player last login ip success! roleid=%d\n", pTmpBody->nRoleID);

	//更新本次登录IP

	char szLoginIP[enmMaxIPAddressLength] = {0};
	inet_ntoa_s(pTmpBody->nIPAddr , szLoginIP);
	ret = UpdateLoginIP(pTmpBody->nRoleID, szLoginIP);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update this login ip failed! errrocode=0x%08X, roleid=%d\n",
				ret, pTmpBody->nRoleID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update this login ip success! roleid=%d\n", pTmpBody->nRoleID);

	//更新登录次数
	ret = UpdateLoginTimes(pTmpBody->nRoleID);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update player login times failed! roleid=%d, errorcode=0x%08X\n",
				pTmpBody->nRoleID, ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update player login times success! roleid=%d\n", pTmpBody->nRoleID);

	//更新操作系统版本号
	ret = UpdateOSVersion(pTmpBody->nRoleID, pTmpBody->nOSVersion);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update client operationsystem version failed! roleid=%d, errorcode=0x%08X\n",
				pTmpBody->nRoleID, ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update client operation system version success! roleid=%d\n", pTmpBody->nRoleID);

	//更新mac地址
	char szMacAddr[MaxMacAddrStringLength] = {0};
	strcpy(szMacAddr, pTmpBody->strMacAddr.GetString());
	ret = UpdateMacAddr(pTmpBody->nRoleID, szMacAddr);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update client mac address failed! roleid=%d, errorcode=0x%08X\n",
				pTmpBody->nRoleID, ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update client mac address success! roleid=%d\n", pTmpBody->nRoleID);

	//更新登陆渠道
	ret = UpdateLoginChannel(pTmpBody->nRoleID, pTmpBody->nChannel);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update player login channel failed! roleid=%d, errorcode=0x%08X\n",pTmpBody->nRoleID, pTmpBody->nChannel);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update player login channel success! roleid=%d\n", pTmpBody->nRoleID);

	//	//查询基本信息
	//	RoleBaseInfo rolebaseinfo;
	//	//	//memset(&rolebaseinfo, 0, sizeof(rolebaseinfo));
	//	ret = QueryRoleBaseInfo(pTmpBody->nRoleID, rolebaseinfo);
	//	if(0 > ret)
	//	{
	//		WRITE_ERROR_LOG( "query user base info failed! roleid=%d, errorcode=0x%08X\n",
	//				pTmpBody->nRoleID, ret);
	//		return ret;
	//	}
	//	//WRITE_DEBUG_LOG( "query user base info success! roleid=%d\n", pTmpBody->nRoleID);
	//
	//	//缓存相关声明
	//	char szKey[enmMaxMemcacheKeyLen] = {0};
	//	size_t keylen = 0;
	//	char szVal[enmMaxMemcacheValueLen] = {0};
	//	GenerateMemcacheKey(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoleID, pTmpBody->nRoleID);
	//	ret = CVDCUserBaseInfo::VDCUserBaseInfoEncode((uint8_t*)szVal, enmMaxMemcacheValueLen, offset, rolebaseinfo);
	//	if(0 > ret)
	//	{
	//		WRITE_ERROR_LOG( "encode rolebaseinfo failed! errorcode=0x%08X, roleid=%d, szkey=%s, keylen=%d\n",
	//				ret, pTmpBody->nRoleID, szKey, keylen);
	//		return ret;
	//	}
	//	//将基本信息写入缓存
	//	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();
	//	ret = cacheobj.MemcacheSet(cacheobj.m_memc, szKey, keylen, szVal, (size_t)offset, 0);
	//	if(0 > ret)
	//	{
	//		WRITE_ERROR_LOG( "write user base info into memcache failed! errorcode=0x%08X, roleid=%d\n",
	//				ret, pTmpBody->nRoleID);
	//		return ret;
	//	}
	//	//WRITE_DEBUG_LOG( "write user base info into memcache success! roleid=%d\n", pTmpBody->nRoleID);
	//
	//	//将web缓存删除掉
	//	memset(szKey, 0, sizeof(szKey));
	//	keylen = 0;
	//	GenerateMemcacheKeyForWeb(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoleID, pTmpBody->nRoleID);
	//	ret = cacheobj.MemcacheDel(cacheobj.m_memc, szKey, keylen, 0);
	//	if(0 > ret)
	//	{
	//		WRITE_ERROR_LOG( "delete memcache information for web failed! errorcode=0x%08X, key=%s, keylen=%d\n",
	//				ret, szKey, keylen);
	//		return ret;
	//	}
	//	WRITE_DEBUG_LOG( "delete memcache information for web success! key=%s, keylen=%d\n",szKey, keylen);

	return S_OK;
}
int32_t CFromHallSetEvent::UpdateOSVersion(const RoleID roleid, const uint32_t os_version)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	uint64_t nAffectedRows = 0;
	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "update vdc_user.user_base_info set `OSVersion`=%d where `RoleID`=%d", os_version, roleid);
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on update os version failed! errorcode=0x%08X, roleid=%d, sql=%s\n",
				ret, roleid, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on update os version success! roleid=%d, sql=%s\n",roleid, szSql);

	return S_OK;
}
int32_t CFromHallSetEvent::UpdateLoginChannel(const RoleID roleid, const uint32_t channel)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};
	uint64_t nAffectedRows = 0;
	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "update vdc_user.user_base_info set `LoginChannel`=%u where `RoleID`=%d", channel, roleid);
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on update login channel failed! errorcode=0x%08X, roleid=%d, sql=%s\n",
				ret, roleid, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on update login channel success! roleid=%d, sql=%s\n",roleid, szSql);

	return S_OK;
}
int32_t CFromHallSetEvent::UpdateMacAddr(const RoleID roleid, const char* szMacAddr)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};
	uint64_t nAffectedRows = 0;
	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "update vdc_user.user_base_info set `MacAddr`='%s' where `RoleID`=%d", szMacAddr, roleid);
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on update macaddress failed! errorcode=0x%08X, roleid=%d, sql=%s\n",
				ret, roleid, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on update mac address success! roleid=%d, sql=%s\n",roleid, szSql);

	return S_OK;
}

int32_t CFromHallSetEvent::OnMessage_SetLogoutInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver get RoleInfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	uint32_t offset = 0;
	//	int32_t threadindex =         需要获取线程ID
	CSetLogoutInfoNoti* pTmpBody = dynamic_cast<CSetLogoutInfoNoti*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	//更新退出时间
	ret = UpdateLastLogoutTime(pTmpBody->nRoleID, pTmpBody->nLogoutTime);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update last logout time failed! msgid=0x%08X, roleid=%d, errorcode=0x%08X\n",
				pMsgHead->nMessageID, pTmpBody->nRoleID, ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update last logout time success! roleid=%d\n", pTmpBody->nRoleID);

	//查询基本信息
	RoleBaseInfo rolebaseinfo;
	//	memset(&rolebaseinfo, 0, sizeof(rolebaseinfo));
	//	ret = QueryRoleBaseInfo(pTmpBody->nRoleID, rolebaseinfo);
	//	if(0 > ret)
	//	{
	//		WRITE_ERROR_LOG( "query user base info failed! roleid=%d, errorcode=0x%08X\n",
	//				pTmpBody->nRoleID, ret);
	//		return ret;
	//	}
	//	WRITE_DEBUG_LOG( "query user base info success! roleid=%d\n", pTmpBody->nRoleID);

	//缓存相关声明
	char szKey[enmMaxMemcacheKeyLen] = {0};
	size_t keylen = 0;
	char szVal[enmMaxMemcacheValueLen] = {0};

	offset = 0;
	char* pVal = NULL;
	size_t vallen = 0;

	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();

	GenerateMemcacheKey(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoleID, pTmpBody->nRoleID);

	ret = cacheobj.MemcacheIfKeyExist(cacheobj.m_memc, szKey, keylen );
	if (ret >= 0)

	{
		//取出缓存信息并更新
		ret = cacheobj.MemcacheGet(cacheobj.m_memc, szKey, keylen, pVal, vallen );
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "this role is exists in cache, but fetch it from cache failed! roleid=%d, errorcode=0x%08X \n",
					pTmpBody->nRoleID, ret );
			return ret;
		}
		WRITE_DEBUG_LOG( "this role is exist in cache, fetch it success! key=%s \n", szKey );

		offset = 0;
		ret = CVDCUserBaseInfo::VDCUserBaseInfoDecode((uint8_t *)pVal, enmMaxMemcacheValueLen, offset, rolebaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "decode role base info get from cache failed! roleid=%d, errorcode=0x%08X ",
					pTmpBody->nRoleID, ret );
			return ret;
		}
		WRITE_DEBUG_LOG( "decode role base info get from cache success! roleid=%d \n",
				pTmpBody->nRoleID );

		rolebaseinfo.nLastLogoutTime = pTmpBody->nLogoutTime;
		rolebaseinfo.nRoleID = pTmpBody->nRoleID;
	}
	else
	{
		ret = QueryRoleBaseInfo(pTmpBody->nRoleID, rolebaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "query user base info failed! roleid=%d, errorcode=0x%08X\n",
					pTmpBody->nRoleID, ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "query user base info success! roleid=%d\n", pTmpBody->nRoleID);
	}

	UpdateMemcache(rolebaseinfo);

	return S_OK;
}
int32_t CFromHallSetEvent::UpdateLastLogoutTime(const RoleID roleid, const int64_t last_logout_time)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};
	uint64_t nAffectedRows = 0;
	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "update vdc_user.user_base_info set `LastLogoutTime`=%ld where `RoleID`=%d", last_logout_time, roleid);
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on update last logout time failed! errorcode=0x%08X, roleid=%d, sql=%s\n",
				ret, roleid, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on update last logout time success! roleid=%d, sql=%s\n",roleid, szSql);

	return S_OK;
}

int32_t CFromHallSetEvent::OnMessage_SetRoleRank(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: hallserver set RoleRank!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CHallSetTitleReq* pTmpBody = dynamic_cast<CHallSetTitleReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CHallSetTitleResp set_rolerank;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBHS_SET_TITLE_RESP);

	//直接回应
	set_rolerank.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &set_rolerank, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &set_rolerank, nOptionLen, (const char*)pOptionData);

	ret = ProcessHallSetRoleRankReq(*pTmpBody, set_rolerank);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process hallserver set RoleRank request failed! errorcode=0x%08X\n",
				ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "process hallserver set RoleRank success! \n");

	return S_OK;
}
int32_t CFromHallSetEvent::ProcessHallSetRoleRankReq(const CHallSetTitleReq& reqbody, CHallSetTitleResp& respbody)
{
	if(reqbody.nDestRoleID < 0 || reqbody.nRoomID < 0)
	{
		WRITE_ERROR_LOG( "invalid arguement while process hall set rolerank in room request! roleid=%d, roomid=%d\n",
				reqbody.nDestRoleID,
				reqbody.nRoomID);
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "argument are all right while process hall set rolerank in room request! threadindex=%d\n");

	int32_t ret = S_OK;
	ret = UpdateRoleRankInRoom(reqbody.nRoomID, reqbody.nDestRoleID, reqbody.nRoleRank, reqbody.nSrcRoleID);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update role rank in room failed! roomid=%d, roleid=%d, rolerank=%d, errorcode=0x%08X\n",
				reqbody.nRoomID,
				reqbody.nDestRoleID,
				reqbody.nRoleRank,
				ret);
		respbody.nResult = ret;
		return ret;
	}
	WRITE_DEBUG_LOG( "update role rank in room success! roomid=%d, roleid=%d, rolerank=%d, operateorid=%d\n",
			reqbody.nRoomID,
			reqbody.nDestRoleID,
			reqbody.nRoleRank,
			reqbody.nSrcRoleID);

	respbody.nResult = S_OK;
	return S_OK;
}
int32_t CFromHallSetEvent::UpdateRoleRankInRoom(const RoomID roomid, const RoleID roleid, const RoleRank rolerank, const RoleID operatorid)
{
	if(0>roomid || 0 > roleid)
	{
		WRITE_ERROR_LOG( "argument is invalid while update rolerank in room in db! roleid=%d, roomid=%d\n", roleid, roomid);
		return E_INVALID_ARGUMENTS;
	}

	int32_t ret = S_OK;

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));
	char szSql[enmMaxSqlStringLength] = {0};
	char szOperatorName[enmMaxRoleNameLength] = {0};

	sprintf(szSql, "select `RoleName` from vdc_user.user_base_info where `RoleID`=%d", operatorid);

	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query update rolerank operator nickname failed! roleid=%d, errorcode=0x%08X, sql=%s\n",
				operatorid, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query update rolerank operator nickname success! roleid=%d, operatorid=%d, sql=%s\n",operatorid, operatorid, szSql);

	if(nRowCount == 0)
	{
		WRITE_WARNING_LOG( "user nickname is null in db! roleid=%d\n", operatorid);
		szOperatorName[0] = '\0';
	}
	else
	{
		strcpy_safe(szOperatorName, enmMaxRoleNameLength, arrRecordSet[0], strlen(arrRecordSet[0]));
	}

	CDateTime dt = CDateTime::CurrentDateTime();
	char szDateTime[enmMaxDateTimeStringLength] = {0};
	dt.ToDateTimeString(szDateTime);

	char szEscapeOperatorName[enmMaxRoleNameLength*2];
	memset(szEscapeOperatorName, 0, sizeof(szEscapeOperatorName));
	mysql_escape_string(szEscapeOperatorName, szOperatorName, strlen(szOperatorName));

	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "insert into vdc_room.room_member(`RoleID`, `RoomID`, `RoleRank`, `Adder`, `JoinTime`) values(%d, %d, %d, '%s', '%s') "
			"on duplicate key update `RoleRank`=%d, `Adder`='%s', `JoinTime`='%s'",
			roleid,
			roomid,
			rolerank,
			szEscapeOperatorName,
			szDateTime,
			rolerank,
			szEscapeOperatorName,
			szDateTime);

	uint64_t nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "Error: execute update on update rolerank in room failed!\
												roleid=%d, roomid=%d, rolerank=%d, errorcode=0x%08X, sql=%s\n", roleid, roomid, rolerank, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "execute update rolerank  in room success! roleid=%d, roomid=%d, rolerank=%d, sql=%s\n",
	//		roleid, roomid, rolerank, szSql);

	return S_OK;
}

int32_t CFromHallSetEvent::OnMessage_SetSelfInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: hallserver set selfinfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CHallSetSelfInfoReq* pTmpBody = dynamic_cast<CHallSetSelfInfoReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CDBSetSelfSelfInfoResp set_selfinfo;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBHS_SETSELFINFO_RESP);

	//直接回应
	set_selfinfo.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &set_selfinfo, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &set_selfinfo, nOptionLen, (const char*)pOptionData);

	ret = ProcessHallSetSelfInfo(*pTmpBody, set_selfinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process hallserver set selfinfo request failed! errorcode=0x%08X, roleid=%d\n",
				ret, pTmpBody->nRoleID);
		return ret;
	}
	WRITE_DEBUG_LOG( "process hallserver set selfinfo success! \n");

	return S_OK;
}
int32_t CFromHallSetEvent::ProcessHallSetSelfInfo(const CHallSetSelfInfoReq& reqbody, CDBSetSelfSelfInfoResp& respbody)
{
	if(enmInvalidRoleID == reqbody.nRoleID || reqbody.nUserInfoType != enmUserInfoType_Self_Setting)
	{
		WRITE_ERROR_LOG( "roleid invalid on process hallserver set selfinfo! roleid=%d\n", reqbody.nRoleID);
		respbody.nResult = E_INVALID_ARGUMENTS;
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "roleid right on process hallserver set selfinfo! roleid=%d\n", reqbody.nRoleID);

	int32_t ret = S_OK;
	ret = UpdateUserSelfSetting(reqbody.nRoleID, reqbody.nUserInfoSelfSet);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update user self setting failed! errorcode=0x%08X, roleid=%d, user_info_type=%d\n",
				ret, reqbody.nRoleID, reqbody.nUserInfoType);
		respbody.nResult = ret;
		return ret;
	}
	//WRITE_DEBUG_LOG( "update user self setting success! roleid=%d, user_info_type=%d\n",
	//		reqbody.nRoleID, reqbody.nUserInfoType);

	respbody.nResult = S_OK;
	return S_OK;
}
int32_t CFromHallSetEvent::UpdateUserSelfSetting(const RoleID roleid, const uint32_t userinfoSelfSet)
{
	if(roleid == enmInvalidRoleID)
	{
		WRITE_ERROR_LOG( "invalid roleid on update user self setting! roleid=%d\n", roleid);
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "roleid no problem on update user self setting! roleid=%d\n", roleid);

	int32_t ret = S_OK;

	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "insert into vdc_user.`user_config_info`(`RoleID`, `UserInfoSelfSet`) values(%d, %d) on duplicate key update `UserInfoSelfSet`=%d",
			roleid,
			userinfoSelfSet,
			userinfoSelfSet);

	uint64_t nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "error: execute update user self setting failure! errorcode=0x%08X, sql=%s\n",
				ret,
				szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "execute update user self setting success! sql=%s\n", szSql);

	return S_OK;
}

void CFromHallSetEvent::UpdateMemcache(RoleBaseInfo& rolebaseinfo)
{

	//写缓存必须的参数声明
	char szKey[enmMaxMemcacheKeyLen] = {0};
	size_t keylen = 0;
	char szVal[enmMaxMemcacheValueLen] = {0};
	uint32_t offset = 0;

	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();

	//获取key值
	GenerateMemcacheKey(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoleID, rolebaseinfo.nRoleID);

	int32_t ret = CVDCUserBaseInfo::VDCUserBaseInfoEncode((uint8_t*)szVal, enmMaxMemcacheValueLen, offset, rolebaseinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "encode user base info in hall online notice failed! errorcode=0x%08X, roleid=%d\n",
				ret, rolebaseinfo.nRoleID);
		return;
	}

	//从新设置缓存

	ret = cacheobj.MemcacheSet(cacheobj.m_memc, szKey, keylen, szVal, offset, 0);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "memcache set failed in hall online notice! errorcode=0x%08X, roleid=%d\n",
				ret, rolebaseinfo.nRoleID);
		return;
	}

	//清除web缓存,避免不同步
	memset(szKey, 0, sizeof(szKey));
	keylen = 0;
	GenerateMemcacheKeyForWeb(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoleID, rolebaseinfo.nRoleID);
	ret = cacheobj.MemcacheDel(cacheobj.m_memc, szKey, keylen, 0);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "delete memcache information for web failed! errorcode=0x%08X, key=%s, keylen=%d\n",
				ret, szKey, keylen);
		return;
	}
	WRITE_DEBUG_LOG( "delete memcache information for web success! key=%s, keylen=%d\n",szKey, keylen);
}

FRAME_DBPROXY_NAMESPACE_END
