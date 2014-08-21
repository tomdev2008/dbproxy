/*
 * bll_event_taskserver_msg.cpp
 *
 *  Created on: 2013-3-8
 *      Author: liufl
 */

#include "bll_event_taskserver_msg.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

int32_t CFromTaskserverEvent::OnMessage_GetTaskInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: taskserver get taskinfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetTaskInfoReq* pTmpBody = dynamic_cast<CGetTaskInfoReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetTaskInfoResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBTASK_GETTASKINFO_RESP);

	ret = ProcessTaskGetTaskInfoReq(*pTmpBody, respbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process taskserver get taskinfo request failed! errorcode=0x%08X, roleid=%d\n",
				ret, pTmpBody->nRoleID);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process taskserver get taskinfo success! \n");

	respbody.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromTaskserverEvent::ProcessTaskGetTaskInfoReq(const CGetTaskInfoReq& reqbody, CGetTaskInfoResp& respbody)
{

	if(0 >= reqbody.nRoleID)
	{
		WRITE_ERROR_LOG( "invalid roleid! in taskserver get player task info request! roleid=%d\n",
				reqbody.nRoleID);
		return E_INVALID_ARGUMENTS;
	}

	int32_t ret = S_OK;
	respbody.nResult = S_OK;
	ret = QueryRoleCreateTime(reqbody.nRoleID, respbody.nRegisterTime);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query createtime failure! errorcode=0x%08X\n", ret);
		return ret;
	}

	ret = QueryInRoomTime(reqbody.nRoleID, respbody.nRoomTime);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query in room time current day failure! errorcode=0x%08X\n",
				ret);
		return ret;
	}

	ret = GetEmailBindStatus(reqbody.nRoleID, respbody.nEmailStat);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query email bind status failed! errorcode=0x%08X\n", ret);
		return ret;
	}

	ret = QueryTaskInfo(reqbody.nRoleID, respbody.nTaskCount, respbody.arrTaskID, respbody.arrTaskStat, respbody.arrTaskUpdateTime);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "queryr task info failed! errorcode=0x%08X\n", ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query task info success! roleid=%d\n", reqbody.nRoleID);

	ret = QueryModifyName(reqbody.nRoleID, respbody.nNickNameStat);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query if modify name failed! errorcode=0x%08X, roleid=%d\n",
				ret, reqbody.nRoleID);
		return ret;
	}

	ret = QueryIfRecharge(reqbody.nRoleID, respbody.nRechargeStat);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query if recharge failed! errrocdoe=0x%08X, roleid=%d\n",
				ret, reqbody.nRoleID);
		return ret;
	}

	respbody.nResult = S_OK;

	return S_OK;
}
int32_t CFromTaskserverEvent::QueryRoleCreateTime(const RoleID roleid, uint32_t& createtime)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select  unix_timestamp(createtime)  from vdc_user.user_base_info  where  roleid = %d", roleid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query player createtime failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query player createtime success! rowcount=%d, sql=%s\n",nRowCount, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in table user_base_info!, sql=%s\n",szSql);
		createtime = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist in table user_base_info\n");

	createtime = (uint32_t)atoi(arrRecordSet[0]);

	return S_OK;
}
int32_t CFromTaskserverEvent::QueryInRoomTime(const RoleID roleid, uint32_t& inroomtime)
{
	CDateTime dt = CDateTime::CurrentDateTime();
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select  sum(`InRoomTimeTotal`)  from vdc_time.user_room_time_%.4d_%.2d_%.2d  where  roleid = %d",
			dt.Year(),
			dt.Month(),
			dt.Day(),
			roleid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query player in room time current day failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query player in room time current day success! rowcount=%d, sql=%s\n",nRowCount, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in table user room time day!, sql=%s\n",szSql);
		inroomtime = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist in user room time day\n");

	inroomtime = (uint32_t)atoi(arrRecordSet[0]);
	inroomtime = inroomtime * 60;

	return S_OK;
}
int32_t CFromTaskserverEvent::GetEmailBindStatus(const RoleID roleid, EmailStat& emailstatus)
{
	CDateTime dt = CDateTime::CurrentDateTime();
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select  `BindEmail`  from vdc_user.user_base_info  where  roleid = %d",
			roleid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query player bind email failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query player bind email success! rowcount=%d, sql=%s\n",nRowCount, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in table user base info!, sql=%s\n",szSql);
		emailstatus = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist in user base info\n");

	if( 0== strlen(arrRecordSet[0]))
	{
		WRITE_WARNING_LOG( "has not bind email! roleid=%d\n", roleid);
		emailstatus = enmNotCheckType;
		return S_OK;
	}

	emailstatus = enmCheckedType;

	return S_OK;
}
int32_t CFromTaskserverEvent::QueryTaskInfo(const RoleID roleid, int32_t& taskcount, TaskID taskid[], TaskStatType taskstatustype[], uint32_t updatetime[])
{
	char szSql[enmMaxSqlStringLength] = {0};
	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	sprintf(szSql, "select `TaskID`, `TaskStat`, unix_timestamp(`UpdateTime`)  from vdc_user.`player_task_info` where roleid=%d",
			roleid);

	int32_t ret = S_OK;
	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query player task info failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query player task info success!, sql=%s\n", szSql);

	if(0 == nRowCount)
	{
		taskcount = 0;
		return S_OK;
	}

	taskcount = nRowCount;

	for(int32_t i=0; i<taskcount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "db operation on fetch row of player task info failed! errorcode=0x%08X, sql=%s\n", ret,szSql);
			return ret;
		}

		taskid[i] = (TaskID)atoi(arrRecordSet[0]);
		taskstatustype[i] = (TaskStatType)atoi(arrRecordSet[1]);
		updatetime[i] = (uint32_t)atoi(arrRecordSet[2]);
	}

	//WRITE_DEBUG_LOG( "query player task info success!\n");

	return S_OK;
}
int32_t CFromTaskserverEvent::QueryModifyName(const RoleID roleid, NickNameStat& nicknamestatus)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select  `rolenameTimes`  from vdc_user.user_rolename_log  where  roleid = %d",
			roleid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query room channel failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query room channel success! rowcount=%d, sql=%s\n",nRowCount, szSql);

	nicknamestatus = enmNickName_NotAlterType;
	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in table room base info!, sql=%s\n",szSql);
		nicknamestatus = enmNickName_NotAlterType;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist in room base info\n");

	NickNameStat mstatus = enmNickName_NotAlterType;
	mstatus = (NickNameStat)atoi(arrRecordSet[0]);
	if(mstatus > 0)
	{
		WRITE_WARNING_LOG( "rolename already modified! roleid=%d\n",
				roleid);
		nicknamestatus = enmNickName_AlreadyAlterType;
	}

	return S_OK;
}
int32_t CFromTaskserverEvent::QueryIfRecharge(const RoleID roleid, RechargeStat& rechargestatus)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select count(*) from vdc_record.user_each_recharge  where  roleid = %d",
			roleid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query room channel failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query room channel success! rowcount=%d, sql=%s\n",nRowCount, szSql);

	rechargestatus = enmRecharge_NotAlterType;
	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in table room base info!, sql=%s\n",szSql);
		rechargestatus = enmRecharge_NotAlterType;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist in room base info\n");

	RechargeStat mstatus = enmRecharge_NotAlterType;
	mstatus = (RechargeStat)atoi(arrRecordSet[0]);
	if(mstatus > 0)
	{
		WRITE_WARNING_LOG( "rolename already modified! roleid=%d\n",
				roleid);
		rechargestatus = enmRecharge_OK;
	}

	return S_OK;
}

