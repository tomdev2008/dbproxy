/*
 * bll_event_roomadd_msg.cpp
 *
 *  Created on: 2013-3-6
 *      Author: liufl
 */

#include "bll_event_roomadd_msg.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

int32_t CFromRoomAddEvent:: OnMessage_AddBlacklist(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData )
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver add black list!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CAddPlayerToBlackReq* pTmpBody = dynamic_cast<CAddPlayerToBlackReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CAddPlayerToBlackResp set_rolerank;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBRS_ADD_BLACK_RESP);

	set_rolerank.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &set_rolerank, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &set_rolerank, nOptionLen, (const char*)pOptionData);

	//时间类
	CDateTime dt = CDateTime::CurrentDateTime();
	char szDateTime[enmMaxTimeStringLength] = {0};
	dt.ToDateTimeString(szDateTime);


	ret = InsertPlayerIntoRoomBlackList(pTmpBody->nRoleID, pTmpBody->nRoomID, szDateTime, pMsgHead->nRoleID);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process roomserver add black list request failed! errorcode=0x%08X\n",
				ret);

		return ret;
	}
	WRITE_DEBUG_LOG( "process roomserver add black success! \n");


	return S_OK;
}
int32_t CFromRoomAddEvent::InsertPlayerIntoRoomBlackList(const RoleID roleid, const RoomID roomid, const char* szDateTime, const RoleID operatorID)
{
	//生成SQL语句
	char szSql[enmMaxSqlStringLength] = {0};

	//sprintf(szSql, "insert into player_historyonlinetimebystage (`RoleID`,`StartTime`,`EndTime`,`TotalTime`) values (%d,%d,%d,%d) on duplicate key update `TotalTime` = %d", roleID, starttime,endtime, nowtotaltime,nowtotaltime);

	sprintf(szSql, "insert into vdc_room.room_black_list(`RoomID`, `RoleID`,`AdderID`, `JoinTime`) values (%d, %d, %d, '%s') on duplicate key update `AdderID`=%d, `JoinTime` = '%s'" ,
			roomid, roleid, operatorID, szDateTime, operatorID, szDateTime);

	uint64_t nAffectedRows = 0;
	int32_t nRet = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if (0 > nRet)
	{
		WRITE_ERROR_LOG( "execute update room black list failed! roomid=%d, roleid=%d, errorcode=0x%08X,  sql=%s\n",
				roomid, roleid, nRet,  szSql);
		return nRet;
	}
	//WRITE_DEBUG_LOG( "execute update room black list success!  roomid=%d, roleid=%d, sql=%s\n",roomid, roleid, szSql);

	if ( (1 != nAffectedRows) && (2!= nAffectedRows) )
	{
		nRet = E_UNKNOWN;
	}

	return S_OK;
}

int32_t CFromRoomAddEvent::OnMessage_AddInRoomTime(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen , const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver add in roomtime!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CAddAdmInRoomOnLineTimeNotice* pTmpBody = dynamic_cast<CAddAdmInRoomOnLineTimeNotice*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	RoleRank rolerank = pTmpBody->nRoleRank;
	//时间类
	CDateTime dt = CDateTime::CurrentDateTime();
	char szDateTime[enmMaxTimeStringLength] = {0};
	dt.ToDateTimeString(szDateTime);

	//更新管理员在房间时长日表
	ret = UpdateAdminInRoomOnlineTimeDay(*pTmpBody, rolerank);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update admin in room online time failed, operation time: %s, roleid=%d, roomid=%d, addtime=%d,  msgid=0x%08X\n",
				szDateTime,
				pTmpBody->nRoleID,
				pTmpBody->nRoomID,
				pTmpBody->nAddTime,

				pMsgHead->nMessageID);
		return ret;
	}
