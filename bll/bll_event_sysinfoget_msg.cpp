/*
 * bll_event_sysinfoget_msg.cpp
 *
 *  Created on: 2013-3-7
 *      Author: liufl
 */

#include "bll_event_sysinfoget_msg.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

int32_t CFromSysinfoGetEvent::OnMessage_getmsgcount(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: sysinfoserver get msgcount!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetMessageCountReq* pTmpBody = dynamic_cast<CGetMessageCountReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetMessageCountResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DSSS_GETMSGCNT_RESP);

	ret = ProcessSysGetMsgCount(*pTmpBody, respbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process rsysinfoserver get msgcount request failed! errorcode=0x%08X, roleid=%d\n",
				ret, pTmpBody->nRoleID);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process sysinfoserver get msgcount success! \n");

	respbody.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromSysinfoGetEvent::ProcessSysGetMsgCount(const CGetMessageCountReq& reqbody, CGetMessageCountResp& respbody)
{
	respbody.nRoleID = reqbody.nRoleID;
	respbody.nChannelType = reqbody.nChannelType;
	respbody.nReaded = reqbody.nReaded;

	if(reqbody.nRoleID == enmInvalidRoleID || reqbody.nChannelType == enumSysChannelType_Invalid)
	{
		WRITE_ERROR_LOG( "invalid argument on sysinfo get msgcount request! roleid=%d, channeltype=%d\n",
				reqbody.nRoleID, reqbody.nChannelType);
		respbody.nResult = E_INVALID_ARGUMENTS;
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "begin to process sysinfo get msgcount request! nRoleID=%d, nChannelType=%d, reqbody.nReaded=%d\n",
//			reqbody.nRoleID,
//			reqbody.nChannelType,
//			reqbody.nReaded);

	int32_t ret = S_OK;
	ret = QueryRoleMsgCount(reqbody.nRoleID, reqbody.nChannelType, reqbody.nReaded, respbody.nMsgCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query role msgcount from db failed! roleid=%d, channeltype=%d, readed=%d, errorcode=0x%08X\n",
				reqbody.nRoleID,
				reqbody.nChannelType,
				reqbody.nReaded,
				ret);
		respbody.nResult = ret;
		return ret;
	}
//	WRITE_DEBUG_LOG( "query role msgcount from db success! roleid=%d, channeltype=%d, readed=%d, msgcount=%d\n",
//			reqbody.nRoleID,
//			reqbody.nChannelType,
//			reqbody.nReaded,
//			respbody.nMsgCount);

	respbody.nResult = S_OK;
	return S_OK;
}
int32_t CFromSysinfoGetEvent::QueryRoleMsgCount(const RoleID roleid, const ChannelType channeltype, const uint8_t readed, uint32_t& msgcount)
{
	char szSql[enmMaxSqlStringLength] = {0};
	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	switch(channeltype)
	{
		case enumSysChannelType_Upgrade:
		switch(readed)
		{
			case 0:
			sprintf(szSql, "select `log_new_vipup` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			case 1:
			sprintf(szSql, "select `log_total_vipup`-`log_new_vipup` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			case 2:
			sprintf(szSql, "select `log_total_vipup` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			default:
			WRITE_WARNING_LOG( "unkown readed type! readed=%d\n", readed);
		}
		break;
		case enumSysChannelType_Degrade:
		switch(readed)
		{
			case 0:
			sprintf(szSql, "select `log_new_vipdegrade` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			case 1:
			sprintf(szSql, "select `log_total_vipdegrade` - `log_new_vipdegrade` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			case 2:
			sprintf(szSql, "select `log_total_vipdegrade` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			default:
			WRITE_WARNING_LOG( "unkown readed type! readed=%d\n", readed);
		}
		break;
		case enumSysChannelType_Ranklist:
		switch(readed)
		{
			case 0:
			sprintf(szSql, "select `log_new_rank` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			case 1:
			sprintf(szSql, "select `log_total_rank`-`log_new_rank` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			case 2:
			sprintf(szSql, "select `log_total_rank` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			default:
			WRITE_WARNING_LOG( "unkown readed type! readed=%d\n", readed);
		}
		break;
		case enumSysChannelType_Room:
		switch(readed)
		{
			case 0:
			sprintf(szSql, "select `log_new_roomrelated` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			case 1:
			sprintf(szSql, "select `log_total_roomrelated`-`log_new_roomrelated` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			case 2:
			sprintf(szSql, "select `log_total_roomrelated` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			default:
			WRITE_WARNING_LOG( "unkown readed type! readed=%d\n", readed);
		}
		break;
		case enumSysChannelType_Charge:
		switch(readed)
		{
			case 0:
			sprintf(szSql, "select `log_new_charge` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			case 1:
			sprintf(szSql, "select `log_total_charge`-`log_new_charge` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			case 2:
			sprintf(szSql, "select `log_total_charge` from vdc_user.user_message_statistic where msg_owner=%d", roleid);
			break;
			default:
			WRITE_WARNING_LOG( "unkown readed type! readed=%d\n", readed);
		}
		break;
		default:
		WRITE_WARNING_LOG( "unknown channel type! channeltype=%d\n", channeltype);
		return S_OK;
	}

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query user system msgcount failed! errorcode=0x%08X, sql=%s\n",
				ret,
				szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query user system msgcount success! sql=%s\n", szSql);

	if(0 == nRowCount)
	{
		msgcount = 0;
		WRITE_WARNING_LOG( "warning: no such record in db! roleid=%d, channeltype=%d, readed=%d\n",
				roleid,
				channeltype,
				readed);
		return S_OK;
	}
	msgcount = atoi(arrRecordSet[0]);
	//WRITE_DEBUG_LOG( "db operation on query user system info count success! roleid=%d, channeletype=%d, readed=%d, sql=%s\n",
//			roleid,
//			channeltype,
//			readed,
//			szSql);

	return S_OK;
}

int32_t CFromSysinfoGetEvent::OnMessage_getnoti(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: sysinfoserver get noti!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	//CGetNoticeReq* pTmpBody = dynamic_cast<CGetNoticeReq*>(pMsgBody);
	CGetNoticeResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DSSS_GETNOTICE_RESP);

	ret = QuerySystemNotice(respbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process sysinfoserver get notirequest failed! errorcode=0x%08X\n",
				ret);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process sysinfoserver get noti success! \n");

	respbody.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromSysinfoGetEvent::QuerySystemNotice(CGetNoticeResp& respbody)
{
	CDateTime dt = CDateTime::CurrentDateTime();

	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select  `id`, `title`, `content`, `type`, `begintime`, `endtime`, `flag`, `LoopTime`, `SendFlag` from vdc_sysnotice.`sysnotice`  \
				   where `begintime`<%d and `endtime`>%d  order by  `addtime` limit %d, %d",
			(int32_t)dt.Seconds(), (int32_t)dt.Seconds(), 0, (int32_t)MaxNoticeCount);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));
	uint64_t nAffectedRows = 0;

	int32_t ret = S_OK;
	respbody.nResultCount = 0;

	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query system notice failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query system notice success!, sql=%s\n", szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "query system notice success! but no record exists!\n");
		respbody.nResultCount = 0;
		return S_OK;
	}

	for(uint32_t i=0; i<nRowCount; ++i)
	{
		//memset(arrRecordSet, 0, sizeof(arrRecordSet));
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "fetch systemnotice from arrRecordSet failed! errorcode=0x%08X, sql=%s\n",
					ret, szSql);
			continue;
		}
		//WRITE_DEBUG_LOG( "fetch row of systemnotice from arrRecordSet success!, sql=%s\n", szSql);
		//`id`, `title`, `content`, `type`, `begintime`, `endtime`
		respbody.arrSysNotice[respbody.nResultCount].nID = (int32_t)atoi(arrRecordSet[0]);
		respbody.arrSysNotice[respbody.nResultCount].strTitle = arrRecordSet[1];
		respbody.arrSysNotice[respbody.nResultCount].strContent = arrRecordSet[2];
		respbody.arrSysNotice[respbody.nResultCount].nType = (int8_t)atoi(arrRecordSet[3]);
		respbody.arrSysNotice[respbody.nResultCount].nStartTime = (int64_t)atoi(arrRecordSet[4]);
		respbody.arrSysNotice[respbody.nResultCount].nEndTime = (int64_t)atoi(arrRecordSet[5]);
		respbody.arrSysNotice[respbody.nResultCount].nFlag = (int8_t)atoi(arrRecordSet[6]);
		respbody.arrSysNotice[respbody.nResultCount].nLoopTime = (int32_t)atoi(arrRecordSet[7]);
		respbody.arrSysNotice[respbody.nResultCount].nSendFlag = (int8_t)atoi(arrRecordSet[8]);

		//如果是否已读标志字段等于0，则将其置1
		if((int8_t)atoi(arrRecordSet[6]) == 0)
		{
			//memset(szSql, 0, sizeof(szSql));
			sprintf(szSql, "update vdc_sysnotice.`sysnotice` set `flag`=%d where `id`=%d",
					(int32_t)enmSysNoticeFlag_Readed, respbody.arrSysNotice[respbody.nResultCount].nID);
			ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "update systemnotice read flag failed! errorcode=0x%08X, sql=%s\n",
						ret, szSql);
				return ret;
			}
			//WRITE_DEBUG_LOG( "update systemnotice read flag success!, sql=%s\n", szSql);
		}

		respbody.nResultCount++;
	}

	if(0 == respbody.nResultCount)
	{
		WRITE_ERROR_LOG( "unknown error happened while query system notice!, sql=%s\n", szSql);
		return E_UNKNOWN;
	}
	//WRITE_DEBUG_LOG( "query system notice success! notice count=%d, sql=%s\n",respbody.nResultCount, szSql);

	return S_OK;
}

int32_t CFromSysinfoGetEvent::OnMessage_getNotiVersion(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: sysinfoserver get version!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	//CGetNoticeVersionReq* pTmpBody = dynamic_cast<CGetNoticeVersionReq*>(pMsgBody);
	CGetNoticeVersionResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DSSS_GETNOTICEVER_RESP);

	ret = QueryNoticeVersion(respbody.nVersion);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process sysinfoserver get version request failed! errorcode=0x%08X\n",
				ret);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	//WRITE_DEBUG_LOG( "process sysinfoserver get version success! \n");

	respbody.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromSysinfoGetEvent::QueryNoticeVersion(int32_t& sysinfo_version)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select  `sysnotice_version`  from vdc_sysnotice.`sysnotice_version`");

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query notice version failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query notice version success! rowcount=%d, sql=%s\n", nRowCount, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "no such record in db of sysinfo noticeversion!sql=%s\n",szSql);
		sysinfo_version = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "record exist of sysinfo noticeversion\n");

	sysinfo_version = (int32_t)atoi(arrRecordSet[0]);

	return S_OK;
}

int32_t CFromSysinfoGetEvent::OnMessage_getRankInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: sysinfoserver get rankinfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	//CRankInfoReq* pTmpBody = dynamic_cast<CRankInfoReq*>(pMsgBody);
	CRankInfoResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DSSS_RANK_RESP);

	ret = QueryRankInfo(respbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process sysinfoserver get rankinfo request failed! errorcode=0x%08X\n",
				ret);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process sysinfoserver get rankinfo success! \n");

	respbody.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromSysinfoGetEvent::QueryRankInfo(CRankInfoResp& respbody)
{
	char szSql[enmMaxSqlStringLength] = {0};
	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t nLastWeek = 0;
	GetLastWeek(nLastWeek);

	if(nLastWeek == 0)
	{
		return E_GET_DATEWEEK;
	}
	respbody.nRankListCount = 0;
	respbody.nGiftStarCount = 0;

	//select `RoleID`, `ItemID`, `ItemCount`, dateline from vdc_server_test.prop_obtain_week_notice where `Week`=201236 group by `ItemID`, itemcount, roleid order by `ItemCount` desc, dateline asc;

	sprintf(szSql, "select `RoleID`, sum(ItemCount) from vdc_stats.`prop_used_week` where `Week`=%d group by RoleID order by sum(ItemCount) desc, `WriteTime` asc limit %d, %d",
			nLastWeek, 0, 3);

	int32_t ret = S_OK;
	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query stamper rank info failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query stamper rank info success!, sql=%s\n", szSql);

	for(uint32_t i=0; i<nRowCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "db operation on fetch row of user stamper rank info failed! errorcode=0x%08X, sql=%s\n", ret,szSql);
			return ret;
		}

		//如果结果记录集为空，则说明没有数据，直接退出
		if(NULL == arrRecordSet[0])
		{
			break;
		}

		//如果roleid为非法值，则直接退出
		if(0 >= atoi(arrRecordSet[0]))
		{
			WRITE_WARNING_LOG( "be careful: invalid roleid! so just ignore it!\n");
			break;
		}

		//如果结果记录集不为空，则开始赋值
		respbody.arrayRankListInfo[respbody.nRankListCount].nRoleID = (RoleID)atoi(arrRecordSet[0]);
		respbody.arrayRankListInfo[respbody.nRankListCount].nRankListType = enmRankType_StampRankList;
		respbody.arrayRankListInfo[respbody.nRankListCount].nPlace = i + 1;
//		WRITE_DEBUG_LOG( "db operation on fetch row of user stamper rank info success! roleid=%d, ranklisttype=%d, rank=%d, sql=%s\n",
//				respbody.arrayRankListInfo[respbody.nRankListCount].nRoleID,
//				respbody.arrayRankListInfo[respbody.nRankListCount].nRankListType,
//				respbody.arrayRankListInfo[respbody.nRankListCount].nPlace,
//				szSql);
		respbody.nRankListCount++;
	}

	//查询用户消费排行(也就是富豪榜)
	//memset(szSql, 0, sizeof(szSql));
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));
	sprintf(szSql, "select `RoleID` from vdc_stats.`user_consume` where `Week`=%d order by `Consume` desc, `WriteTime` asc limit %d, %d",
			nLastWeek, 0, 3);