int32_t CFromTaskserverEvent::OnMessage_UpdateAsset(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen , const void *pOptionData)
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
	CTaskUpdateAssetReq* pTmpBody = dynamic_cast<CTaskUpdateAssetReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CTaskUpdateAssetResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_TODB_UPDATEASSET_RESP);
	//直接返回成功
	respbody.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	//限制钱数
	if(pTmpBody->nMoney >= enmMaxPlayerMoney)
	{
		WRITE_ERROR_LOG( "to many money! money=%ld\n",
				pTmpBody->nMoney);
		return E_INVALID_ARGUMENTS;
	}

	uint32_t leftmoney = 0;
	ret = UpdateRoleAsset(pTmpBody->nRoleID, pTmpBody->nUpdateMode, pTmpBody->nMoney, NULL, leftmoney);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update asset failed! errorcode=0x%08X, roleid=%d\n",
				ret, pTmpBody->nRoleID);
		return ret;
	}

	//cache相关组件声明
	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();
	char szWebKey[enmMaxMemcacheKeyLen] = {0};
	size_t nWebKeyLen = 0;
	char szSvrKey[enmMaxMemcacheKeyLen] = {0};
	size_t nSvrKeyLen = 0;
	char szMemcacheVal[enmMaxMemcacheValueLen] = {0};
	size_t nValLen = 0;

	//用户基本信息结构声明
	RoleBaseInfo rolebaseinfo;

