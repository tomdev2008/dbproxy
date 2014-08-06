/*
 * bll_event_roomset_msg.cpp
 *
 *  Created on: 2013-3-6
 *      Author: liufl
 */

#include "bll_event_roomset_msg.h"
#include "../dal/roomserver_message.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

int32_t CFromRoomSetEvent::OnMessage_EnterRoomNotice(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver get RoleInfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CEnterRoomNotice_Public* pTmpBody = dynamic_cast<CEnterRoomNotice_Public*>(pMsgBody);

	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}

	if(pTmpBody->nRoleRank == enmRoleRank_None || pTmpBody->nRoleRank == enmRoleRank_TempAdmin || pTmpBody->nRoleRank == enmRoleRank_Super)
	{
		WRITE_WARNING_LOG( "admin permission is not stored in db! so just ignore is! admin pemission is %d, roomid=%d, roleid=%d\n",
				pTmpBody->nRoleRank,
				pTmpBody->nRoomID,
				pTmpBody->nRoleID);
		return S_OK;
	}
	WRITE_DEBUG_LOG( "admin permission is stored in db! admin permission is %d, roomid=%d, roleid=%d\n",
			pTmpBody->nRoleRank,
			pTmpBody->nRoomID,
			pTmpBody->nRoleID);

	//时间类
	CDateTime dt = CDateTime::CurrentDateTime();
	char szDateTime[enmMaxTimeStringLength] = {0};
	dt.ToDateTimeString(szDateTime);

	//更新room_member表中的最后进入房间的时间字段
	ret = UpdateAdminLastEnterRoomTime(pTmpBody->nRoomID, pTmpBody->nRoleID, szDateTime);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update admin last enter room time failed, operation time: %s, roleid=%d, roomid=%d, msgid=0x%08X\n",
				szDateTime,
				pTmpBody->nRoleID,
				pTmpBody->nRoomID,
				pMsgHead->nMessageID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update admin last enter room time success! operation time: %s, roleid=%d, roomid=%d, msgid=0x%08X\n",
//			szDateTime,
//			pTmpBody->nRoleID,
//			pTmpBody->nRoomID,
//			pMsgHead->nMessageID);

	//更新最近进入房间信息表
	ret = UpdateRoleLatestEnterRoomInfo(pTmpBody->nRoleID, pTmpBody->nRoomID, szDateTime);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update admin latest enter room info failed! errorcode=0x%08X, roleid=%d, roomid=%d\n",
				ret,
				pTmpBody->nRoleID,
				pTmpBody->nRoomID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update admin latest enter room info success! roleid=%d, roomid=%d\n",pTmpBody->nRoleID,pTmpBody->nRoomID);

	return S_OK;
}
int32_t CFromRoomSetEvent::UpdateAdminLastEnterRoomTime(const RoomID roomid, const RoleID roleid, const char* szDateTime)
{
	if(0>roomid || 0 > roleid)
	{
		WRITE_ERROR_LOG( "argument is invalid! roleid=%d, roomid=%d\n", roleid, roomid);
		return E_INVALID_ARGUMENTS;
	}

	int32_t ret = S_OK;

	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "update vdc_room.room_member set `LastEnterTime`='%s' where `RoleID`=%d and `RoomID`=%d", szDateTime, roleid, roomid);

	uint64_t nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "Error: execute update on update room admin last enterroom time failed!\
											roleid=%d, roomid=%d, lastlogintime=%s, errorcode=0x%08X, sql=%s\n",
				roleid, roomid, szDateTime, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "execute update on role last enterroom time success! roleid=%d, roomid=%d, last login time=%s, sql=%s\n",
	//		roleid, roomid, szDateTime, szSql);

	return S_OK;
}
int32_t CFromRoomSetEvent::UpdateRoleLatestEnterRoomInfo(const RoleID roleid, const RoomID roomid, const char* szLastEnterTime)
{
	//生成SQL语句
	char szSql[enmMaxSqlStringLength] = {0};

	sprintf(szSql, "insert into vdc_user.latest_enter_room (`RoleID`,`RoomID`,`LastEnterTime`) values (%d,%d,'%s') on duplicate key update `LastEnterTime` = '%s'" ,
			roleid,
			roomid,
			szLastEnterTime,
			szLastEnterTime);

	uint64_t nAffectedRows = 0;
	int32_t nRet = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if (0 > nRet)
	{
		WRITE_ERROR_LOG( "db operation on update user latest enter room info failed! errorcode=0x%08X, roleid=%d, roomid=%d, sql=%s\n",
				nRet,
				roleid,
				roomid,
				szSql);
		return nRet;
	}
	//WRITE_DEBUG_LOG( "db operation on update user latest enter room info success! roleid=%d, roomid=%d, sql=%s\n",roleid, roomid, szSql);

	if ( (1 != nAffectedRows) && (2!= nAffectedRows) )
	{
		WRITE_ERROR_LOG( "affected row number is not one or two, it is wrong on update user latest enter room info! affected rows=%d\n", nAffectedRows);
		nRet = E_UNKNOWN;
	}
	//WRITE_DEBUG_LOG( "affected row number is right on update user latest enter room! affected row=%d\n", nAffectedRows);

	return S_OK;
}