//	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);

	MYSQLREADENGINE.ExecuteQueryEnd();
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query user consume record failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query user consume record success! sql=%s\n", szSql);

	for(uint32_t i=0; i<nRowCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "db operation on fetch row of user stamper rank info failed! errorcode=0x%08X\n", ret);
			return ret;
		}

		if(NULL == arrRecordSet[0])
		{
			WRITE_NOTICE_LOG( "be careful, no record of consume rank exist in last week! lastweek=%d\n", nLastWeek);
			break;
		}

		if(0 >= atoi(arrRecordSet[0]))
		{
			WRITE_WARNING_LOG( "be careful! invalid roleid occured! just ignore and quit loop!\n");
			break;
		}

		respbody.arrayRankListInfo[respbody.nRankListCount].nRoleID = (RoleID)atoi(arrRecordSet[0]);
		respbody.arrayRankListInfo[respbody.nRankListCount].nRankListType = enmRankType_WealthRankList;
		respbody.arrayRankListInfo[respbody.nRankListCount].nPlace = i+1;
//		WRITE_DEBUG_LOG( "db operation on query user consume record success! roleid=%d, ranklisttype=%d, rank=%d, sql=%s\n",
//				respbody.arrayRankListInfo[respbody.nRankListCount].nRoleID,
//				respbody.arrayRankListInfo[respbody.nRankListCount].nRankListType,
//				respbody.arrayRankListInfo[respbody.nRankListCount].nPlace,
//				szSql);
		respbody.nRankListCount++;
	}

	//房间综合排行（人气、消费各一半）
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));
	//memset(szSql, 0, sizeof(szSql));
	RoomID tempRoomID = 0;
	RoleID tempRoomHostUin = 0;
	sprintf(szSql, "select `RoomID` from vdc_stats.`room_online_consume_week` where `Week`=%d order by `Value` desc limit %d, %d",
			nLastWeek, 0, 3);