//	//删掉web缓存
//	GenerateMemcacheKeyForWeb(szWebKey, enmMaxMemcacheKeyLen, nWebKeyLen, enmStoreType_RoleID, pTmpBody->nRoleID);
//	ret = cacheobj.MemcacheDel(cacheobj.m_memc, szWebKey, nWebKeyLen, 0);
//	if(0 > ret)
//	{
//		WRITE_ERROR_LOG( "delete memcache failed! errorcode=0x%08X, memcachekey=%s, keylen=%d\n",
//				ret,
//				szWebKey,
//				nWebKeyLen);
//		return ret;
//	}
	//WRITE_DEBUG_LOG( "delete memcache success! memcachekey=%s, keylen=%d\n",szWebKey,nWebKeyLen);

	//从数据库中获取该玩家基本信息
	ret = QueryRoleBaseInfo(pTmpBody->nRoleID, rolebaseinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query user base info from db failed! errorcode=0x%08X, roleid=%d\n",
				ret, pTmpBody->nRoleID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query user base info from db success! roleid=%d\n", pTmpBody->nRoleID);

	//将玩家基本信息编码到一个临时缓冲区
	offset = 0;
	ret = CVDCUserBaseInfo::VDCUserBaseInfoEncode((uint8_t*)szMemcacheVal, enmMaxMemcacheValueLen, offset, rolebaseinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "encode user base info into tmpbuf failed! errorcode=0x%08X\n", ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "encode user base info into tmpbuf success!\n");

	//将该缓冲区写入cache
	nValLen = (size_t)offset;
	GenerateMemcacheKey(szSvrKey, enmMaxMemcacheKeyLen, nSvrKeyLen, enmStoreType_RoleID, pTmpBody->nRoleID);
	ret = cacheobj.MemcacheSet(cacheobj.m_memc, szSvrKey, nSvrKeyLen, szMemcacheVal, nValLen, 0);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "write user base info into memcache failed! errorcode=0x%08X, key=%s\n",
				ret, szSvrKey);
		return ret;
	}
	//WRITE_DEBUG_LOG( "write user base info into memcache success on degrade user viplevel! key=%s\n",szSvrKey);

	//账单引擎对象
	CBillEngine& billEngine = GET_BILLENGINE_INSTANCE();
	CDateTime dt = CDateTime::CurrentDateTime();
	char szDatetime[enmMaxDateTimeStringLength] = {0};
	dt.ToDateTimeString(szDatetime);

	//写资产更新账单
	billEngine.WriteUpdateAssetBill(pTmpBody->strTransID.GetString(), pTmpBody->nRoleID, 0, szDatetime, pTmpBody->nMoney, rolebaseinfo.nMoney);

	return S_OK;

}
int32_t CFromTaskserverEvent::UpdateRoleAsset(const RoleID roleid, const UpdateAssetOperate opmod, const uint32_t nMoney, const char* szTransID, uint32_t& nLeftMoney)
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
		WRITE_ERROR_LOG( "db operation of query player money failed! roleid=%d, errorcode=0x%08X, sql=%s\n",
				roleid, ret, szSql);
		return ret;
	}
	nLeftMoney = (uint32_t)atoi(arrRecordSet[0]);