int32_t CFromRoomSetEvent::OnMessage_SetAttr(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver set attr!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CSetRoomAttrReq* pTmpBody = dynamic_cast<CSetRoomAttrReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CSetRoomAttrResp set_rolerank;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBRS_SET_ROOM_RESP);

	//直接回应
	set_rolerank.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &set_rolerank, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &set_rolerank, nOptionLen, (const char*)pOptionData);

	ret = ProcessRoomSetAttrReq(*pTmpBody, set_rolerank, pMsgHead->nRoomID);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process roomserver set attr request failed! errorcode=0x%08X\n",
				ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "process roomserver set attr success! \n");



	return S_OK;
}
int32_t CFromRoomSetEvent::ProcessRoomSetAttrReq(const CSetRoomAttrReq& reqbody, CSetRoomAttrResp& respbody, const RoomID roomid)
{
	//判断参数是否合法
	if(0 >= roomid)
	{
		WRITE_ERROR_LOG( "roomid in msghead is invalid! roomid=%d\n", roomid);
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "roomid in message head is valid! roomid=%d\n", roomid);

	if(reqbody.nStringCount > reqbody.nSetCount)
	{
		WRITE_ERROR_LOG( "Error: string set count is bigger than total set count! string set count=%d, total set count=%d\n",
				reqbody.nStringCount, reqbody.nSetCount);
		return E_STRING_COUNT_LARGERER_THAN_SET_COUNT;
	}
	//WRITE_DEBUG_LOG( "string set count is small than total set count! right status! threadindex=%d\n");

	RoomBaseInfo roombaseinfo;
	//memset(&roombaseinfo, 0, sizeof(RoomBaseInfo));
	roombaseinfo.uRoomID = (uint32_t)roomid;

	int32_t nTmpStringCount = 0;
	int32_t ret = S_OK;
	for(int32_t i=0; i<reqbody.nSetCount; ++i)
	{
		switch(reqbody.arrRoomInfoType[i])
		{
			case enmRoomInfoType_NAME:
			strcpy(roombaseinfo.szRoomName, reqbody.arrSetString[nTmpStringCount].GetString());
			ret = UpdateRoomBaseInfo(roombaseinfo, enmRoomInfoType_NAME);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "Error: set room name failed! roomid=%d, errorcode=0x%08X\n", roomid, ret);
				return ret;
			}
			//WRITE_DEBUG_LOG( "set room name success! roomid=%d, roomname=%s\n", roomid, reqbody.arrSetString[nTmpStringCount].GetString());
			nTmpStringCount++;
			break;
			case enmRoomInfoType_SIGNATURE:
			strcpy(roombaseinfo.szRoomTheme, reqbody.arrSetString[nTmpStringCount].GetString());
			ret = UpdateRoomBaseInfo(roombaseinfo, enmRoomInfoType_SIGNATURE);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "Error: set room signature failed! roomid=%d, errorcode=0x%08X\n", roomid, ret);
				return ret;
			}
			//WRITE_DEBUG_LOG( "set room signature success! roomid=%d\n", roomid);
			nTmpStringCount++;
			break;
			case enmRoomInfoType_PASSWORD:
			strcpy(roombaseinfo.szRoomPassword, reqbody.arrSetString[nTmpStringCount].GetString());
			ret = UpdateRoomBaseInfo(roombaseinfo, enmRoomInfoType_PASSWORD);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "Error: set room password failed! roomid=%d, errorcode=0x%08X\n", roomid, ret);
				return ret;
			}
			//WRITE_DEBUG_LOG( "set room password success! roomid=%d\n", roomid);
			nTmpStringCount++;
			break;
			case enmRoomInfoType_FREESHOW_TIME:
			roombaseinfo.nMicroTime = reqbody.nMicTime;
			ret = UpdateRoomBaseInfo(roombaseinfo, enmRoomInfoType_FREESHOW_TIME);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "Error: set room free show time failed! roomid=%d, free show time=%d, errorcode=0x%08X\n",
						roomid, reqbody.nMicTime, ret);
				return ret;
			}
			//WRITE_DEBUG_LOG( "set room free show time success! roomid=%d, free show time=%d\n", roomid, reqbody.nMicTime);
			break;
			case enmRoomInfoType_WELCOME_WORDS:
			strcpy(roombaseinfo.szWelcomewords, reqbody.arrSetString[nTmpStringCount].GetString());
			ret = UpdateRoomBaseInfo(roombaseinfo, enmRoomInfoType_WELCOME_WORDS);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "Error: set room welcome words failed! roomid=%d, welcome words=%s, errorcode=0x%08X\n",
						roomid, reqbody.arrSetString[nTmpStringCount].GetString(), ret);
				return ret;
			}
			//WRITE_DEBUG_LOG( "set room welcome words success! roomid=%d, welcome words=%s\n",
			//		roomid, reqbody.arrSetString[nTmpStringCount].GetString());
			nTmpStringCount++;
			break;
			case enmRoomInfoType_NOTICE:
			strcpy(roombaseinfo.szRoomNotice, reqbody.arrSetString[nTmpStringCount].GetString());
			ret = UpdateRoomBaseInfo(roombaseinfo, enmRoomInfoType_NOTICE);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "Error: set room notice failed! roomid=%d, room notice=%s, errorcode=0x%08X\n",
						roomid, reqbody.arrSetString[nTmpStringCount].GetString(), ret);
				return ret;
			}
			//WRITE_DEBUG_LOG( "set room notice success! roomid=%d, room notice=%s\n", roomid, reqbody.arrSetString[nTmpStringCount].GetString());
			nTmpStringCount++;
			break;
			case enmRoomInfoType_OPTION:
			roombaseinfo.nOption = (int32_t)reqbody.nRoomOptionType;
			ret = UpdateRoomBaseInfo(roombaseinfo, enmRoomInfoType_OPTION);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "Error: set room option failed! roomid=%d, errorcode=0x%08X\n", roomid, reqbody.nRoomOptionType);
				return ret;
			}
			//WRITE_DEBUG_LOG( "set room option success! roomid=%d, room option type=%d\n", roomid, reqbody.nRoomOptionType);
			break;
			default:
			WRITE_WARNING_LOG( "unknown room attributte! room info type=%d\n", reqbody.arrRoomInfoType[i]);
			break;
		}
	}

	//从数据库中读取room_base_info：为改写缓存做准备
	ret = QueryRoomBaseInfo(roomid, roombaseinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: query room base info from db failed! roomid=%d, errorcode=0x%08X\n", roomid, ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query room base info from db success! roomid=%d\n", roomid);

	//将该房间信息写入缓存中
	char szKey[enmMaxMemcacheKeyLen] = {0};
	size_t keylen = 0;
	char szMemcacheVal[enmMaxMemcacheValueLen] = {0};
	size_t valLen = 0;

	//获取key
	GenerateMemcacheKey(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoomID, roomid);

	uint32_t offset = 0;
	ret = CVDCRoomBaseInfo::VDCRoomBaseInfoEncode((uint8_t *)szMemcacheVal, enmMaxMemcacheValueLen, offset, roombaseinfo);
	valLen = (size_t)offset;

	CVDCMemcache& memcacheobj = GET_VDCMEMCACHE_INSTANCE();
	ret = memcacheobj.MemcacheSet(memcacheobj.m_memc, szKey, keylen, szMemcacheVal, valLen, 0);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: write room base info to memcache failed! roomid=%d, errorcode=0x%08X\n", roomid, ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "write room base info to memcache success! roomid=%d\n", roomid);

	//限制channelid的范围
	if(roombaseinfo.usChannelID > (ChannelID)MaxChannelID)
	{
		WRITE_ERROR_LOG( "error: channelid is too big! channelid=%ld\n", roombaseinfo.usChannelID);
		return E_MAX_CHANNELID;
	}

	//生成房间频道列表缓存的key,然后删除之
	memset(szKey, 0, sizeof(szKey));
	keylen = 0;

	GenerateRoomChannelKey(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoomChannel, roombaseinfo.usChannelID);
	//GenerateRoomChannelKey(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoomChannel, channelid);
	ret = memcacheobj.MemcacheDel(memcacheobj.m_memc, szKey, keylen, 0);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "delete room channel info list failed! errorcode=0x%08X, key=%s\n",
				ret, szKey);
		return ret;
	}
	//WRITE_DEBUG_LOG( "delete room channel info list memcache success! key=%s\n",szKey);

	//生成推荐频道的Key， 然后删除之
	memset(szKey, 0, sizeof(szKey));
	keylen = 0;
	GenerateRecommandRoomChannelKey(szKey, enmMaxMemcacheKeyLen, keylen, 0, roombaseinfo.usChannelID);

	ret = memcacheobj.MemcacheDel(memcacheobj.m_memc, szKey, keylen, 0);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "delete room recommand channel list failed! errorcode=0x%08X, key=%s\n",
				ret, szKey);
		return ret;
	}
	//WRITE_DEBUG_LOG( "delete room recommand channel list success! key=%s\n",szKey);

	return S_OK;
}
int32_t CFromRoomSetEvent::UpdateRoomBaseInfo(const RoomBaseInfo& roombaseinfo, const int32_t roombaseinfotype)
{
	if(roombaseinfo.uRoomID <= 0 || roombaseinfo.nRoomShowCapacity < 0 || roombaseinfo.nRoomRealCapacity < 0)
	{
		WRITE_ERROR_LOG( "Error: arguments in update room base info is invalid! so just ignore!\n");
		return E_INVALID_ARGUMENTS;
	}

	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	CDateTime dt = CDateTime::CurrentDateTime();

	uint64_t nAffectedRows = 0;

	switch(roombaseinfotype)
	{
		case enmRoomInfoType_NAME:
		sprintf(szSql, "update vdc_room.room_base_info set `BeingConfRoomName`='%s', `RoomAuditStatus`=%d, `RoomNameAuditTime`=%d where `RoomID`=%d",
				roombaseinfo.szRoomName, enmRoomAuditType_UnAudited, (int32_t)dt.Seconds(), roombaseinfo.uRoomID);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0>ret)
		{
			WRITE_ERROR_LOG( "Error: update room name failed! roomid=%d, errorcode=0x%08X, sql:%s\n",
					roombaseinfo.uRoomID, ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "update room name success! roomid=%d, sql=%s\n",roombaseinfo.uRoomID, szSql);
		break;
		case enmRoomInfoType_SIGNATURE:
		sprintf(szSql, "update vdc_room.room_base_info set `RoomTheme`='%s' where `RoomID`=%d", roombaseinfo.szRoomTheme, roombaseinfo.uRoomID);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "Error: update room theme failed! roomid=%d, errorcode=0x%08X, sql=%s\n",
					roombaseinfo.uRoomID, ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "update room theme success! roomid=%d, sql=%s\n",roombaseinfo.uRoomID, szSql);
		break;
		case enmRoomInfoType_PASSWORD:
		sprintf(szSql, "update vdc_room.room_base_info set `Password`='%s' where `RoomID`=%d", roombaseinfo.szRoomPassword, roombaseinfo.uRoomID);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "update room password failed! roomid=%d, errorcode=0x%08X, sql=%s\n",
					roombaseinfo.szRoomPassword, ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "update room password success! roomid=%d, %s\n", roombaseinfo.uRoomID, szSql);
		break;
		case enmRoomInfoType_FREESHOW_TIME:
		sprintf(szSql, "update vdc_room.room_base_info set `MicroTime`=%d where `RoomID`=%d", roombaseinfo.nMicroTime, roombaseinfo.uRoomID);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "update room micro time failed! roomid=%d, errorcode=0x%08X, sql=%s\n",
					roombaseinfo.uRoomID, ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "update room micro time success! roomid=%d, sql=%s\n",roombaseinfo.uRoomID, szSql);
		break;
		case enmRoomInfoType_WELCOME_WORDS:
		sprintf(szSql, "update vdc_room.room_base_info set `WelcomeWords`='%s' where `RoomID`=%d", roombaseinfo.szWelcomewords, roombaseinfo.uRoomID);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "update room welcome words failed! roomid=%d, errorcode=0x%08X, sql=%s\n",
					roombaseinfo.uRoomID, ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "update room welcome words success! roomid=%d, sql=%s\n", roombaseinfo.uRoomID, szSql);
		break;
		case enmRoomInfoType_NOTICE:
		sprintf(szSql, "update vdc_room.room_base_info set `RoomNotice`='%s' where `RoomID`=%d", roombaseinfo.szRoomNotice, roombaseinfo.uRoomID);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "update room notice failed! roomid=%d, errorcode=0x%08X, sql=%s\n",
					roombaseinfo.uRoomID, ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "update room notice success! roomid=%d, sql=%s\n", roombaseinfo.uRoomID, szSql);
		break;
		case enmRoomInfoType_OPTION:
		sprintf(szSql, "update vdc_room.room_base_info set `Option`=%d where `RoomID`=%d", roombaseinfo.nOption, roombaseinfo.uRoomID);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG( "update room option failed! roomid=%d, errorcode=0x%08X, sql=%s\n", roombaseinfo.uRoomID, ret, szSql);
			return ret;
		}
		//WRITE_DEBUG_LOG( "update room option success! roomid=%d, sql=%s\n", roombaseinfo.uRoomID, szSql);
		break;
		default:
		WRITE_WARNING_LOG( "warning: undefined room info type! room info type=%d\n", roombaseinfotype);
		break;
	}

	return S_OK;
}