//	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	MYSQLREADENGINE.ExecuteQueryEnd();
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query room online_consume failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query room online_consume success! sql=%s\n", szSql);

	for(uint32_t i=0; i<nRowCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "fetch row of get room rank info failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "fetch row of get room rank info success! sql=%s\n", szSql);

		if(NULL == arrRecordSet[0])
		{
			WRITE_NOTICE_LOG( "be careful! no record of roomrank exist in last week! week=%d\n", nLastWeek);
			break;
		}

		tempRoomID = (RoomID)atoi(arrRecordSet[0]);
		if(0 >= tempRoomID)
		{
			WRITE_WARNING_LOG( "be careful! invalid roomid occured!\n");
			break;
		}

		//查询该房间的室主roleid
		ret = QueryRoomHostRoleID(tempRoomID, tempRoomHostUin);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "query room host roleid failed! errorcode=0x%08X, roomid=%d\n", ret, tempRoomID);
			return ret;
		}
		//WRITE_DEBUG_LOG( "query room host roleid success! roomid=%d, roomhost roleid=%d\n", tempRoomID, tempRoomHostUin);

		respbody.arrayRankListInfo[respbody.nRankListCount].nRoleID = tempRoomHostUin;
		respbody.arrayRankListInfo[respbody.nRankListCount].nRankListType = enmRankType_RoomRankList;
		respbody.arrayRankListInfo[respbody.nRankListCount].nPlace = i + 1;
//		WRITE_DEBUG_LOG( "query room rank info success! roomid=%d, roomhost roleid=%d, rank=%d\n",
//				tempRoomID,
//				tempRoomHostUin,
//				respbody.arrayRankListInfo[respbody.nRankListCount].nPlace);

		respbody.nRankListCount++;
	}

	//上周10种礼物之星
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));
	//memset(szSql, 0, sizeof(szSql));
	//sprintf(szSql, "select  `RoleID`, `ItemID`, `ItemCount` from(select `ItemID`, `ItemCount`, `RoleID`, `RoleName`, `Week` from vdc_stats.prop_obtain_week_notice where `Week`=%d order by `ItemCount` desc) temp group by ItemID order by ItemCount desc", nLastWeek);
	sprintf(szSql, "select `RoleID`, `ItemID`, `ItemCount` from vdc_stats.prop_obtain_week_notice where `Week`=%d group by `ItemID` order by `ItemCount` desc", nLastWeek);
//	CMysqlGuard  stSqlEngine(MYSQLREADENGINE);
	MYSQLREADENGINE.ExecuteQueryEnd();
	ret = stSqlEngine.ExecuteQuery(szSql, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on queyr gift rank failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query gift rank success! sql=%s\n", szSql);

	for(uint32_t i=0; i<nRowCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "fetch row of query gift rank failure! errorcode=0x%08X\n", ret);
			return ret;
		}
		//WRITE_DEBUG_LOG( "fetch row of queyr gift rank success!\n");

		if(NULL == arrRecordSet[0])
		{
			WRITE_NOTICE_LOG( "no record exist in db of giftstar lastweek! lastweek=%d\n", nLastWeek);
			break;
		}

		respbody.arrayGiftStarInfo[respbody.nGiftStarCount].nRoleID = (RoleID)atoi(arrRecordSet[0]);
		respbody.arrayGiftStarInfo[respbody.nGiftStarCount].nItemID = (ItemID)atoi(arrRecordSet[1]);
		respbody.arrayGiftStarInfo[respbody.nGiftStarCount].nGiftCount = (uint32_t)atoi(arrRecordSet[2]);