//	WRITE_DEBUG_LOG( "update admin in room online time success! operation time: %s, roleid=%d, roomid=%d, addtime=%d, msgid=0x%08X\n",
//			szDateTime,
//			pTmpBody->nRoleID,
//			pTmpBody->nRoomID,
//			pTmpBody->nAddTime,
//			pMsgHead->nMessageID);

	//更新管理员在房间时长总表（更新两张表， 一张跟房间号相关， 一张无关）
	ret = UpdateAdminInRoomOnlineTimeTotal(pTmpBody->nRoleID, pTmpBody->nRoomID, pTmpBody->nAddTime, rolerank);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "add admin inroom online total time failed! errorcode=0x%08X\n", ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "add admin inroom online total time success!\n");

	return S_OK;
}
int32_t CFromRoomAddEvent::UpdateAdminInRoomOnlineTimeDay(const CAddAdmInRoomOnLineTimeNotice& msgbody, const RoleRank rolerank)
{
	if(0 >= msgbody.nRoomID || 0 > msgbody.nRoleID)
	{
		WRITE_ERROR_LOG( "argument invalid while update admin inroom online time! roleid=%d, roomid=%d, year=%d, month=%d, day=%d, addtime=%d\n",
				msgbody.nRoomID,
				msgbody.nRoleID,
				msgbody.nYear,
				msgbody.nMonth,
				msgbody.nDay,
				msgbody.nAddTime);
		return E_INVALID_ARGUMENTS;
	}

	int32_t ret = S_OK;

	char szSql[enmMaxSqlStringLength] = {0};

	//不满足管理条件，只更新InRoomTimeTotal字段
	if(enmRoleRank_TempAdmin >= rolerank || enmRoleRank_Super <= rolerank)
	{
		RoleRank writen_rolerank = 0;
		WRITE_NOTICE_LOG( "the rolerank is below tempAdmin or above super amdin! just write zero into db! roomid=%d, roleid=%d, rolerank=%d\n",
				msgbody.nRoomID, msgbody.nRoleID, rolerank);

		sprintf(szSql, "insert into vdc_time.`user_room_time_%04d_%02d_%02d`(`RoleID`, `RoomID`, `RoleRank`, `InRoomTimeTotal`) values(%d, %d, %d, %d)  \
						   on duplicate key update `InRoomTimeTotal`=`InRoomTimeTotal`+%d, `RoleRank`=%d",
				msgbody.nYear,
				msgbody.nMonth,
				msgbody.nDay,
				msgbody.nRoleID,
				msgbody.nRoomID,
				writen_rolerank,
				msgbody.nAddTime,
				msgbody.nAddTime,
				writen_rolerank);
	}
	else //满足管理条件， 同时更新 InRoomTime, InRoomTimeTotal两个字段
	{
		sprintf(szSql, "insert into vdc_time.user_room_time_%04d_%02d_%02d(`RoleID`, `RoomID`, `RoleRank`, `InRoomTime`, `InRoomTimeTotal`) "
				"values(%d, %d, %d, %d, %d) "
				"on duplicate key update `InRoomTime`=`InRoomTime` + %d, `RoleRank`= %d, `InRoomTimeTotal`=`InRoomTimeTotal` + %d",
				msgbody.nYear,
				msgbody.nMonth,
				msgbody.nDay,
				msgbody.nRoleID,
				msgbody.nRoomID,
				msgbody.nRoleRank,
				msgbody.nAddTime,
				msgbody.nAddTime,
				msgbody.nAddTime,
				msgbody.nRoleRank,
				msgbody.nAddTime);
	}

	uint64_t nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "Error: execute update on update room admin in room time day table in room time total failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "execute update on update player in room time InRoomTimeTotal success!, sql=%s\n", szSql);

	if(1 != nAffectedRows && 2 != nAffectedRows)
	{
		WRITE_ERROR_LOG( "Error: execute update admin inroom online time! unknown error! affectedrows=%d, sql=%s\n",
				nAffectedRows, szSql);
		return E_UNKNOWN;
	}
	//WRITE_DEBUG_LOG( "execute update admin inroom online time day table success!, sql=%s\n", szSql);

	return S_OK;
}
int32_t CFromRoomAddEvent::UpdateAdminInRoomOnlineTimeTotal(const RoleID roleid, const RoomID roomid, const uint32_t addtime, const RoleRank rolerank)
{
	if(0 > roleid || 0 > roomid || 0 > addtime)
	{
		WRITE_ERROR_LOG( "invalid argument on update admin inroom online time total! roleid=%d, roomid=%d, addtime=%d\n",
				roleid, roomid, addtime);
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "argument on update admin inroom online total all right!\n");

	//更新 user_room_time_total表
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};

	//memset(szSql, 0, sizeof(szSql));

	//不满足管理员条件， 只更新 InRoomTimeTotal
	if(enmRoleRank_TempAdmin >= rolerank || enmRoleRank_Super <= rolerank)
	{
		RoleRank writen_rolerank = 0;
		WRITE_NOTICE_LOG( "the rolerank is below tempAdmin or above super amdin! just write zero into db! roomid=%d, roleid=%d, rolerank=%d\n",
				roomid, roleid, rolerank);

		//更新room_member表里面的总共在房间的时长，没有就新增，有就更新TotalTimeInRoom字段
		sprintf(szSql, "insert into vdc_time.`user_room_time_total`(`RoleID`, `RoomID`, `RoleRank`, `InRoomTimeTotal`) values(%d, %d, %d, %d)  \
					   on duplicate key update `InRoomTimeTotal`=`InRoomTimeTotal` + %d, `RoleRank`=%d",
				roleid, roomid, writen_rolerank, addtime, addtime, writen_rolerank);
	}
	else//满足管理条件， 同时修改InRoomTime, InRoomTimeTotal
	{
		sprintf(szSql, "insert into vdc_time.user_room_time_total(RoleID, RoomID, RoleRank, InRoomTime, InRoomTimeTotal) "
				"values(%d, %d, %d, %d, %d) "
				"on duplicate key update `RoleRank`=%d, `InRoomTime`=`InRoomTime`+%d, `InRoomTimeTotal`=`InRoomTimeTotal`+%d",
				roleid, roomid, rolerank, addtime, addtime, rolerank, addtime, addtime);
	}

	uint64_t nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "add admin inroom online time failed! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "add player InRoomTime success!, sql=%s\n", szSql);

	if(1 != nAffectedRows && 2 != nAffectedRows)
	{
		WRITE_ERROR_LOG( "unknown error occured on update admin inroom online time! affectedrows=%d, sql=%s\n",
				nAffectedRows, szSql);
		return E_UNKNOWN;
	}
	//WRITE_DEBUG_LOG( "add admin inroom online time success! sql=%s\n", szSql);

	//更新user_totaltime表
	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "insert into vdc_time.`user_totaltime`(`RoleID`, `TotalInRoomTime`) values(%d, %d) on duplicate key update `TotalInRoomTime`=`TotalInRoomTime` + %d",
			roleid, addtime, addtime);
	nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "add user totaltime failed!  errorcode=0x%08X, roleid=%d, sql=%s\n",
				ret, roleid, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "add user_totaltime success!, sql=%s\n", szSql);

	if(1 != nAffectedRows && 2 != nAffectedRows)
	{
		WRITE_ERROR_LOG( "unknown error happens while update user totaltime table! affectedrows=%d, roleid=%d, sql=%s\n",
				nAffectedRows, roleid, szSql);
		return ret;
	}

	//WRITE_DEBUG_LOG( "add user total time success! roleid=%d, sql=%s\n", roleid, szSql);

	return S_OK;
}