int32_t CFromRoomSetEvent::OnMessage_SetPlayerCount(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen , const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver setPlayerCount!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CStaPlayerCount* pTmpBody = dynamic_cast<CStaPlayerCount*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}

	char szKey[enmMaxMemcacheKeyLen] = {0};
	size_t keylen = 0;
	char szVal[enmMaxMemcacheValueLen] = {0};
	GeneratePlayerCountKey(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoomPlayerCount, pTmpBody->nRoomID);
	sprintf(szVal, "%d", pTmpBody->nPlayerCount);

	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();
	ret = cacheobj.MemcacheSet(cacheobj.m_memc, szKey, keylen, szVal, (size_t)strlen(szVal), 0);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "write room player count into memcache failed! errorcode=0x%08X, roomid=%d\n",
			ret, pTmpBody->nRoomID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "write room player count into memcache success! roomid=%d\n", pTmpBody->nRoomID);

	memset(szKey, 0, sizeof(szKey));
	memset(szVal, 0, sizeof(szVal));
	GeneratePlayerCountKey(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoomRobotCount, pTmpBody->nRoomID);
	sprintf(szVal, "%d", pTmpBody->nRobotCount);
	ret = cacheobj.MemcacheSet(cacheobj.m_memc, szKey, keylen, szVal,  (size_t)strlen(szVal), 0);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "write room robot count into memcache  failed! errorcode=0x%08X, roomid=%d\n",
			ret, pTmpBody->nRoomID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "write room robot count into memcache success! roomid=%d\n",pTmpBody->nRoomID);

	return S_OK;
}