//		WRITE_DEBUG_LOG( "fetch gift rank list success! roleid=%d, itemid=%d, itemcount=%d\n",
//				respbody.arrayGiftStarInfo[respbody.nGiftStarCount].nRoleID,
//				respbody.arrayGiftStarInfo[respbody.nGiftStarCount].nItemID,
//				respbody.arrayGiftStarInfo[respbody.nGiftStarCount].nGiftCount);
		respbody.nGiftStarCount++;
	}

//	WRITE_DEBUG_LOG( "query all rank list info success! ranklistcount=%d, giftstarcount=%d\n",
//			respbody.nRankListCount, respbody.nGiftStarCount);

	return S_OK;
}

int32_t CFromSysinfoGetEvent::OnMessage_getRecharge(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: sysinfoserver get rankinfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetSupplymentReq* pTmpBody = dynamic_cast<CGetSupplymentReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetSupplymentResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead, MSGID_DSSS_SYPPLYMENT_RESP);

	ret = ProcessSysGetRecharge(*pTmpBody, respbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process sysinfoserver get rankinfo request failed! errorcode=0x%08X\n",
				ret);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process sysinfoserver get rankinfo success! \n");

	respbody.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromSysinfoGetEvent::ProcessSysGetRecharge(const CGetSupplymentReq& reqbody, CGetSupplymentResp& respbody)
{
	if(0 >= reqbody.nRoleCount)
	{
		WRITE_ERROR_LOG( "rolecount in sysinfo get user recharge record request is not right! rolecount=%d\n",
				reqbody.nRoleCount);
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "rolecount in sysinfo get user recharge record request is right! rolecount=%d\n",reqbody.nRoleCount);

	respbody.nResultCount = 0;

	CDateTime dtStart(reqbody.nStartTime * US_PER_SECOND);
	char szStartTime[enmMaxTimeStringLength] = {0};
	dtStart.ToDateTimeString(szStartTime);

	CDateTime dtEnd(reqbody.nEndTime * US_PER_SECOND);
	char szEndTime[enmMaxTimeStringLength] = {0};
	dtEnd.ToDateTimeString(szEndTime);

	int32_t ret = S_OK;

	for(uint32_t i=0; i<reqbody.nRoleCount; ++i)
	{
		//查询某个玩家在某个时间段内的总的充值数量
		respbody.arrRoleConsump[respbody.nResultCount].nRoleID = reqbody.arrRoleID[i];
		ret = QueryUserTotalRechargeInTimeBucket(reqbody.arrRoleID[i], reqbody.nStartTime, reqbody.nEndTime, respbody.arrRoleConsump[respbody.nResultCount].nSupplymentTotal);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "get user total recharge between time period is failed! roleid=%d, index=%d, time period: %s to %s\n", reqbody.arrRoleID[i], i, szStartTime, szEndTime);

			//失败则继续后面的未完成的玩家的查询，而不是直接返回错误
			continue;
		}
		//WRITE_DEBUG_LOG( "get user total recharge between time period success! roleid=%d, time period: %s to %s\n", reqbody.arrRoleID[i], szStartTime, szEndTime);

		respbody.nResultCount++;
	}

	if(0 >= respbody.nResultCount)
	{
		WRITE_ERROR_LOG( "get user total recharge failed! because resultcount is zero!\n");
		return E_QUERY_USER_TOTAL_RECHARGE_TIMESPAN;
	}
	//WRITE_DEBUG_LOG( "get user total recharge success! requestcount=%d, resultcount=%d\n", reqbody.nRoleCount, respbody.nResultCount);

	respbody.nResult = S_OK;
	return S_OK;
}
int32_t CFromSysinfoGetEvent::QueryUserTotalRechargeInTimeBucket(const RoleID roleid, const uint64_t starttime, const uint64_t endtime, int32_t& totalRechargeAmount)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select sum(`RechargeAmount`) from vdc_record.user_each_recharge where `RoleID`=%d and `RechargeTime`>=%d and `RechargeTime`<=%d", roleid, (int32_t)starttime, (int32_t)endtime);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	CDateTime dtStart(starttime * US_PER_SECOND);
	char szStartTime[enmMaxTimeStringLength] = {0};
	dtStart.ToDateTimeString(szStartTime);

	CDateTime dtEnd(endtime * US_PER_SECOND);
	char szEndTime[enmMaxTimeStringLength] = {0};
	dtEnd.ToDateTimeString(szEndTime);

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation on query user recharge between sometime failed! errorcode=0x%08X, roleid=%d, starttime=%s, endtime=%s, sql=%s\n",
				ret,
				roleid,
				szStartTime,
				szEndTime,
				szSql);
		return ret;
	}
	totalRechargeAmount = atoi(arrRecordSet[0]);
//	WRITE_DEBUG_LOG( "db operation on query user recharge record success! roleid=%d, starttime=%s, endtime=%s, sql=%s\n",
//			roleid,
//			szStartTime,
//			szEndTime,
//			szSql);

	return S_OK;
}

int32_t CFromSysinfoGetEvent::OnMessage_getVipInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: sysinfoserver get rankinfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetVipRoleReq* pTmpBody = dynamic_cast<CGetVipRoleReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetVipInfoResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DSSS_VIPINFO_RESP);

	ret = QueryUserVipInfo(respbody.nResultCount, respbody.arrVipInfo, *pTmpBody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process sysinfoserver get rankinfo request failed! errorcode=0x%08X\n",
				ret);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	//WRITE_DEBUG_LOG( "process sysinfoserver get rankinfo success! \n");

	respbody.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromSysinfoGetEvent::QueryUserVipInfo(uint32_t& vipplayercount, SingleRoleInfo arrVipPlayerInfo[], const CGetVipRoleReq& reqbody)
{
	if(enmVipLevel_Gold > reqbody.nVipLevel)
	{
		WRITE_WARNING_LOG( "warning: argument viplevel on get vip role request is invalid! viplevel=%d\n", reqbody.nVipLevel);
		vipplayercount = 0;
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "arguement viplevel on get vip role info request has no problem! minviplevel=%d\n", reqbody.nVipLevel);

	if(MaxDegradeCountEveryTime < reqbody.nLimit)
	{
		WRITE_ERROR_LOG( "error: too many count! count=%d\n", reqbody.nLimit);
		return E_MAXCOUNT;
	}

	CDateTime dt(reqbody.nBeingVipTime * US_PER_SECOND);
	char szBeingVipTime[enmMaxTimeStringLength] = {0};
	dt.ToDateTimeString(szBeingVipTime);

	//首先将数量置零
	vipplayercount = 0;

	CDateTime dtStart(reqbody.nStartTime * US_PER_SECOND);
	char szStartTime[enmMaxTimeStringLength] = {0};
	dtStart.ToDateTimeString(szStartTime);

	CDateTime dtEnd(reqbody.nEndTime * US_PER_SECOND);
	char szEndTime[enmMaxTimeStringLength] = {0};
	dtEnd.ToDateTimeString(szEndTime);

	//WRITE_DEBUG_LOG( "begin to query vip player info! viplevel=%d, beingVipTime=%s\n", reqbody.nVipLevel, szBeingVipTime);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	char szSqlGetVipUserList[enmMaxSqlStringLength] = {0};
	sprintf(szSqlGetVipUserList, "select `RoleID`, `VipLevel` from vdc_user.user_base_info where `VipLevel`>=%d and `BecomeVipTime`<%d limit %d, %d",
			reqbody.nVipLevel,
			(int32_t)reqbody.nBeingVipTime,
			reqbody.nBegin,
			reqbody.nLimit);

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQueryBegin(szSqlGetVipUserList, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: db operation on query vip player info failed! viplevel=%d, beingvipTime=%s, errorcode=0x%08X, sql=%s\n",
				reqbody.nVipLevel,
				szBeingVipTime,
				ret,
				szSqlGetVipUserList);
		return ret;
	}
