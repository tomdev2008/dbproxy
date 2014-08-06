/*
 * bll_event_sysinfoset_msg.cpp
 *
 *  Created on: 2013-3-8
 *      Author: liufl
 */

#include "bll_event_sysinfoset_msg.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

int32_t CFromSysinfoSetEvent::OnMessage_ClearVip(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: sysinfo clear vip!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         ��Ҫ��ȡ�߳�ID
	CClearTimeoutVipReq* pTmpBody = dynamic_cast<CClearTimeoutVipReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CClearTimeoutVipResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DSSS_CLEARTIMEOUTVIP_RESP);

	ret = ProcessSysClearTimeOutVip(*pTmpBody, respbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process sysinfo clear vip request failed! errorcode=0x%08X\n",
				ret);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process sysinfo clear vip success! \n");

	respbody.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromSysinfoSetEvent::ProcessSysClearTimeOutVip(const CClearTimeoutVipReq& reqbody, CClearTimeoutVipResp& respbody)
{
	//WRITE_DEBUG_LOG( "now begin to process sysinfo clear timeout vip request!\n");

	int32_t ret = S_OK;

	//enmVipLevel_PURPLE

	//��ѯ���ڵ���ҫvip��roleid
	RoleID arrTimeOutPurpleVipRoleID[UserCountEveryTime];

	//���ڵ���ҫvip������������
	int32_t nTimeOutPurpleVipCount = 0;
	respbody.nResultCount = 0;

	ret = QueryTimeOutPurpleVip(arrTimeOutPurpleVipRoleID, nTimeOutPurpleVipCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query timeout purple vip roleid from db failed! errorcode=0x%08X\n", ret);
		respbody.nResult = ret;
		return ret;
	}
	//WRITE_DEBUG_LOG( "query timeout purple vip roleid succes! count=%d\n", nTimeOutPurpleVipCount);

	//���ݿ����治���������ļ�¼�����ڵ���ҫvip���, ��ֱ�ӷ�����ȷ�����Ҵ�ӡһ��������־
	if(0 == nTimeOutPurpleVipCount)
	{
		WRITE_WARNING_LOG( "no record exist in db of timeout purple vip user!\n");
		respbody.nResultCount = 0;
		respbody.nResult = S_OK;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "there is %d timeout purple vip user in db!\n", nTimeOutPurpleVipCount);

	//������Щ���ڵ���ҫvip�ĵȼ�������Щ�˱�Ϊ����
	for(int32_t i=0; i<nTimeOutPurpleVipCount; ++i)
	{
		ret = UpdateTimeOutPurpleVipLevel(arrTimeOutPurpleVipRoleID[i]);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "update timeout purplevip level failed! errorcode=0x%08X, roleid=%d\n",
					ret, arrTimeOutPurpleVipRoleID[i]);
			respbody.nResult = ret;
			return ret;
		}
		//WRITE_DEBUG_LOG( "update timeout purplevip level success! roleid=%d\n", arrTimeOutPurpleVipRoleID[i]);
	}

	//д�������Ĳ�������
	char szKey[enmMaxMemcacheKeyLen] = {0};
	size_t keylen = 0;
	char szVal[enmMaxMemcacheValueLen] = {0};
	uint32_t offset = 0;

	//��һ�����Ϣ�ṹ
	RoleBaseInfo rolebaseinfo;
	CVDCUserBaseInfo::VDCUserBaseInfoInit(rolebaseinfo);

	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();

	//��ѯ�������Ϊ�������ҵĻ�����Ϣ, Ȼ���д����
	for(int32_t i=0; i<nTimeOutPurpleVipCount; ++i)
	{
		//��ѯ������Ϣ
		ret = QueryRoleBaseInfo(arrTimeOutPurpleVipRoleID[i], rolebaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "query role base info of timeout vip failed! errorcode=0x%08X, roleid=%d\n",
					ret, arrTimeOutPurpleVipRoleID[i]);
			respbody.nResult = ret;
			return ret;
		}
		//WRITE_DEBUG_LOG( "query role base info of timeout vip success! roleid=%d\n", arrTimeOutPurpleVipRoleID[i]);

		//����key
		GenerateMemcacheKey(szKey,enmMaxMemcacheKeyLen, keylen, enmStoreType_RoleID, arrTimeOutPurpleVipRoleID[i]);

		//���»���
		ret = CVDCUserBaseInfo::VDCUserBaseInfoEncode((uint8_t *)szVal, enmMaxMemcacheValueLen, offset, rolebaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "encode role base info failed! roleid=%d, errorcode=0x%08X\n",
					arrTimeOutPurpleVipRoleID[i], ret);
			respbody.nResult = ret;
			return ret;
		}
		//WRITE_DEBUG_LOG( "encode role base info success! roleid=%d\n",arrTimeOutPurpleVipRoleID[i]);

		ret = cacheobj.MemcacheSet(cacheobj.m_memc, szKey, keylen, szVal, (size_t)offset, 0);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "set role base info struct to cache failed! key=%s, keylen=%d, errorcode=0x%08X\n",
					szKey, keylen, ret);
			respbody.nResult = ret;
			return ret;
		}
		//WRITE_DEBUG_LOG( "set role base info struct to cache success! key=%s\n", szKey);

		respbody.nResultCount++;

		//����Ӧ�е��ֶθ�ֵ
		respbody.arrTempVipInfo[i].nRoleID = arrTimeOutPurpleVipRoleID[i];
		respbody.arrTempVipInfo[i].nExpireVipTime = rolebaseinfo.nExpireVipTime;
	}

	respbody.nResult = S_OK;
	return S_OK;
}
int32_t CFromSysinfoSetEvent::QueryTimeOutPurpleVip(RoleID arrRoleID[], int32_t& nTimeOutVipCount)
{
	CDateTime dt = CDateTime::CurrentDateTime();

	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `RoleID` from vdc_user.user_base_info where (`VipLevel`=%d or `VipLevel`=%d) and `ExpireVipTime`>%d and `ExpireVipTime`<%d",
			(int32_t)enmVipLevel_PURPLE, (int32_t)enmVipLevel_RED, 0, (int32_t)dt.Seconds());

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;

	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: db operation on query timeout vip failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query timeout vip success!, sql=%s\n", szSql);

	//��ѯ���ļ�¼������
	nTimeOutVipCount = nRowCount;

	//ȡ�����в�ѯ���ļ�¼��������¼��
	for(int32_t i=0; i<nTimeOutVipCount; ++i)
	{
		//memset(arrRecordSet, 0, sizeof(arrRecordSet));
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "error: db operation of fetch row on query timeout vip failed! errorcode=0x%08X, sql=%s\n",
					ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "db operation of fetch row on query timeout vip success! roleid=%d, sql=%s\n",(RoleID)atoi(arrRecordSet[0]), szSql);

		arrRoleID[i] = (RoleID)atoi(arrRecordSet[0]);
	}

	return S_OK;
}
int32_t CFromSysinfoSetEvent::UpdateTimeOutPurpleVipLevel(const RoleID roleid)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "update vdc_user.user_base_info set `VipLevel`=%d  where `RoleID`=%d", enmVipLevel_REGISTER, roleid);

	uint64_t nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "Error: execute update on role viplevel failed! roleid=%d, errorcode=0x%08X, sql:%s\n",
			roleid, ret, szSql);
		return ret;
	}

	return S_OK;
}