int32_t CFromRoomSetEvent::OnMessage_SetTitle(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver set title!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CRoomSetTitleReq* pTmpBody = dynamic_cast<CRoomSetTitleReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CRoomSetTitleResp set_rolerank;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBRS_SET_TITLE_RESP);
	//直接返回
	set_rolerank.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &set_rolerank, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &set_rolerank, nOptionLen, (const char*)pOptionData);

	RoomAdminInfo roomadmin;
	roomadmin.nRoleID = pTmpBody->nRoleID;
	roomadmin.uRoomID = pMsgHead->nRoomID;
	roomadmin.usRoleRank = pTmpBody->nRoleRank;
	ret = UpdateRoomAdminInfo(roomadmin);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process roomserver set title request failed! errorcode=0x%08X\n",
				ret);
		set_rolerank.nResult = ret;
		//不回应
//		g_Frame.PostMessage(&resphead, &set_rolerank, nOptionLen, (const char*)pOptionData);
//		DumpMessage("", &resphead, &set_rolerank, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	//WRITE_DEBUG_LOG( "process roomserver set title success! \n");

	set_rolerank.nResult = S_OK;

	g_Frame.PostMessage(&resphead, &set_rolerank, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &set_rolerank, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromRoomSetEvent::UpdateRoomAdminInfo(const RoomAdminInfo& roomadmin)
{
	if(roomadmin.nRoleID < 0 || roomadmin.uRoomID <= 0)
	{
		WRITE_ERROR_LOG( "Error: arguments in update room admin info invalid! threadindex=%d\n");
		return E_INVALID_ARGUMENTS;
	}

	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	sprintf(szSql, "insert into vdc_room.room_member(`RoomID`, `RoleID`, `RoleRank`) values(%d, %d, %d) on duplicate key update `RoleRank`=%d",
	roomadmin.uRoomID,
	roomadmin.nRoleID,
	roomadmin.usRoleRank,
	roomadmin.usRoleRank);

	uint64_t nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "Error: update room member table failed! roomid=%d, roleid=%d, errorcode=0x%08X, sql:%s\n",
			roomadmin.uRoomID, roomadmin.nRoleID, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update room member table success! roleid=%d, sql=%s\n",roomadmin.nRoleID, szSql);

	return S_OK;
}

int32_t CFromRoomSetEvent::OnMessage_UpdownMike(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver get RoleInfo!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CStaPlayerMicStatus* pTmpBody = dynamic_cast<CStaPlayerMicStatus*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	ret = InsertUpdownMikeInfo(*pTmpBody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "insert updown mike failed! errorcode=0x%08X, roleid=%d, roomid=%d\n",
				ret,
				pTmpBody->nRoleID,
				pTmpBody->nRoomID);
		return ret;
	}
	return S_OK;
}
int32_t CFromRoomSetEvent:: InsertUpdownMikeInfo(const CStaPlayerMicStatus& msgbody)
{
	if(msgbody.nRoomID == enmInvalidRoomID || msgbody.nRoleID == enmInvalidRoleID)
	{
		WRITE_ERROR_LOG( "invalid argument in insert up down mike info notice! roomid=%d, roleid=%d\n",
			msgbody.nRoomID, msgbody.nRoleID);
		return E_INVALID_ARGUMENTS;
	}

	int32_t ret = S_OK;

	CDateTime dt = CDateTime::CurrentDateTime();
	int32_t dateline = dt.Year() * 10000 + dt.Month() * 100 + dt.Day();

	char nRoleName[MaxRoleNameLength*2]={0};
	memset(nRoleName, 0, sizeof(nRoleName));
	//将玩家昵称进行转义避免SQL注入
	mysql_escape_string(nRoleName, msgbody.strRoleName.GetString(), msgbody.strRoleName.GetStringLength());

	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "insert into vdc_user_action.`updownmike_%.4d_%.2d_%.2d`(`RoomID`, `RoleID`, `RoleName`, `RoleIdentity`, `MsgID`, `Type`, `ActionTime`, `DateLine`, `Result`) \
				   values(%d, %d, '%s', %d, %d, %d, %d, %d, %d)",
				   dt.Year(),
				   dt.Month(),
				   dt.Day(),
				   msgbody.nRoomID,
				   msgbody.nRoleID,
				   nRoleName,
				   msgbody.nIdentityType,
				   msgbody.nMsgID,
				   msgbody.nMicOperateType,
				   (int32_t)dt.Seconds(),
				   msgbody.nTime,
				   msgbody.nMicResult);

	uint64_t nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "Error: execute role recharge info table failed! roomid=%d, roleid=%d, errorcode=0x%08X, sql:%s\n",
			msgbody.nRoomID, msgbody.nRoleID, ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "execute role recharge info table success! roomid=%d, roleid=%d, sql=%s\n",msgbody.nRoomID, msgbody.nRoleID, szSql);

	int32_t mday = dt.Year() * 10000 + dt.Month()*100 + dt.Day();

	//update updownmike_total
	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "insert into vdc_user_action.updownmike_total(`RoleID`, `RoomID`, `Type`, `ActionTime`, `Day`) values(%d, %d, %d, %ld, %d)"
		" on duplicate key update `Type`=%d, `ActionTime`=%ld",
		msgbody.nRoleID,
		msgbody.nRoomID,
		msgbody.nMicOperateType,
		dt.Seconds(),
		mday,
		msgbody.nMicOperateType,
		dt.Seconds());
	nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "error: execute update updownmike_total failed! errorcode=0x%08X, sql=%s\n",
			ret, szSql);
		return ret;
	}

	return S_OK;
}