//	WRITE_DEBUG_LOG( "db operation on query vip player info success! viplevel=%d, beingvipTime=%s, sql=%s\n",
//			reqbody.nVipLevel,
//			szBeingVipTime,
//			szSqlGetVipUserList);

	//取出所有查询到的记录（多条记录）
	for(uint32_t i=0; i<nRowCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "error: db operation of fetch vip player record info failed!\n");
			continue;
		}

		arrVipPlayerInfo[vipplayercount].nRoleID = atoi(arrRecordSet[0]);
		arrVipPlayerInfo[vipplayercount].nVipLevel = atoi(arrRecordSet[1]);
//		WRITE_DEBUG_LOG( "db operation of fetch row on query role item success! roleid=%d, viplevel=%d\n",
//				arrVipPlayerInfo[vipplayercount].nRoleID,
//				arrVipPlayerInfo[vipplayercount].nVipLevel);

		vipplayercount++;
	}

	if(0 >= vipplayercount)
	{
		WRITE_WARNING_LOG( "no vip player in db!\n");
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "there is some vip player in db! vipplayercount=%d\n", vipplayercount);

	//查询这些vip玩家在某个时间段内的充值记录
	char szSql[enmMaxSqlStringLength] = {0};
	uint32_t vipPlayerCountIndex = 0;
	for(uint32_t i=0; i<vipplayercount; ++i)
	{
		//memset(szSql, 0, sizeof(szSql));
		//memset(arrRecordSet, 0, sizeof(arrRecordSet));
		nRowCount = 0;
		nFieldCount = 0;

		sprintf(szSql, "select sum(`RechargeAmount`) from vdc_record.user_each_recharge where `RoleID`=%d and `RechargeTime`>=%d and `RechargeTime`<=%d",
				arrVipPlayerInfo[vipPlayerCountIndex].nRoleID,
				(int32_t)reqbody.nStartTime,
				(int32_t)reqbody.nEndTime);
		ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "db operation on query vip player recharge record failed! roleid=%d, viplevel=%d, starttime=%s, endtime=%s, errorcode=0x%08X, sql=%s\n",
					arrVipPlayerInfo[vipPlayerCountIndex].nRoleID,
					arrVipPlayerInfo[vipPlayerCountIndex].nVipLevel,
					szStartTime,
					szEndTime,
					ret,
					szSql);
			continue;
		}
//		WRITE_DEBUG_LOG( "db operation on query vip player recharge record success! roleid=%d, viplevel=%d, starttime=%s, endtime=%s, sql=%s\n",
//				arrVipPlayerInfo[i].nRoleID,
//				arrVipPlayerInfo[i].nVipLevel,
//				szStartTime,
//				szEndTime,
//				szSql);

		arrVipPlayerInfo[vipPlayerCountIndex].nSupplymentTotal = atoi(arrRecordSet[0]);
		vipPlayerCountIndex++;
	}

	if(0 >= vipPlayerCountIndex)
	{
		WRITE_WARNING_LOG( "we get none recharge record of vip player! starttime=%s, endtime=%s\n",
				szStartTime,
				szEndTime);
		return S_OK;
	}
//	WRITE_DEBUG_LOG( "query vip user recharge record from db success! we have get %d vip player recharge record! starttime=%s, endtime=%s\n",
//			vipPlayerCountIndex,
//			szStartTime,
//			szEndTime);

	return S_OK;
}

int32_t CFromSysinfoGetEvent::OnMessage_getMsg(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: sysinfoserver get rankinfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CGetMessageReq* pTmpBody = dynamic_cast<CGetMessageReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CGetMessageResp respbody;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead, MSGID_DSSS_GETMSG_RESP);

	ret = ProcessSysGetMsg(*pTmpBody, respbody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process sysinfoserver get rankinfo request failed! errorcode=0x%08X, roleid=%d\n",
				ret, pTmpBody->nRoleID);
		respbody.nResult = ret;
		g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	//WRITE_DEBUG_LOG( "process sysinfoserver get rankinfo success! \n");

	respbody.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &respbody, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &respbody, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromSysinfoGetEvent::ProcessSysGetMsg(const CGetMessageReq& reqbody, CGetMessageResp& respbody)
{
	respbody.nRoleID = reqbody.nRoleID;
	if(reqbody.nRoleID == enmInvalidRoleID || reqbody.nChannelType == enumSysChannelType_Invalid)
	{
		WRITE_ERROR_LOG( "argument are not right on process sysinfo get msg request! roleid=%d, channeltype=%d\n",
				reqbody.nRoleID,
				reqbody.nChannelType);
		respbody.nResult = E_INVALID_ARGUMENTS;
		return E_INVALID_ARGUMENTS;
	}
	WRITE_DEBUG_LOG( "argument are all right on process sysinfo get msg request! roleid=%d, channeltype=%d\n",
			reqbody.nRoleID,
			reqbody.nChannelType);

	int32_t ret = S_OK;
	switch(reqbody.nReaded)
	{
		case enmMsgReadedType_New:
		ret = GetUnReadeSysMessage(reqbody, respbody);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "get unreaded message from db failed! errorcode=0x%08X, roleid=%d, channeltype=%d, readed=%d\n",
					ret,
					reqbody.nRoleID,
					reqbody.nChannelType,
					reqbody.nReaded);
			respbody.nResult = ret;
			return ret;
		}