int32_t CFromSysinfoSetEvent::OnMessage_Degrade(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: sysinfo degrade!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         ��Ҫ��ȡ�߳�ID
	CDegradeReq* pTmpBody = dynamic_cast<CDegradeReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CDegradeResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead, MSGID_DSSS_DEGRADE_RESP);

	ret = ProcessSysDegrade(*pTmpBody, respbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process sysinfo degrade request failed! errorcode=0x%08X\n",
				ret);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process sysinfo degrade success! \n");

	respbody.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromSysinfoSetEvent::ProcessSysDegrade(const CDegradeReq& reqbody, CDegradeResp& respbody)
{
	respbody.nRoleID = reqbody.nRoleID;

	int32_t ret = S_OK;

	//�޸����ݿ⣺ user_base_info���е�viplevel�ֶ�
	ret = DegradeUserVipLevel(reqbody.nRoleID, enmVipLevel_REGISTER);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "degrade user vip level failed! errorcode=0x%08X, roleid=%d\n",
				ret,
				reqbody.nRoleID);
		respbody.nResult = ret;
		return ret;
	}
	//WRITE_DEBUG_LOG( "degrade user vip level success! roleid=%d\n", reqbody.nRoleID);

	//cache����������
	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();
	char szWebKey[enmMaxMemcacheKeyLen] = {0};
	size_t nWebKeyLen = 0;
	char szSvrKey[enmMaxMemcacheKeyLen] = {0};
	size_t nSvrKeyLen = 0;
	char szMemcacheVal[enmMaxMemcacheValueLen] = {0};
	size_t nValLen = 0;
	uint32_t offset = 0;

	//�û�������Ϣ�ṹ����
	RoleBaseInfo rolebaseinfo;

	//ɾ��web����: �о������Ƕ��һ�٣���Ϊ����memcache_set��ʱ�����key�Ѿ����ڣ���Ḳ�ǣ���ˣ�����ط�����Ҫɾ��
	GenerateMemcacheKeyForWeb(szWebKey, enmMaxMemcacheKeyLen, nWebKeyLen, enmStoreType_RoleID, reqbody.nRoleID);
	ret = cacheobj.MemcacheDel(cacheobj.m_memc, szWebKey, nWebKeyLen, 0);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "delete memcache failed! errorcode=0x%08X, memcachekey=%s, keylen=%d\n",
				ret,
				szWebKey,
				nWebKeyLen);
		return ret;
	}
	//WRITE_DEBUG_LOG( "delete memcache success! memcachekey=%s, keylen=%d\n",szWebKey,nWebKeyLen);

	//�����ݿ��л�ȡ����һ�����Ϣ
	ret = QueryRoleBaseInfo(reqbody.nRoleID, rolebaseinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query user base info from db failed! errorcode=0x%08X, roleid=%d\n",
				ret, reqbody.nRoleID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query user base info from db success! roleid=%d\n", reqbody.nRoleID);

	//����һ�����Ϣ���뵽һ����ʱ������
	ret = CVDCUserBaseInfo::VDCUserBaseInfoEncode((uint8_t*)szMemcacheVal, enmMaxMemcacheValueLen, offset, rolebaseinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "encode user base info into tmpbuf failed! errorcode=0x%08X\n", ret);
		respbody.nResult = ret;
		return ret;
	}
	//WRITE_DEBUG_LOG( "encode user base info into tmpbuf success!\n");

	//���û�����д��cache
	nValLen = (size_t)offset;
	GenerateMemcacheKey(szSvrKey, enmMaxMemcacheKeyLen, nSvrKeyLen, enmStoreType_RoleID, reqbody.nRoleID);
	ret = cacheobj.MemcacheSet(cacheobj.m_memc, szSvrKey, nSvrKeyLen, szMemcacheVal, nValLen, 0);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "write user base info into memcache failed! errorcode=0x%08X, key=%s\n", ret, szSvrKey);
		respbody.nResult = ret;
		return ret;
	}
	//WRITE_DEBUG_LOG( "write user base info into memcache success on degrade user viplevel! key=%s\n", szSvrKey);

	respbody.nResult = S_OK;
	return S_OK;
}
int32_t CFromSysinfoSetEvent:: DegradeUserVipLevel(const RoleID roleid, const VipLevel cur_viplevel)
{
	int32_t ret = S_OK;

	CDateTime dt = CDateTime::CurrentDateTime();
	char szDateTime[enmMaxTimeStringLength] = {0};
	dt.ToDateTimeString(szDateTime);

	char szYearMonth[enmMaxTimeStringLength] = {0};
	sprintf(szYearMonth, "%04d%02d", dt.Year(), dt.Month());
	int32_t nYearMonth = 0;
	nYearMonth = (int32_t)atoi(szYearMonth);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	uint64_t nAffectedRows = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `YearMonth`, `Degrade` from vdc_user.`user_vipdegrade_record` where `RoleID`=%d", roleid);

	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query vipdegrade record failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query vipdegrade record success! sql=%s\n", szSql);

	//�����ѯ���Ľ��YearMonth>=��ǰ���� ����  Degrade=1�� ��˵���������������Ѿ������������ٽ�������ʱֱ�ӷ��ؼ���
	if((nYearMonth <= (int32_t)atoi(arrRecordSet[0])) && (1 == (int32_t)atoi(arrRecordSet[1])))
	{
		WRITE_WARNING_LOG( "this user viplevel has been degraded this month! roleid=%d\n", roleid);
		return S_ALREADY_DEGRADE;
	}
	//WRITE_DEBUG_LOG( "this user viplevel has not degrade this month, we will degrade it! roleid=%d\n", roleid);

	//�򽵼�������д��һ����¼������Ѿ��и���ҵĽ�����¼������¸ü�¼��
	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "insert into vdc_user.`user_vipdegrade_record`(`RoleID`, `YearMonth`, `Degrade`, `DegradeTime`) values(%d, %d, %d, '%s') on duplicate key update `YearMonth`=%d, `Degrade`=%d, `DegradeTime`='%s'",
			roleid,
			nYearMonth,
			1,
			szDateTime,
			nYearMonth,
			1,
			szDateTime);
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "insert degrade viplevel failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "insert degrade viplevel success! roleid=%d, sql=%s\n", roleid, szSql);

	//���֮ǰ�Ľ�����¼�����±ȵ�ǰ����С���ߵ�ǰ������δ�������򽵼�
	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "update vdc_user.user_base_info set `VipLevel`=%d  where `RoleID`=%d", cur_viplevel, roleid);

	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "Error: execute update on degrade user viplevel failed! roleid=%d, errorcode=0x%08X, sql=%s\n", roleid, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "execute update on degrade user viplevel success! roleid=%d, sql=%s\n", roleid, szSql);

	return S_OK;
}