//	WRITE_DEBUG_LOG( "db operation of query player left money success! roleid=%d, nLeftMoney=%u, "
//			"opmode=%d(1:add, 2:decrease, other: invalide), updateAmount=%u, sql=%s\n",
//			roleid, nLeftMoney, opmod, nMoney, szSql);

	//memset(szSql, 0, sizeof(szSql));
	switch(opmod)
	{
		case enmUpdateAssetOperate_Increase:
		if((nLeftMoney + nMoney) > (uint32_t)enmMaxPlayerMoney)
		{
			WRITE_ERROR_LOG( "player left money plus nMoney is more than max player money limit! \
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
			WRITE_ERROR_LOG( "player left money is not enough!\
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
		WRITE_ERROR_LOG( "Error: execute update on role asset failed! roleid=%d, errorcode=0x%08X, transid=%s, sql:%s\n",
				roleid, ret, szTransID, szSql);
		return ret;
	}
//	WRITE_DEBUG_LOG( "execute update on role asset success! roleid=%d, transid=%s, sql=%s\n",
//			roleid, szTransID, szSql);

	if(0 == nAffectedRows)
	{
		WRITE_ERROR_LOG( "Error: execute update role asset sql success but 0 rows affected! transid=%s, roleid=%d, sql=%s\n",
				szTransID, roleid, szSql);
		return E_UPDATE_ZERO_ROW_AFFETCTED;
	}
//	WRITE_DEBUG_LOG( "execute update role asset role asset sql success! szTransID=%s, roleid=%d, sql=%s\n",
//			szTransID, roleid, szSql);

	return S_OK;
}

int32_t CFromTaskserverEvent::OnMessage_UpdateTaskStat(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver get RoleInfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	//	int32_t threadindex =         需要获取线程ID
	CUpdateTaskStat* pTmpBody = dynamic_cast<CUpdateTaskStat*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	//限制任务的个数
	if(pTmpBody->nTaskCount >= MAX_TASK_COUNT)
	{
		WRITE_ERROR_LOG( "to many task count! taskcount=%d\n",
				pTmpBody->nTaskCount);
		return E_INVALID_ARGUMENTS;
	}
	for(int32_t i=0; i<pTmpBody->nTaskCount; ++i)
	{
		ret = UpdateTaskStat(pTmpBody->nRoleID, pTmpBody->arrTaskID[i], pTmpBody->arrTaskStat[i]);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "update task statsitic info failed! errorcode=0x%08X\n",
					ret);
			return ret;
		}
	}

	return S_OK;
}
int32_t CFromTaskserverEvent::UpdateTaskStat(const RoleID roleid, const TaskID taskid, const TaskStatType stattype)
{
	int32_t ret = S_OK;

	CDateTime dt = CDateTime::CurrentDateTime();
	char szDatetime[enmMaxDateTimeStringLength];
	memset(szDatetime, 0, sizeof(szDatetime));
	dt.ToDateTimeString(szDatetime);
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "insert into vdc_user.player_task_info(`RoleID`, `TaskID`, `TaskStat`, `UpdateTime`) "
			"values(%d, %d, %d, '%s') "
			"on duplicate key update `TaskStat`=%d, `UpdateTime`='%s'",
			roleid,
			taskid,
			stattype,
			szDatetime,
			stattype,
			szDatetime);

	uint64_t nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "Error: execute update player task statistic info table failed! roleid=%d, errorcode=0x%08X, sql:%s\n",
				roleid, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "execute role recharge info table success! roleid=%d, sql=%s\n",roleid, szSql);

	return S_OK;
}

FRAME_DBPROXY_NAMESPACE_END