//		WRITE_DEBUG_LOG( "get unreaded message from db success! roleid=%d, channeltype=%d, readed=%d\n",
//				reqbody.nRoleID,
//				reqbody.nChannelType,
//				reqbody.nReaded);
		respbody.nResult = S_OK;
		return S_OK;
		case enmMsgReadedType_Readed:
		ret = GetReadedSysMessage(reqbody, respbody);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "get already readed message from db failed! errorcode=0x%08X, roleid=%d, channeltype=%d, readed=%d\n",
					ret,
					reqbody.nRoleID,
					reqbody.nChannelType,
					reqbody.nReaded);
			respbody.nResult = ret;
			return ret;
		}
//		WRITE_DEBUG_LOG( "get already readed message from db success! roleid=%d, channeltype=%d, readed=%d\n",
//				reqbody.nRoleID,
//				reqbody.nChannelType,
//				reqbody.nReaded);
		respbody.nResult = S_OK;
		return S_OK;
		case enmMsgReadedType_Total:
		ret = GetTotalSysMessage(reqbody, respbody);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "get total message from db failed! errorcode=0x%08X, roleid=%d, channeltype=%d, readed=%d\n",
					ret,
					reqbody.nRoleID,
					reqbody.nChannelType,
					reqbody.nReaded);
			respbody.nResult = ret;
			return ret;
		}
//		WRITE_DEBUG_LOG( "get total message from db success! roleid=%d, channeltype=%d, readed=%d\n",
//				reqbody.nRoleID,
//				reqbody.nChannelType,
//				reqbody.nReaded);
		respbody.nResult = S_OK;
		return S_OK;
		default:
		respbody.nResultCount = 0;
		respbody.nResult = S_OK;
		WRITE_WARNING_LOG( "unknown readed type on query role system message! readtype=%d\n", reqbody.nReaded);
		return S_OK;
	}

	return S_OK;
}
int32_t CFromSysinfoGetEvent::GetUnReadeSysMessage(const CGetMessageReq& reqbody, CGetMessageResp& respbody)
{
	if(enmInvalidRoleID == reqbody.nRoleID || enumSysChannelType_Invalid == reqbody.nChannelType)
	{
		WRITE_WARNING_LOG( "warning: argument invalid on get not readed system message! roleid=%d, channeltype=%d\n",
				reqbody.nRoleID,
				reqbody.nChannelType);
		respbody.nResult = E_INVALID_ARGUMENTS;
		return E_INVALID_ARGUMENTS;
	}
//	WRITE_DEBUG_LOG( "arguement all right on get not readed system message! roleid=%d, channeltype=%d\n",
//			reqbody.nRoleID,
//			reqbody.nChannelType);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	uint64_t nAffectedRows = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));
	char szSqlNewMsg[enmMaxSqlStringLength] = {0};

	sprintf(szSqlNewMsg, "select `msg_id`, `msg_content`, `msg_inserttime` from vdc_user.user_message where `msg_owner`=%d and `msg_readed`=%d and `msg_channel_type`=%d limit %d, %d",
			reqbody.nRoleID,
			enmMsgReadedType_New,
			reqbody.nChannelType,
			reqbody.nBegin,
			reqbody.nLimit);

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQueryBegin(szSqlNewMsg, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: db operation on query readed system info failed! errorcode=0x%08X, sql=%s\n",
				ret,
				szSqlNewMsg);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query system message success! sql=%s\n", szSqlNewMsg);

	//取出所有查询到的记录（多条记录）
	respbody.nResultCount = 0;
	for(uint32_t i=0; i<nRowCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "error: db operation of fetch system message failed!\n");
			continue;
		}
		//WRITE_DEBUG_LOG( "db operation of fetch system message success!\n");

		respbody.arrMsgInfos[respbody.nResultCount].nRoleID = reqbody.nRoleID;
		respbody.arrMsgInfos[respbody.nResultCount].nChannelType = reqbody.nChannelType;
		respbody.arrMsgInfos[respbody.nResultCount].nReaded = enmMsgReadedType_Readed;
		respbody.arrMsgInfos[respbody.nResultCount].nMsgID = (uint32_t)atoi(arrRecordSet[0]);
		respbody.arrMsgInfos[respbody.nResultCount].szMsgContent = arrRecordSet[1];
		respbody.arrMsgInfos[respbody.nResultCount].nMsgdate = (uint32_t)atoi(arrRecordSet[2]);

		//修改表user_message: readed字段为已读
		memset(szSqlNewMsg, 0, sizeof(szSqlNewMsg));
		nAffectedRows = 0;
		sprintf(szSqlNewMsg, "update vdc_user.user_message set `msg_readed`=%d where `msg_id`=%d", enmMsgReadedType_Readed, respbody.arrMsgInfos[respbody.nResultCount].nMsgID);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSqlNewMsg, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "execute update readed failed! errorcode=0x%08X, sql=%s\n", ret, szSqlNewMsg);
			continue;
		}
		//WRITE_DEBUG_LOG( "execute update readed success! sql=%s\n", szSqlNewMsg);

		//修改user_message_statistic表中的log_new_%s字段，取一条减1
		memset(szSqlNewMsg, 0, sizeof(szSqlNewMsg));
		nAffectedRows = 0;
		switch(reqbody.nChannelType)
		{
			case enumSysChannelType_Upgrade:
			sprintf(szSqlNewMsg, "update vdc_user.user_message_statistic set `log_new_vipup`=`log_new_vipup`-1 where `msg_owner`=%d and `log_new_vipup` > 0",
					reqbody.nRoleID);
			ret = MYSQLREADENGINE.ExecuteUpdate(szSqlNewMsg, nAffectedRows);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "execute vdc_user.user_message_statistic log_new_vipup failure! errorcode=0x%08X, sql=%s\n",
						ret,
						szSqlNewMsg);
				continue;
			}
			//WRITE_DEBUG_LOG( "execute vdc_user.user_message_statistic log_new_vipup success! sql=%s\n", szSqlNewMsg);
			break;
			case enumSysChannelType_Degrade:
			sprintf(szSqlNewMsg, "update vdc_user.user_message_statistic set `log_new_vipdegrade`=`log_new_vipdegrade`-1 where `msg_owner`=%d and `log_new_vipdegrade` > 0",
					reqbody.nRoleID);
			ret = MYSQLREADENGINE.ExecuteUpdate(szSqlNewMsg, nAffectedRows);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "execute vdc_user.user_message_statistic log_new_vipdegrade failure! errorcode=0x%08X, sql=%s\n",
						ret,
						szSqlNewMsg);
				continue;
			}
			//WRITE_DEBUG_LOG( "execute update user_message_statistic log_new_vipdegrade success! sql=%s\n", szSqlNewMsg);
			break;
			case enumSysChannelType_Ranklist:
			sprintf(szSqlNewMsg, "update vdc_user.user_message_statistic set `log_new_rank`=`log_new_rank`-1 where `msg_owner`=%d and `log_new_rank` > 0",
					reqbody.nRoleID);
			ret = MYSQLREADENGINE.ExecuteUpdate(szSqlNewMsg, nAffectedRows);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "execute udpate user_messsage_statistic log_new_rank failure! errorcode=0x%08X, sql=%s\n",
						ret,
						szSqlNewMsg);
				continue;
			}
			//WRITE_DEBUG_LOG( "execute update user_message_statistic log_new_rank success! sql=%s\n", szSqlNewMsg);
			break;
			case enumSysChannelType_Room:
			sprintf(szSqlNewMsg, "update vdc_user.user_message_statistic set `log_new_roomrelated`=`log_new_roomrelated`-1 where `msg_owner`=%d and `log_new_roomrelated` > 0",
					reqbody.nRoleID);
			ret = MYSQLREADENGINE.ExecuteUpdate(szSqlNewMsg, nAffectedRows);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "execute udpate user_messsage_statistic log_new_roomrelated failure! errorcode=0x%08X, sql=%s\n",
						ret,
						szSqlNewMsg);
				continue;
			}
			//WRITE_DEBUG_LOG( "execute update user_message_statistic log_new_roomrelated success! sql=%s\n", szSqlNewMsg);
			break;
			case enumSysChannelType_Charge:
			sprintf(szSqlNewMsg, "update vdc_user.user_message_statistic set `log_new_charge`=`log_new_charge`-1 where `msg_owner`=%d and `log_new_charge` > 0",
					reqbody.nRoleID);
			ret = MYSQLREADENGINE.ExecuteUpdate(szSqlNewMsg, nAffectedRows);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "execute udpate user_messsage_statistic log_new_charge failure! errorcode=0x%08X, sql=%s\n",
						ret,
						szSqlNewMsg);
				continue;
			}
			//WRITE_DEBUG_LOG( "execute update user_message_statistic log_new_charge success! sql=%s\n", szSqlNewMsg);
			break;
			default:
			WRITE_WARNING_LOG( "unknown channeltype! channeltype=%d\n", reqbody.nChannelType);
			continue;
		}//end of switch

		respbody.nResultCount++;
	}

	if(0 >= respbody.nResultCount)
	{
		WRITE_WARNING_LOG( "no such message record in db! roleid=%d, channeltype=%d, readed=%d\n",
				reqbody.nRoleID,
				reqbody.nChannelType,
				reqbody.nReaded);
		return S_OK;
	}