int32_t CFromRoomSetEvent::OnMessage_SetSongList(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: update song list!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CUpdateSongListNoti* pTmpBody = dynamic_cast<CUpdateSongListNoti*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	ret = UpdateSongList(*pTmpBody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update song list failed! errorcode=0x%08X\n",ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update song list success! \n");

	return S_OK;

}
int32_t CFromRoomSetEvent::UpdateSongList(const CUpdateSongListNoti& reqbody)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	uint64_t nAffectedRows = 0;

	char nSongName[MaxSongNameLength*2] = {0};

	for(int32_t i = 0; i < reqbody.nSongCount; ++i)
	{
		memset(nSongName, 0, sizeof(nSongName));
		mysql_escape_string(nSongName, reqbody.arrSongName[i].GetString(), reqbody.arrSongName[i].GetStringLength());

		//memset(szSql, 0, sizeof(szSql));
		sprintf(szSql, "insert into vdc_requestsong.song_list(`SongIndex`, `SongerRoleID`, `RoomID`, `SongName`, `SongAttr`)"
				"values(%d, %d, %d, '%s', %d) on duplicate key update `SongName` = '%s' , `SongAttr` = %d ",
				reqbody.arrSongIndex[i],
				reqbody.nSongerRoleID,
				reqbody.nRoomID,
				reqbody.arrSongName[i].GetString(),
				reqbody.arrSongAttr[i],
				reqbody.arrSongName[i].GetString(),
				reqbody.arrSongAttr[i]
				);

		//WRITE_DEBUG_LOG("update song list sql:: %s",szSql);
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("update song list  failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
			return ret;
		}
	}
	return S_OK;
}