int32_t CFromRoomAddEvent::OnMessage_AddOnMike(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: roomserver add in roomtime!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CAddOnMicTimeNotice* pTmpBody = dynamic_cast<CAddOnMicTimeNotice*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	RoleRank rolerank = 0;
	ret = QueryRoleRankInRoom(pTmpBody->nRoleID, pTmpBody->nRoomID, rolerank);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query rolerank in room failed! roleid=%d, roomid=%d, errorcode=0x%08X\n",
				pTmpBody->nRoleID, pTmpBody->nRoomID, ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "query rolerank in room success! roleid=%d, roomid=%d\n",pTmpBody->nRoleID, pTmpBody->nRoomID);


	//更新user_inroom_onmike_day日表
	ret = UpdateRoleOnMikeDay(*pTmpBody, rolerank);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update user onmike time day table failed! errorcode=0x%08X, roleid=%d\n",
				ret, pTmpBody->nRoleID, pTmpBody->nRoomID);
		return ret;
	}
	//WRITE_DEBUG_LOG( "update user onmike time day table success! roleid=%d, roomid=%d\n",pTmpBody->nRoleID, pTmpBody->nRoomID);

	//更新房间时间相关总表(user_room_time_total表及user_totaltime表，同时更新两个表)
	ret = UpdateRoleOnMikeTotal(pTmpBody->nRoomID, pTmpBody->nRoleID, pTmpBody->nAddTime, rolerank);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "update user onmike time total failed! errorcode=0x%08X, roomid=%d, roleid=%d\n",
				ret, pTmpBody->nRoomID, pTmpBody->nRoleID);
	}
	//WRITE_DEBUG_LOG( "update user onmike time total success! roleid=%d, roomid=%d\n",pTmpBody->nRoleID, pTmpBody->nRoomID);

	return S_OK;

}
int32_t CFromRoomAddEvent::QueryRoleRankInRoom(const RoleID roleid, const RoomID roomid, RoleRank& rolerank)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `RoleRank` from vdc_room.room_member where `RoomID`=%d and `RoleID`=%d", roomid, roleid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "error: db opertion on query rolerank in room failed! roomid=%d, roleid=%d, errorcode=0x%08X, sql=%s\n",
				roomid,
				roleid,
				ret,
				szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "db operation on query rolerank in room success! roomid=%d, roleid=%d, sql=%s\n",roomid,roleid,szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "this player is not the admin of this room! roomid=%d, roleid=%d\n",
				roomid,
				roleid);
		rolerank = enmRoleRank_None;
		return S_OK;
	}

	rolerank = (RoleRank)atoi(arrRecordSet[0]);
	//WRITE_DEBUG_LOG( "rolerank in room %d of roleid %d is %d\n", roomid, roleid, rolerank);

	return S_OK;
}
int32_t CFromRoomAddEvent::UpdateRoleOnMikeDay(const CAddOnMicTimeNotice& msgbody, const RoleRank rolerank)
{
	if(0 >= msgbody.nRoleID || 0 >= msgbody.nRoomID)
	{
		WRITE_ERROR_LOG( "argument is invalid on update role onmike time! roleid=%d, roomid=%d, year=%d, month=%d, day=%d, addtime=%d\n",
				msgbody.nRoleID,
				msgbody.nRoomID,
				msgbody.nYear,
				msgbody.nMonth,
				msgbody.nDay,
				msgbody.nAddTime);
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "argument no problem on update role onmike time notice!");

	int32_t ret = S_OK;

	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "insert into vdc_time.user_room_time_%04d_%02d_%02d(`RoleID`, `RoomID`, `RoleRank`, `OnMikeTime`)  values(%d, %d, %d, %d)  \
				   on duplicate key update `OnMikeTime`=`OnMikeTime` + %d, `RoleRank`=%d",
			msgbody.nYear,
			msgbody.nMonth,
			msgbody.nDay,
			msgbody.nRoleID,
			msgbody.nRoomID,
			rolerank,
			msgbody.nAddTime,
			msgbody.nAddTime,
			rolerank);

	uint64_t nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "Error: execute update on add user onmike time failed! errorcode=0x%08X, sql=%s\n",
				ret, szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "execute update on add user onmike time success! sql=%s\n", szSql);

	if(1 != nAffectedRows && 2 != nAffectedRows)
	{
		WRITE_ERROR_LOG( "unknown error occured while add user onmike time! sql=%s\n", szSql);
		return E_UNKNOWN;
	}
	//WRITE_DEBUG_LOG( "execute update on add role onmike time success! sql=%s\n", szSql);

	return S_OK;
}
int32_t CFromRoomAddEvent::UpdateRoleOnMikeTotal(const RoomID roomid, const RoleID roleid, const uint32_t addtime, const RoleRank rolerank)
{
	if(0 > roomid || 0 > roleid || 0 > addtime)
	{
		WRITE_ERROR_LOG( "invalid arguments on update role onmike total time! roomid=%d, roleid=%d, addtime=%d\n",
				roomid, roleid, addtime);
		return E_INVALID_ARGUMENTS;
	}
	//WRITE_DEBUG_LOG( "argument all right on update role onmike time!\n");

	//首先更新在某个房间的总的在麦时长
	int32_t ret = S_OK;
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "insert into vdc_time.`user_room_time_total`(`RoleID`, `RoomID`, `RoleRank`, `OnMikeTime`) values(%d, %d, %d, %d) "
			"on duplicate key update `OnMikeTime`=`OnMikeTime`+%d, `RoleRank`=%d",
			roleid, roomid, rolerank, addtime, addtime, rolerank);

	uint64_t nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: execute update on add user total onmike time in one room failure! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}

	if(1 != nAffectedRows && 2 != nAffectedRows)
	{
		WRITE_ERROR_LOG( "unknown error occured on add user total onmike in one room!\n");
		return E_UNKNOWN;
	}
	//WRITE_DEBUG_LOG( "execute update on add user total onmike time in one room success! sql=%s\n", szSql);

	//然后更新总的在麦时长（没有房间概念，所有房间的在麦时长加起来）
	//memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "insert into vdc_time.`user_totaltime`(`RoleID`, `TotalOnMikeTime`) values(%d, %d) on duplicate key update `TotalOnMikeTime`=`TotalOnMikeTime`+%d",
			roleid, addtime, addtime);

	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "Error: execute update on add user total onmike time failure! errorcode=0x%08X, sql=%s\n", ret, szSql);
		return ret;
	}

	if(1 != nAffectedRows && 2 != nAffectedRows)
	{
		WRITE_ERROR_LOG( "unknown error occured on add user total onmike time!\n");
		return E_UNKNOWN;
	}
	//WRITE_DEBUG_LOG( "execute update on add user total onmike time success! sql=%s\n", szSql);

	return S_OK;
}

FRAME_DBPROXY_NAMESPACE_END