//	WRITE_DEBUG_LOG( "there is %d such system message in db! msgcount=%d, roleid=%d, channeltype=%d, readed=%d\n",
//			respbody.nResultCount,
//			reqbody.nRoleID,
//			reqbody.nChannelType,
//			reqbody.nReaded);

	return S_OK;
}
int32_t CFromSysinfoGetEvent::GetReadedSysMessage(const CGetMessageReq& reqbody, CGetMessageResp& respbody)
{
	if(enmInvalidRoleID == reqbody.nRoleID || enumSysChannelType_Invalid == reqbody.nChannelType)
	{
		WRITE_WARNING_LOG( "warning: argument invalid on get readed system message! roleid=%d, channeltype=%d\n",
				reqbody.nRoleID,
				reqbody.nChannelType);
		respbody.nResult = E_INVALID_ARGUMENTS;
		return E_INVALID_ARGUMENTS;
	}
//	WRITE_DEBUG_LOG( "arguement all right on get readed system message! roleid=%d, channeltype=%d\n",
//			reqbody.nRoleID,
//			reqbody.nChannelType);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	char szSqlReadMsg[enmMaxSqlStringLength] = {0};
	sprintf(szSqlReadMsg, "select `msg_id`, `msg_content`, `msg_inserttime` from vdc_user.user_message where `msg_owner`=%d and `msg_readed`=%d limit %d, %d",
			reqbody.nRoleID,
			enmMsgReadedType_Readed,
			reqbody.nBegin,
			reqbody.nLimit);

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQueryBegin(szSqlReadMsg, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: db operation on query readed system info failed! errorcode=0x%08X, sql=%s\n",
				ret,
				szSqlReadMsg);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query vip player info success! sql=%s\n", szSqlReadMsg);

	//取出所有查询到的记录（多条记录）
	respbody.nResultCount = 0;
	for(uint32_t i=0; i<nRowCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "error: db operation of fetch vip player record info failed!\n");
			continue;
		}

		respbody.arrMsgInfos[respbody.nResultCount].nRoleID = reqbody.nRoleID;
		respbody.arrMsgInfos[respbody.nResultCount].nChannelType = reqbody.nChannelType;
		respbody.arrMsgInfos[respbody.nResultCount].nReaded = enmMsgReadedType_Readed;
		respbody.arrMsgInfos[respbody.nResultCount].nMsgID = (uint32_t)atoi(arrRecordSet[0]);
		respbody.arrMsgInfos[respbody.nResultCount].szMsgContent = arrRecordSet[1];
		respbody.arrMsgInfos[respbody.nResultCount].nMsgdate = (uint32_t)atoi(arrRecordSet[2]);

		respbody.nResultCount++;
	}

	if(0 >= respbody.nResultCount)
	{
		WRITE_WARNING_LOG( "no such message record in db! roleid=%d, channeltype=%d, readed=%d\n",
				reqbody.nRoleID,
				reqbody.nChannelType,
				reqbody.nReaded);
		return S_OK;
	}