int32_t CFromRoomSetEvent::OnMessage_SetSongOrder(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: write song order!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CUpdateSongOrderInfoNoti* pTmpBody = dynamic_cast<CUpdateSongOrderInfoNoti*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	ret = InsertRoomSongOrder(*pTmpBody);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "write song order failed! errorcode=0x%08X\n",ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "write song order success! \n");

	return S_OK;
}
int32_t CFromRoomSetEvent::InsertRoomSongOrder(const CUpdateSongOrderInfoNoti& reqbody)
{
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};
	uint64_t nAffectedRows = 0;

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	char nSongName[MaxSongNameLength*2] = {0};
	mysql_escape_string(nSongName, reqbody.strSongName.GetString(), reqbody.strSongName.GetStringLength());

	char nSongerName[MaxRoleNameLength*2] = {0};
	mysql_escape_string(nSongerName, reqbody.strSongerName.GetString(), reqbody.strSongerName.GetStringLength());

	char nRequestName[MaxRoleNameLength*2] = {0};
	mysql_escape_string(nRequestName, reqbody.strRequestName.GetString(), reqbody.strRequestName.GetStringLength());

	char nWishwords[MaxWishWordsLength*2] = {0};
	mysql_escape_string(nWishwords, reqbody.strWishWords.GetString(), reqbody.strWishWords.GetStringLength());

	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "insert into vdc_requestsong.order_list(`TransID`,`RoomID`,`OrderStartTime`,`OrderEndTime`,"
			"`OrderStatus`,`SongName`,`SongerRoleID`,`SongerName`,`RequestRoleID`, `RequestName`, `WishWords`, `OrderCost`,`SongCount`) "
			"values('%s', %d, %ld, %ld, %d, '%s', %d, '%s', %d, '%s', '%s', %d, %d)"
			" on duplicate key update `OrderEndTime`=%ld , `OrderStatus`=%d, `TotalTicketsCount`=%d, "
			"`RequesterGoodCount`=%d, `RequesterBadCount`=%d, `RequesterGiveupCount`=%d, `GerneralGoodCount`=%d, "
			"`GerneralBadCount`=%d, `GerneralGiveupCount`=%d, `MarkResult`=%d",
			reqbody.strTransID.GetString(),
			reqbody.nRoomID,
			reqbody.nOrderStartTime,
			reqbody.nOrderEndTime,
			reqbody.nOrderStatus,
			nSongName,
			reqbody.nSongerRoleID,
			nSongerName,
			reqbody.nRequestRoleID,
			nRequestName,
			nWishwords,
			reqbody.nOrderCost,
			reqbody.nSongCount,
			reqbody.nOrderEndTime,
			reqbody.nOrderStatus,
			reqbody.nTotalTicketCount,
			reqbody.nRequesterGoodCount,
			reqbody.nRequesterBadCount,
			reqbody.nRequesterGiveupCount,
			reqbody.nGerneralGoodCount,
			reqbody.nGerneralBadCount,
			reqbody.nGerneralGiveupCount,
			reqbody.nMarkResult
	);

	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("insert song order  failed! errorcode=0x%08X, sql=%s\n",
			ret, szSql);
		return ret;
	}
	if (reqbody.nOrderStatus == enmOrderStatus_Accepted)
	{
		//memset(szSql, 0, sizeof(szSql));
		if(reqbody.nSongCount > 1)
		{
			sprintf(szSql, "insert into vdc_artist.artist_account(`RoleID`,`Flowers`, `PropsIns`)"
					"values(%d, 0, 1) on duplicate key update `PropsIns`= `PropsIns` + %d", reqbody.nSongerRoleID, reqbody.nSongCount);
		} else {
			sprintf(szSql, "insert into vdc_artist.artist_account(`RoleID`,`Flowers`, `Props`)"
					"values(%d, 0, 1) on duplicate key update `Props`= `Props` + 1", reqbody.nSongerRoleID);
		}
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("update artist props  failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
			return ret;
		}
		//memset(szSql, 0, sizeof(szSql));
		sprintf(szSql,"select RoleID from vdc_room.room_member where `RoomID`=%d and RoleRank=%d",reqbody.nRoomID,enmRoleRank_Host);
		//memset(arrRecordSet, 0, sizeof(arrRecordSet));
		ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("query room host failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
			return ret;
		}
		if(0 == nRowCount)
		{
			WRITE_WARNING_LOG("query  NULL!, sql=%s\n",szSql);
			return S_OK;
		}
		int32_t nHostRoleID = (int32_t)atoi(arrRecordSet[0]);
		//memset(szSql, 0, sizeof(szSql));
		if(reqbody.nSongCount > 1)
		{
			sprintf(szSql, "insert into vdc_roomlord.account(`RoleID`,`Flowers`, `PropsIns`)"
					"values(%d, 0, 1) on duplicate key update `PropsIns`= `PropsIns` + %d", nHostRoleID, reqbody.nSongCount);
		} else {
			sprintf(szSql, "insert into vdc_roomlord.account(`RoleID`,`Flowers`, `Props`)"
					"values(%d, 0, 1) on duplicate key update `Props`= `Props` + 1", nHostRoleID);
		}
		ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("update room host props failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
			return ret;
		}
	}
	return S_OK;
}


FRAME_DBPROXY_NAMESPACE_END
