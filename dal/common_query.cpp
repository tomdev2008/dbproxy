/*
 * common_query.cpp
 *
 *  Created on: 2013-2-28
 *      Author: liufl
 */

#include "common_query.h"

FRAME_DBPROXY_NAMESPACE_BEGIN


int32_t QueryRoleBaseInfo(const RoleID roleid, RoleBaseInfo& rolebaseinfo)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `RoleID`,"
			"`RoleName`,"
			"`AccountName`,"
			"`AccountID`,"
			"`Gender`,"
			"`UserLevel`,"
			"`VipLevel`,"
			"`RoleIDentity`,"
			"`Money`,"
			"`CreateTime`,"
			"`LastLoginTime`,"
			"`LastLogoutTime`,"
			"`Birthday`,"
			"`HometownProvince`,"
			"`HometownCity`,"
			"`HometownCounty`,"
			"`CurLocateAddrProvince`,"
			"`CurLocateAddrCity`,"
			"`CurLocateAddrCounty`,"
			"`Experience`,"
			"`LastLoginIP`,"
			"`LoginIP`,"
			"`HoroScope`,"
			"`Email`,"
			"`Telephone`,"
			"`QQ`,"
			"`bActiveEmail`,"
			"`bActiveTele`,"
			"`BecomeVipTime`,"
			"`ExpireVipTime`,"
			"`RechargeStatus`,"
			"`PwdProStatus`,"
			"`LoginTimes`, "
			"`MagnateLevel`, "
			"`ConsumeAmt`, "
			"`NextLevelConsume`"
			"from vdc_user.user_base_info where `RoleID` = %d", roleid);
	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "Error: DB operation failed on query role baseinfo for roleid=%d, sql=%s\n",
				roleid, szSql);
		return ret;
	}
	WRITE_DEBUG_LOG( "db operation success on query role baseinfo for roleid=%d, sql=%s\n",
			roleid, szSql);

	if(0 == nRowCount)
	{
		WRITE_ERROR_LOG("no such a role in db, rolied=%d\n", roleid);

		memset(&rolebaseinfo, 0, sizeof(rolebaseinfo));
		rolebaseinfo.nRoleID = roleid;
		memset(rolebaseinfo.szRoleName, 0, sizeof(rolebaseinfo.szRoleName));
		memset(rolebaseinfo.szAccountName, 0, sizeof(rolebaseinfo.szAccountName));
		rolebaseinfo.nAccountID = enmInvalidAccountID;
		rolebaseinfo.ucGender = enmGender_Unisex;
		rolebaseinfo.ucUserLevel = 0;
		rolebaseinfo.ucVipLevel = 0;
		rolebaseinfo.uIdentity = enmIdentityType_None;
		rolebaseinfo.nMoney = 0;
		memset(rolebaseinfo.szCreateTime, 0, sizeof(rolebaseinfo.szCreateTime));
		rolebaseinfo.nLastLoginTime = 0;
		rolebaseinfo.nLastLogoutTime = 0;
		rolebaseinfo.nBirthday = 19000101;
		memset(rolebaseinfo.szHometownProvince, 0, sizeof(rolebaseinfo.szHometownProvince));
		memset(rolebaseinfo.szHometownCity, 0, sizeof(rolebaseinfo.szHometownCity));
		memset(rolebaseinfo.szHometownCounty, 0, sizeof(rolebaseinfo.szHometownCounty));
		memset(rolebaseinfo.szCurAddrProvince, 0, sizeof(rolebaseinfo.szCurAddrProvince));
		memset(rolebaseinfo.szCurAddrCity, 0, sizeof(rolebaseinfo.szCurAddrCity));
		memset(rolebaseinfo.szCurAddrCounty, 0, sizeof(rolebaseinfo.szCurAddrCounty));
		rolebaseinfo.nExperience = 0;
		memset(rolebaseinfo.szLastLoginIP, 0, sizeof(rolebaseinfo.szLastLoginIP));
		memset(rolebaseinfo.szLoginIP, 0, sizeof(rolebaseinfo.szLoginIP));
		memset(rolebaseinfo.szHoroScope, 0, sizeof(rolebaseinfo.szHoroScope));
		memset(rolebaseinfo.szEmail, 0,sizeof(rolebaseinfo.szEmail));
		memset(rolebaseinfo.szQQ, 0,sizeof(rolebaseinfo.szQQ));
		rolebaseinfo.nActiveEmail = 0;
		rolebaseinfo.nActiveTele = 0;
		rolebaseinfo.nBecomeVipTime = 0;
		rolebaseinfo.nExpireVipTime = 0;
		rolebaseinfo.nRmbUserType = 0;
		rolebaseinfo.nAccountSafeType = 0;
		rolebaseinfo.nLoginTimes = 0;
		rolebaseinfo.nMagnateLevel = 0;
		rolebaseinfo.nConsume = 0;
		rolebaseinfo.nNextLevelConusme = 0;
		return S_OK;
	}

	rolebaseinfo.nRoleID = (RoleID)atoi(arrRecordSet[enmUserBaseInfo_RoleID]);
	strcpy_safe(rolebaseinfo.szRoleName, enmMaxRoleNameLength, arrRecordSet[enmUserBaseInfo_RoleName], strlen(arrRecordSet[enmUserBaseInfo_RoleName]));
	strcpy_safe(rolebaseinfo.szAccountName, MaxAccountName, arrRecordSet[enmUserBaseInfo_AccountName], strlen(arrRecordSet[enmUserBaseInfo_AccountName]));
	rolebaseinfo.nAccountID = (AccountID)atoi(arrRecordSet[enmUserBaseInfo_AccountID]);
	rolebaseinfo.ucGender = (Gender)atoi(arrRecordSet[enmUserBaseInfo_Gender]);
	rolebaseinfo.ucUserLevel = (UserLevel)atoi(arrRecordSet[enmUserBaseInfo_UserLevel]);
	rolebaseinfo.ucVipLevel = (VipLevel)atoi(arrRecordSet[enmUserBaseInfo_VipLevel]);
	rolebaseinfo.uIdentity = (IdentityType)atoi(arrRecordSet[enmUserBaseInfo_Identity]);
	rolebaseinfo.nMoney = atoi(arrRecordSet[enmUserBaseInfo_Money]);
	strcpy_safe(rolebaseinfo.szCreateTime, enmMaxTimeStringLength, arrRecordSet[enmUserBaseInfo_CreateTime], strlen(arrRecordSet[enmUserBaseInfo_CreateTime]));
	rolebaseinfo.nLastLoginTime = atoi(arrRecordSet[enmUserBaseInfo_LastLoginTime]);
	rolebaseinfo.nLastLogoutTime = atoi(arrRecordSet[enmUserBaseInfo_LastLogoutTime]);
	rolebaseinfo.nBirthday = atoi(arrRecordSet[enmUserBaseInfo_Birthday]);
	strcpy_safe(rolebaseinfo.szHometownProvince, enmMaxProvinceNameLength, arrRecordSet[enmUserBaseInfo_HometownProvince], strlen(arrRecordSet[enmUserBaseInfo_HometownProvince]));
	strcpy_safe(rolebaseinfo.szHometownCity, enmMaxCityNameLength, arrRecordSet[enmUserBaseInfo_HometownCity], strlen(arrRecordSet[enmUserBaseInfo_HometownCity]));
	strcpy_safe(rolebaseinfo.szHometownCounty, enmMaxCountyNameLength, arrRecordSet[enmUserBaseInfo_HometownCounty], strlen(arrRecordSet[enmUserBaseInfo_HometownCounty]));
	strcpy_safe(rolebaseinfo.szCurAddrProvince, enmMaxProvinceNameLength, arrRecordSet[enmUserBaseInfo_CurAddrProvince], strlen(arrRecordSet[enmUserBaseInfo_CurAddrProvince]));
	strcpy_safe(rolebaseinfo.szCurAddrCity, enmMaxCityNameLength, arrRecordSet[enmUserBaseInfo_CurAddrCity], strlen(arrRecordSet[enmUserBaseInfo_CurAddrCity]));
	strcpy_safe(rolebaseinfo.szCurAddrCounty, enmMaxCountyNameLength, arrRecordSet[enmUserBaseInfo_CurAddrCounty], strlen(arrRecordSet[enmUserBaseInfo_CurAddrCounty]));
	rolebaseinfo.nExperience = atoi(arrRecordSet[enmUserBaseInfo_Experince]);
	strcpy_safe(rolebaseinfo.szLastLoginIP, enmMaxIPAddressLength, arrRecordSet[enmUserBaseInfo_LastLoginIP], strlen(arrRecordSet[enmUserBaseInfo_LastLoginIP]));
	strcpy_safe(rolebaseinfo.szLoginIP, enmMaxIPAddressLength, arrRecordSet[enmUserBaseInfo_LoginIP], strlen(arrRecordSet[enmUserBaseInfo_LoginIP]));
	strcpy_safe(rolebaseinfo.szHoroScope, enmMaxHoroScopeNameLength, arrRecordSet[enmUserBaseInfo_HoroScope], strlen(arrRecordSet[enmUserBaseInfo_HoroScope]));
	strcpy_safe(rolebaseinfo.szEmail, enmMaxEmailAddrLength, arrRecordSet[enmUserBaseInfo_Email], strlen(arrRecordSet[enmUserBaseInfo_Email]));
	strcpy_safe(rolebaseinfo.szTelephone, enmMaxTelephoneLength, arrRecordSet[enmUserBaseInfo_Tele], strlen(arrRecordSet[enmUserBaseInfo_Tele]));
	strcpy_safe(rolebaseinfo.szQQ, enmMaxQQLength, arrRecordSet[enmUserBaseInfo_QQ], strlen(arrRecordSet[enmUserBaseInfo_QQ]));

	rolebaseinfo.nActiveEmail = atoi(arrRecordSet[enmUserBaseInfo_ActiveEamil]);
	rolebaseinfo.nActiveTele = atoi(arrRecordSet[enmUserBaseInfo_ActiveTele]);
	rolebaseinfo.nBecomeVipTime = (uint64_t)atoi(arrRecordSet[enmUserBaseInfo_BecomeVipTime]);
	rolebaseinfo.nExpireVipTime = (uint64_t)atoi(arrRecordSet[enmUserBaseInfo_ExpireVipTime]);
	rolebaseinfo.nRmbUserType = (int32_t)atoi(arrRecordSet[enmUserBaseInfo_RmbUserType]);
	rolebaseinfo.nAccountSafeType = (int32_t)atoi(arrRecordSet[enmUserBaseInfo_AccountSafeType]);
	rolebaseinfo.nLoginTimes = (int32_t)atoi(arrRecordSet[enmUserBaseInfo_LoginTimes]);
	rolebaseinfo.nMagnateLevel = (MagnateLevel)atoi(arrRecordSet[enmUserBaseInfo_MagnateLevel]);
	rolebaseinfo.nConsume = (uint64_t)atol(arrRecordSet[enmUserBaseInfo_Consume]);
	rolebaseinfo.nNextLevelConusme = (uint64_t)atol(arrRecordSet[enmUserBaseInfo_NextLevelConsume]);

	return S_OK;
}