//	WRITE_DEBUG_LOG( "there is %d such system message in db! msgcount=%d, roleid=%d, channeltype=%d, readed=%d\n",
//			respbody.nResultCount,
//			reqbody.nRoleID,
//			reqbody.nChannelType,
//			reqbody.nReaded);

	return S_OK;
}
int32_t CFromSysinfoGetEvent::GetTotalSysMessage(const CGetMessageReq& reqbody, CGetMessageResp& respbody)
{
	if(enmInvalidRoleID == reqbody.nRoleID || enumSysChannelType_Invalid == reqbody.nChannelType)
	{
		WRITE_WARNING_LOG( "warning: argument invalid on get total system message! roleid=%d, channeltype=%d\n",
				reqbody.nRoleID,
				reqbody.nChannelType);
		respbody.nResult = E_INVALID_ARGUMENTS;
		return E_INVALID_ARGUMENTS;
	}
//	WRITE_DEBUG_LOG( "arguement all right on get total system message! roleid=%d, channeltype=%d\n",
//			reqbody.nRoleID,
//			reqbody.nChannelType);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	uint64_t nAffectedRows = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	char szSqlTotalMsg[enmMaxSqlStringLength] = {0};
	sprintf(szSqlTotalMsg, "select `msg_id`, `msg_content`, `msg_inserttime` from vdc_user.user_message where `msg_owner`=%d limit %d, %d",
			reqbody.nRoleID,
			reqbody.nBegin,
			reqbody.nLimit);

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQueryBegin(szSqlTotalMsg, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: db operation on query readed system info failed! errorcode=0x%08X, sql=%s\n",
				ret,
				szSqlTotalMsg);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query vip player info success! sql=%s\n", szSqlTotalMsg);

	//取出所有查询到的记录（多条记录）
	respbody.nResultCount = 0;
	for(uint32_t i=0; i<nRowCount; ++i)
	{
		ret = MYSQLREADENGINE.FetchRow(arrRecordSet, enmMaxFieldValueLength);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "error: db operation of fetch vip player record info failed!\n");
			continue;
		}

		respbody.arrMsgInfos[respbody.nResultCount].nRoleID = reqbody.nRoleID;
		respbody.arrMsgInfos[respbody.nResultCount].nChannelType = reqbody.nChannelType;
		respbody.arrMsgInfos[respbody.nResultCount].nReaded = enmMsgReadedType_Readed;
		respbody.arrMsgInfos[respbody.nResultCount].nMsgID = (uint32_t)atoi(arrRecordSet[0]);
		respbody.arrMsgInfos[respbody.nResultCount].szMsgContent = arrRecordSet[1];
		respbody.arrMsgInfos[respbody.nResultCount].nMsgdate = (uint32_t)atoi(arrRecordSet[2]);

		respbody.nResultCount++;
	}

	if(0 >= respbody.nResultCount)
	{
		WRITE_WARNING_LOG( "no such message record in db! roleid=%d, channeltype=%d, readed=%d\n",
				reqbody.nRoleID,
				reqbody.nChannelType,
				reqbody.nReaded);
		return S_OK;
	}
//	WRITE_DEBUG_LOG( "there is %d such system message in db! msgcount=%d, roleid=%d, channeltype=%d, readed=%d\n",
//			respbody.nResultCount,
//			reqbody.nRoleID,
//			reqbody.nChannelType,
//			reqbody.nReaded);

	//将user_message表中的该玩家的该channeltype的所有readed字段改为已读
	memset(szSqlTotalMsg, 0, sizeof(szSqlTotalMsg));
	nAffectedRows = 0;
	sprintf(szSqlTotalMsg, "update vdc_user.user_message set `msg_readed`=%d where `msg_owner`=%d and `msg_channel_type`=%d",
			enmMsgReadedType_Readed,
			reqbody.nRoleID,
			reqbody.nChannelType);
	ret = MYSQLREADENGINE.ExecuteUpdate(szSqlTotalMsg, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "execute update user_message readed field failed! errorcode=0x%08X, sql=%s\n",
				ret,
				szSqlTotalMsg);
	}
	//WRITE_DEBUG_LOG( "execute update user_message readed field success! sql=%s\n", szSqlTotalMsg);

	//将user_message_statistic表中的该channeltype的log_new字段改为0
	memset(szSqlTotalMsg, 0, sizeof(szSqlTotalMsg));
	nAffectedRows = 0;
	switch(reqbody.nChannelType)
	{
		case enumSysChannelType_Upgrade:
		sprintf(szSqlTotalMsg, "update vdc_user.user_message_statistic set `log_new_vipup`=%d where `msg_owner`=%d", 0, reqbody.nRoleID);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSqlTotalMsg, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "execute update user_message_statistic log_new_vipup field failed! errorcode=0x%08X, sql=%s\n",
					ret,
					szSqlTotalMsg);
			return ret;
		}
		//WRITE_DEBUG_LOG( "execute update user_message_statistic log_new_vipup field success! sql=%s\n", szSqlTotalMsg);
		return S_OK;
		case enumSysChannelType_Degrade:
		sprintf(szSqlTotalMsg, "update vdc_user.user_message_statistic set `log_new_vipdegrade`=%d where `msg_owner`=%d", 0, reqbody.nRoleID);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSqlTotalMsg, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "execute update user_message_statistic log_new_vipdegrade failed! errorcode=0x%08X, sql=%s\n",
					ret,
					szSqlTotalMsg);
			return ret;
		}
		//WRITE_DEBUG_LOG( "execute update user_message_statistic log_new_vipdegrade success! sql=%s\n", szSqlTotalMsg);
		return S_OK;
		case enumSysChannelType_Ranklist:
		sprintf(szSqlTotalMsg, "update vdc_user.user_message_statistic set `log_new_rank`=%d where `msg_owner`=%d", 0, reqbody.nRoleID);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSqlTotalMsg, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "execute update user_message_statistic log_new_rank failure! errorcode=0x%08X, sql=%s\n",
					ret,
					szSqlTotalMsg);
			return ret;
		}
		//WRITE_DEBUG_LOG( "execute update user_message_statistic log_new_rank success! sql=%s\n", szSqlTotalMsg);
		return S_OK;
		case enumSysChannelType_Room:
		sprintf(szSqlTotalMsg, "update vdc_user.user_message_statistic set `log_new_roomrelated`=%d where `msg_owner`=%d", 0, reqbody.nRoleID);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSqlTotalMsg, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "execute update user_message_statistic log_new_roomrelated failure! errorcode=0x%08X, sql=%s\n",
					ret,
					szSqlTotalMsg);
			return ret;
		}
		//WRITE_DEBUG_LOG( "execute update user_message_statistic log_new_roomrelated success! sql=%s\n", szSqlTotalMsg);
		return S_OK;
		case enumSysChannelType_Charge:
		sprintf(szSqlTotalMsg, "update vdc_user.user_message_statistic set `log_new_charge`=%d where `msg_owner`=%d", 0, reqbody.nRoleID);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSqlTotalMsg, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "execute update user_message_statistic log_new_charge failure! errorcode=0x%08X, sql=%s\n",
					ret,
					szSqlTotalMsg);
			return ret;
		}
		//WRITE_DEBUG_LOG( "execute update user_message_statistic log_new_charge success! sql=%s\n", szSqlTotalMsg);
		return S_OK;
		default:
		WRITE_WARNING_LOG( "unknown channeltype! channeltype=%d\n", reqbody.nChannelType);
		return S_OK;
	}

	return S_OK;
}

FRAME_DBPROXY_NAMESPACE_END