int32_t CFromSysinfoSetEvent::OnMessage_GroupDegrade(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: sysinfo groupdegrade!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         ��Ҫ��ȡ�߳�ID
	CGroupDegradeReq* pTmpBody = dynamic_cast<CGroupDegradeReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGroupDegradeResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead, MSGID_DSSS_GROUPDEGRADE_RESP);

	ret = ProcessSysGroupDegrade(*pTmpBody, respbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process sysinfo groupdegrade request failed! errorcode=0x%08X\n",
				ret);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process sysinfo groupdegrade success! \n");

	respbody.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromSysinfoSetEvent::ProcessSysGroupDegrade(const CGroupDegradeReq& reqbody, CGroupDegradeResp& respbody)
{
	if(0 > reqbody.nCount || MaxDegradeCountEveryTime < reqbody.nCount)
	{
		WRITE_ERROR_LOG( "role count invalid while process group degrade request! rolecount=%d\n", reqbody.nCount);
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "rolecount right while process group degrade request! role count=%d\n", reqbody.nCount);

	int32_t ret = S_OK;

	RoleBaseInfo rolebaseinfo;

	//д������ر�������
	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();
	char szSvrKey[enmMaxMemcacheKeyLen] = {0};
	size_t nSvrKeyLen = 0;
	char szWebKey[enmMaxMemcacheKeyLen] = {0};
	size_t nWebKeyLen = 0;
	char szValBuf[enmMaxMemcacheValueLen] = {0};
	size_t nValLen = 0;
	uint32_t offset = 0;

	//����ʧ�ܵĸ�����ͳ��
	int32_t nFailedCount = 0;

	//�Ѿ��������������ٽ����ĸ���ͳ��
	int32_t nAlreadyDegrade = 0;

	//�����ɹ���Ҽ�����
	respbody.nCount = 0;
	for(int32_t i=0; i<reqbody.nCount; ++i)
	{
		//WRITE_DEBUG_LOG( "begin to degrade user vip level! roleid=%d\n",reqbody.arrRoleID[i]);

		//�޸����ݿ��и���ҵ�vip�ȼ�
		ret = DegradeUserVipLevel(reqbody.arrRoleID[i], reqbody.arrCurVipLevel[i]);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "degrade this user viplevel failed! roleid=%d\n",
					reqbody.arrRoleID[i]);
			nFailedCount ++;
			continue;
		}
		//WRITE_DEBUG_LOG( "degrade this user viplevel success! roleid=%d\n",reqbody.arrRoleID[i]);

		//�������ֵ����0�� ˵������ҵ����Ѿ��������ˣ� �������д���ֱ��continue
		if(ret > 0)
		{
			WRITE_WARNING_LOG( "this user viplevel has already degrade this month! roleid=%d, ret=0x%08X\n",
					reqbody.arrRoleID[i], ret);
			nAlreadyDegrade ++;
			continue;
		}
		//WRITE_DEBUG_LOG( "degrade user viplevel success! roleid=%d\n",reqbody.arrRoleID[i]);

		//������Ҵӻ�����ɾ����web��ʽ�Ļ��棩
		memset(szWebKey, 0, sizeof(szWebKey));
		GenerateMemcacheKeyForWeb(szWebKey, enmMaxMemcacheKeyLen, nWebKeyLen, enmStoreType_RoleID, reqbody.arrRoleID[i]);
		ret = cacheobj.MemcacheDel(cacheobj.m_memc, szWebKey, nWebKeyLen, 0);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "delete web memcache failed! webkey=%s, webkeylen=%d, errorcode=0x%08X\n",
					szWebKey, nWebKeyLen, ret);
			nFailedCount ++;
			continue;
		}
		//WRITE_DEBUG_LOG( "delete web memcache success! webkey=%s, webkeylen=%d\n",szWebKey,nWebKeyLen);

		//�����ݿ��в�ѯ����ҵĻ�����Ϣ
		ret = QueryRoleBaseInfo(reqbody.arrRoleID[i], rolebaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "query user base info from db failed! roleid=%d, errorcode=0x%08X\n",
					reqbody.arrRoleID[i], ret);
			nFailedCount ++;
			continue;
		}
		//WRITE_DEBUG_LOG( "query user base info from db success! roleid=%d, viplevel=%d\n",reqbody.arrRoleID[i], rolebaseinfo.ucVipLevel);

		//���������Ϣ���룬׼��д����
		memset(szValBuf, 0, sizeof(szValBuf));
		offset = 0;
		ret = CVDCUserBaseInfo::VDCUserBaseInfoEncode((uint8_t*)szValBuf, enmMaxMemcacheValueLen, offset, rolebaseinfo);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "enocode user base info into valbuf failed! roleid=%d, errorcode=0x%08X\n",
					reqbody.arrRoleID[i], ret);
			nFailedCount ++;
			continue;
		}
		nValLen = offset;
		//WRITE_DEBUG_LOG( "encode user base info into valbuf success! roleid=%d\n", reqbody.arrRoleID[i]);

		//д����
		memset(szSvrKey, 0, sizeof(szSvrKey));
		GenerateMemcacheKey(szSvrKey, enmMaxMemcacheKeyLen, nSvrKeyLen, enmStoreType_RoleID, reqbody.arrRoleID[i]);
		ret = cacheobj.MemcacheSet(cacheobj.m_memc, szSvrKey, nSvrKeyLen, szValBuf, nValLen, 0);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "write user base info into memcache failed! szKey=%s, keylen=%d, errorcode=0x%08X\n",
					szSvrKey, nSvrKeyLen, ret);
			nFailedCount ++;
			continue;
		}
		//WRITE_DEBUG_LOG( "write user base info into memcache success! szKey=%s, keylen=%d\n",szSvrKey, nSvrKeyLen);

		respbody.arrRoleID[respbody.nCount] = reqbody.arrRoleID[i];
		respbody.arrVipLevel[respbody.nCount] = rolebaseinfo.ucVipLevel;
		respbody.nCount++;
	}

	if(nFailedCount >= (int32_t)reqbody.nCount)
	{
		WRITE_ERROR_LOG( "group degrade viplevel with no one success! all failed! failedcount=%d\n", nFailedCount);
		respbody.nResult = E_GROUP_DEGRADE_VIPLEVEL;
		return E_GROUP_DEGRADE_VIPLEVEL;
	}
	//WRITE_DEBUG_LOG( "group degrade viplevel success! failedcount=%d\n", nFailedCount);

	respbody.nResult = S_OK;
	//WRITE_DEBUG_LOG( "group degrade user viplevel  success! already degrade count=%d, successcount=%d\n",nAlreadyDegrade, respbody.nCount);

	return S_OK;
}