int32_t QueryRole179ID(const RoleID roleid, uint32_t& n179id)
{
	if(0 > roleid)
	{
		WRITE_ERROR_LOG("roleid(179uin) on query user 179id is invalid! roleid=%d\n", roleid);
		return E_INVALID_ARGUMENTS;
	}
	WRITE_DEBUG_LOG("roleid(179uin) on query user 179id is right! roleid=%d\n", roleid);

	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `179ID` from vdc_user.user_account where `179Uin`=%d", roleid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG("db operation on query role 179id failed! errorcode=0x%08X, roleid(179uin)=%d, sql=%s\n",
				ret, roleid, szSql);
		return ret;
	}
	WRITE_DEBUG_LOG("db operation on query role 179id success! roleid=%d, sql=%s\n",
			roleid, szSql);

	if(nRowCount == 0)
	{
		WRITE_WARNING_LOG("unknown error happened while query user 179id! roleid=%d\n", roleid);
		n179id = 0;
		return S_OK;
	}
	n179id = (uint32_t)atoi(arrRecordSet[0]);
	WRITE_DEBUG_LOG("db operation on query user 179id success! roleid=%d, 179id=%d\n", roleid, n179id);

	return S_OK;
}

int32_t QueryRoomBaseInfo(const RoomID roomid, RoomBaseInfo& roombaseinfo)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select  "
			"`RoomID`,"
			"`RoomName`,"
			"`RoomType`,"
			"`ChannelType`,"
			"`ChannelID`,"
			"`ChannelName`,"
			"`RegionType`,"
			"`RegionID`,"
			"`RegionName`,"
			"`RoomShowCapacity`,"
			"`RoomRealCapacity`,"
			"`RoomFund`,"
			"`Founder`,"
			"`Owner`,"
			"`RoomLevel`,"
			"`CreateTime`,"
			"`TempNotice`,"
			"`RoomNotice`,"
			"`RoomTheme`,"
			"`Password`,"
			"`Option`,"
			"`MicroTime`,"
			"`RechargeNotice`,"
			"`WelcomeWords`,"
			"`RobotPercent`,"
			"`RobotColorBarProb` "
			"from vdc_room.room_base_info where `RoomID` = %d", roomid);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "Error: DB operation failed on query room base info for roomid=%d!, sql=%s\n",
				roomid, szSql);
		return ret;
	}
	WRITE_DEBUG_LOG( "db operation success on query role asset for roomid=%d, sql=%s\n",
			roomid, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "warning: this room is not found in db! roomid=%d\n", roomid);
		memset(&roombaseinfo, 0, sizeof(roombaseinfo));
		return E_NOT_EXIST_IN_DB;
	}
	WRITE_DEBUG_LOG( "this room is  existed in db! room id is %d\n", roomid);

	roombaseinfo.uRoomID = atoi(arrRecordSet[enmRoomBaseInfo_roomid]);
	strcpy_safe(roombaseinfo.szRoomName, MaxRoomNameLength, arrRecordSet[enmRoomBaseInfo_roomname], strlen(arrRecordSet[enmRoomBaseInfo_roomname]));
	roombaseinfo.ucRoomType = (RoomType)atoi(arrRecordSet[enmRoomBaseInfo_roomtype]);
	roombaseinfo.ucChannelType = (ChannelType)atoi(arrRecordSet[enmRoomBaseInfo_roomchanneltype]);
	roombaseinfo.usChannelID = (ChannelID)atoi(arrRecordSet[enmRoomBaseInfo_roomchannelid]);
	strcpy_safe(roombaseinfo.szChannelName, MaxChannelNameLength, arrRecordSet[enmRoomBaseInfo_roomchannelname], strlen(arrRecordSet[enmRoomBaseInfo_roomchannelname]));
	roombaseinfo.ucRegionType = (RegionType)atoi(arrRecordSet[enmRoomBaseInfo_roomregiontype]);
	roombaseinfo.usRegionID = (RegionID)atoi(arrRecordSet[enmRoomBaseInfo_roomregionid]);
	strcpy_safe(roombaseinfo.szRegionName, MaxRegionNameLength, arrRecordSet[enmRoomBaseInfo_roomregionname], strlen(arrRecordSet[enmRoomBaseInfo_roomregionname]));
	roombaseinfo.nRoomShowCapacity = atoi(arrRecordSet[enmRoomBaseInfo_roomshowcapacity]);
	roombaseinfo.nRoomRealCapacity = atoi(arrRecordSet[enmRoomBaseInfo_roomrealcapacity]);
	roombaseinfo.nRoomFund = atoi(arrRecordSet[enmRoomBaseInfo_roomfund]);
	roombaseinfo.nRoomFounderID = atoi(arrRecordSet[enmRoomBaseInfo_roomfounderid]);
	roombaseinfo.nOwner = atoi(arrRecordSet[enmRoomBaseInfo_roomowner]);
	roombaseinfo.nRoomLevel = atoi(arrRecordSet[enmRoomBaseInfo_roomlevel]);
	strcpy_safe(roombaseinfo.szRoomCreateTime, enmMaxTimeStringLength, arrRecordSet[enmRoomBaseInfo_roomcreatetime], strlen(arrRecordSet[enmRoomBaseInfo_roomcreatetime]));
	strcpy_safe(roombaseinfo.szTempNotice, MaxRoomNoticeLength, arrRecordSet[enmRoomBaseInfo_roomtmpnotice], strlen(arrRecordSet[enmRoomBaseInfo_roomtmpnotice]));
	strcpy_safe(roombaseinfo.szRoomNotice, MaxRoomNoticeLength, arrRecordSet[enmRoomBaseInfo_roomnotice], strlen(arrRecordSet[enmRoomBaseInfo_roomnotice]));
	strcpy_safe(roombaseinfo.szRoomTheme, MaxRoomSignatureLength, arrRecordSet[enmRoomBaseInfo_roomtheme], strlen(arrRecordSet[enmRoomBaseInfo_roomtheme]));
	strcpy_safe(roombaseinfo.szRoomPassword, MaxRoomPasswordLength, arrRecordSet[enmRoomBaseInfo_roompassword], strlen(arrRecordSet[enmRoomBaseInfo_roompassword]));
	roombaseinfo.nOption = atoi(arrRecordSet[enmRoomBaseInfo_roomoption]);
	roombaseinfo.nMicroTime = atoi(arrRecordSet[enmRoomBaseInfo_roommicrotime]);
	strcpy_safe(roombaseinfo.szRechargeNotice, MaxRoomNoticeLength, arrRecordSet[enmRoomBaseInfo_roomrechargenotice], strlen(arrRecordSet[enmRoomBaseInfo_roomrechargenotice]));
	strcpy_safe(roombaseinfo.szWelcomewords, MaxRoomWelcomeLength, arrRecordSet[enmRoomBaseInfo_roomwelcomewords], strlen(arrRecordSet[enmRoomBaseInfo_roomwelcomewords]));
	roombaseinfo.nRobotPercent = atoi(arrRecordSet[enmRoomBaseInfo_RobotPercent]); //机器人比例
	roombaseinfo.nSendPercent = atoi(arrRecordSet[enmRoomBaseInfo_SendPercent]); //机器人发送彩条的概率

	return S_OK;
}

int32_t QueryRoomHostRoleID(const RoomID roomid, RoleID& roleid)
{
	char szSql[enmMaxSqlStringLength] = {0};
	sprintf(szSql, "select `RoleID` from vdc_room.room_member where `RoomID`=%d and `RoleRank`=%d", roomid, enmRoleRank_Host);

	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	memset(arrRecordSet, 0, sizeof(arrRecordSet));

	int32_t ret = S_OK;
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "error: db opertion on query room host failed! roomid=%d, rolerank=%d, errorcode=0x%08X, sql=%s\n",
				roomid, enmRoleRank_Host, ret, szSql);
		return ret;
	}
	WRITE_DEBUG_LOG( "db operation on query room host success! roomid=%d, rolerank=%d, sql=%s\n",
			roomid, enmRoleRank_Host, szSql);

	if(0 == nRowCount)
	{
		WRITE_WARNING_LOG( "warning: this room has no host! roomid=%d\n", roomid);
		roleid = enmInvalidRoleID;
		return S_OK;
	}
	roleid = (RoleID)atoi(arrRecordSet[0]);
	WRITE_DEBUG_LOG( "the host of this room is: %d, roomid=%d\n", roleid, roomid);

	return S_OK;
}

FRAME_DBPROXY_NAMESPACE_END