int32_t CFromSysinfoSetEvent::OnMessage_NewMsg(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: sysinfo newMsg!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         ��Ҫ��ȡ�߳�ID
	CNewMessageReq* pTmpBody = dynamic_cast<CNewMessageReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CNewMessageResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead, MSGID_DSSS_NEWMSG_RESP);

	ret = ProcessSysNewMsg(*pTmpBody, respbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process sysinfo newMsg request failed! errorcode=0x%08X\n",
				ret);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process sysinfo newMsg success! \n");

	respbody.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromSysinfoSetEvent::ProcessSysNewMsg(const CNewMessageReq& reqbody, CNewMessageResp& respbody)
{
	respbody.nRoleID = reqbody.nRoleID;

	if(enmInvalidRoleID == reqbody.nRoleID)
	{
		WRITE_WARNING_LOG( "invalid roleid in sysinfoserver new message request! roleid=%d\n",
				reqbody.nRoleID);
		respbody.nResult = S_OK;
		return S_OK;
	}

	if(reqbody.nChannelType == enumSysChannelType_Invalid)
	{
		WRITE_WARNING_LOG( "invalid channeltype on sysinfo newmsg request! channeltype=%d\n",
				reqbody.nChannelType);
		respbody.nResult = S_OK;
		return S_OK;
	}

	int32_t ret = S_OK;
	ret = InsertNewMessage(reqbody.nRoleID, reqbody.nChannelType, reqbody.szMsgContent.GetString(), strlen(reqbody.szMsgContent.GetString()));
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "insert new message failed! roleid=%d, channeltype=%d, errorcode=0x%08X\n",
				reqbody.nRoleID,
				reqbody.nChannelType,
				ret);
		respbody.nResult = ret;
		return ret;
	}
	//WRITE_DEBUG_LOG( "insert new message success! roleid=%d, channeltype=%d\n",reqbody.nRoleID, reqbody.nChannelType);

	respbody.nResult = S_OK;
	return S_OK;
}
int32_t CFromSysinfoSetEvent::InsertNewMessage(const RoleID roleid, const ChannelType channeltype, const char* szContent, const int32_t nContentLen)
{
	if(roleid == enmInvalidRoleID || channeltype == enumSysChannelType_Invalid)
	{
		WRITE_ERROR_LOG( "invalid argument on insert new message! roleid=%d, channeltype=%d\n", roleid, channeltype);
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "begin to write systeminfo to user_message table! roleid=%d, channeltype=%d\n", roleid, channeltype);

	//����SQL���
	char szSql[enmMaxSqlStringLength] = {0};

	CDateTime dt = CDateTime::CurrentDateTime();

	sprintf(szSql, "insert into vdc_user.user_message(`msg_owner`, `msg_channel_type`, `msg_content`, `msg_inserttime`) values (%d, %d, '%s', %d)" ,
			roleid, channeltype, szContent, (int32_t)dt.Seconds());

	uint64_t nAffectedRows = 0;
	int32_t nRet = MYSQLREADENGINE.ExecuteInsert(szSql, nAffectedRows);
	if (0 > nRet)
	{
		WRITE_ERROR_LOG( "db operation on insert user message failed! errorcode=0x%08X, sql=%s\n",
				nRet, szSql);
		return nRet;
	}
	//WRITE_DEBUG_LOG( "db operation on insert user message success! sql=%s\n", szSql);

	//memset(szSql, 0, sizeof(szSql));
	nAffectedRows = 0;

	switch(channeltype)
	{
		case enumSysChannelType_Upgrade:
		sprintf(szSql, "insert into vdc_user.user_message_statistic(`msg_owner`, `log_total_vipup`, `log_new_vipup`) values(%d, `log_total_vipup`+1, `log_new_vipup`+1) \
					   on duplicate key update `log_total_vipup`=`log_total_vipup`+1, `log_new_vipup`=`log_new_vipup`+1",
				roleid);
		break;
		case enumSysChannelType_Degrade:
		sprintf(szSql, "insert into vdc_user.user_message_statistic(`msg_owner`, `log_total_vipdegrade`, `log_new_vipdegrade`) values(%d, `log_total_vipdegrade`+1, `log_new_vipdegrade`+1) \
					   on duplicate key update `log_total_vipdegrade`=`log_total_vipdegrade`+1, `log_new_vipdegrade`=`log_new_vipdegrade`+1",
				roleid);
		break;
		case enumSysChannelType_Ranklist:
		sprintf(szSql, "insert into vdc_user.user_message_statistic(`msg_owner`, `log_total_rank`, `log_new_rank`) values(%d, `log_total_rank`+1, `log_new_rank`+1) \
					   on duplicate key update `log_total_rank`=`log_total_rank`+1, `log_new_rank`=`log_new_rank`+1",
				roleid);
		break;
		case enumSysChannelType_Room:
		sprintf(szSql, "insert into vdc_user.user_message_statistic(`msg_owner`, `log_total_roomrelated`, `log_new_roomrelated`) values(%d, `log_total_roomrelated`+1, `log_new_roomrelated`+1) \
					   on duplicate key update `log_total_roomrelated`=`log_total_roomrelated`+1, `log_new_roomrelated`=`log_new_roomrelated`+1",
				roleid);
		break;
		case enumSysChannelType_Charge:
		sprintf(szSql, "insert into vdc_user.user_message_statistic(`msg_owner`, `log_total_charge`, `log_new_charge`) values(%d, `log_total_charge`+1, `log_new_charge`+1) \
					   on duplicate key update `log_total_charge`=`log_total_charge`+1, `log_new_charge`=`log_new_charge`+1", roleid);
		break;
		default:
		WRITE_WARNING_LOG( "unknown channel type on insert new message! channeltype=%d\n", channeltype);
		return S_OK;
	}

	nRet = MYSQLREADENGINE.ExecuteInsert(szSql, nAffectedRows);
	if(0 > nRet)
	{
		WRITE_ERROR_LOG( "insert user message statistics failed! errorcode=0x%08X, sql=%s\n", nRet, szSql);
		return nRet;
	}
	//WRITE_DEBUG_LOG( "insert user message statistics success! sql=%s\n", szSql);

	return S_OK;
}

int32_t CFromSysinfoSetEvent::OnMessage_WeekRecharge(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: sysinfo week recharge!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         ��Ҫ��ȡ�߳�ID
	CGetWeekSupplymentReq* pTmpBody = dynamic_cast<CGetWeekSupplymentReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetWeekSupplymentResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead, MSGID_DSSS_WEEKSYPPLYMENT_RESP);

	ret = QueryWeekRecharge(pTmpBody->nRoleID, pTmpBody->nWeekIndex, respbody.nSupplymentAmount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process sysinfo week recharge request failed! errorcode=0x%08X\n",
				ret);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process sysinfo week recharge success! \n");

	respbody.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromSysinfoSetEvent::QueryWeekRecharge(const RoleID roleid, const uint32_t weekindex, uint32_t& amount)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `TotalRechargeAmount` from vdc_record.user_week_recharge where `RoleID`=%d and `Week`=%d",
			roleid, weekindex);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query week recharge amount endtime failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in table user_week_recharge!, sql=%s\n",szSql);
		amount = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist in table mac_lock\n");

	amount = (uint32_t)atoi(arrRecordSet[0]);

	return S_OK;
}

FRAME_DBPROXY_NAMESPACE_END
