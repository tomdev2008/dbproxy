/***************************************************************
* filename: dal_player.cpp
* date:     2011-12-31 9:52
* author:   xiaohq
* purpose:  定义缓存中玩家信息，因为memcache中的键-值都是字符串
            因此，对结构进行编解码而得到字符串个人觉得是一个简洁
			明了的方法，同时也节约空间
* revision: not yet
* version:  1.0
****************************************************************/

#include "common/common_errordef.h"
#include "dal_player.h"
#include "common/common_codeengine.h"
#include "../def/def_dbproxy_errorcode.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

int32_t CVDCPlayerRoomInfo::VDCPlayerRoomInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const VDCPlayerRoomInfo& vdcPlayerRoomInfo)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Encode(buf,size, offset, vdcPlayerRoomInfo.nRoomID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcPlayerRoomInfo.nPlayerState);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcPlayerRoomInfo.nRoleRank);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcPlayerRoomInfo.nEnterRoomTime);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcPlayerRoomInfo.nTotalTime);
	if(0>ret)
	{
		return ret;
	}

	return S_OK;
}

int32_t CVDCPlayerRoomInfo::VDCPlayerRoomInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, VDCPlayerRoomInfo& vdcPlayerRoomInfo)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Decode(buf, size, offset, vdcPlayerRoomInfo.nRoomID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcPlayerRoomInfo.nPlayerState);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcPlayerRoomInfo.nRoleRank);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcPlayerRoomInfo.nEnterRoomTime);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcPlayerRoomInfo.nTotalTime);
	if(0>ret)
	{
		return ret;
	}

	return S_OK;
}

void CVDCPlayerRoomInfo::VDCPlayerRoomInfoDump(char* buf, const uint32_t size, uint32_t& offset, const VDCPlayerRoomInfo& vdcplayerRoomInfo)
{
	sprintf(buf + offset, "{\n roomid=%d,\n playerstate=%d,\n rolerand=%d,\n enterroomtime=%d,\n totaltime=%ld\n}", 
		vdcplayerRoomInfo.nRoomID,
		vdcplayerRoomInfo.nPlayerState,
		vdcplayerRoomInfo.nRoleRank,
		vdcplayerRoomInfo.nEnterRoomTime,
		vdcplayerRoomInfo.nTotalTime);
	offset = (uint32_t)strlen(buf);
}

int32_t CVDCPlayerItemInfo::VDCPlayerItemInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const VDCPlayerItemInfo& vdcItemInfo)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Encode(buf, size, offset, vdcItemInfo.nRoleID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcItemInfo.nItemID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcItemInfo.nUseMeans);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcItemInfo.nQuantity);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcItemInfo.nGetTime);
	if(0>ret)
	{
		return ret;
	}
	return S_OK;
}

int32_t CVDCPlayerItemInfo::VDCPlayerItemInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, VDCPlayerItemInfo& vdcItemInfo)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Decode(buf, size, offset, vdcItemInfo.nRoleID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcItemInfo.nItemID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcItemInfo.nUseMeans);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcItemInfo.nQuantity);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcItemInfo.nGetTime);
	if(0>ret)
	{
		return ret;
	}

	return S_OK;	
}

void CVDCPlayerItemInfo::VDCPlayerItemInfoDump(char* buf, const uint32_t size, uint32_t& offset, const VDCPlayerItemInfo& vdcItemInfo)
{
	sprintf(buf	 + offset, "{\n roleid=%d,\n itemid=%d,\n usemeans=%d,\n quantity=%d,\n gettime=%d\n}", 
		vdcItemInfo.nRoleID, 
		vdcItemInfo.nItemID,
		vdcItemInfo.nUseMeans,
		vdcItemInfo.nQuantity,
		vdcItemInfo.nGetTime);
	offset = (uint32_t)strlen(buf);
}

int32_t CVDCPlayerRechargeInfo::VDCPlayerRechargeInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const VDCPlayerRechargeInfo& vdcRechargeInfo)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Encode(buf, size, offset, vdcRechargeInfo.nRoleID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcRechargeInfo.nRechargeTimes);
	if(0>ret)
	{
		return ret;
	}
	//ret = CCodeEngine::Encode(buf, size, offset, vdcRechargeInfo.nLastRechargeTime);
	//if(0>ret)
	//{
	//	return ret;
	//}
	ret = CCodeEngine::Encode(buf, size, offset, vdcRechargeInfo.szLastRechargeTime, sizeof(vdcRechargeInfo.szLastRechargeTime));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcRechargeInfo.nLastRechargeAmount);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcRechargeInfo.nLastRechageChannel);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcRechargeInfo.nTotalRechargeAmount);
	if(0>ret)
	{
		return ret;
	}
	return S_OK;
}

int32_t CVDCPlayerRechargeInfo::VDCPlayerRechargeInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, VDCPlayerRechargeInfo& vdcRechargeInfo)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Decode(buf, size, offset, vdcRechargeInfo.nRoleID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcRechargeInfo.nRechargeTimes);
	if(0>ret)
	{
		return ret;
	}
	//ret = CCodeEngine::Decode(buf, size, offset, vdcRechargeInfo.nLastRechargeTime);
	//if(0>ret)
	//{
	//	return ret;
	//}
	ret = CCodeEngine::Decode(buf, size, offset, vdcRechargeInfo.szLastRechargeTime, sizeof(vdcRechargeInfo.szLastRechargeTime));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcRechargeInfo.nLastRechargeAmount);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcRechargeInfo.nLastRechageChannel);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcRechargeInfo.nTotalRechargeAmount);
	if(0>ret)
	{
		return ret;
	}
	return S_OK;
}

void CVDCPlayerRechargeInfo::VDCPlayerRechargeInfoDump(char* buf, const uint32_t size, uint32_t& offset, const VDCPlayerRechargeInfo& vdcRechargeInfo)
{
	sprintf(buf + offset, "{roleid=%d, rechargetimes=%d, lastRechargetime=%s, lastrechargeamount=%d, lastrechargechannle=%d, totalRechargeAmount=%d}\n", 
		vdcRechargeInfo.nRoleID, 
		vdcRechargeInfo.nRechargeTimes,
		vdcRechargeInfo.szLastRechargeTime,
		vdcRechargeInfo.nLastRechargeAmount,
		vdcRechargeInfo.nLastRechageChannel,
		vdcRechargeInfo.nTotalRechargeAmount
		);
	offset = (uint32_t)strlen(buf);
}

int32_t CVDCPlayerPrizeInfo::VDCPlayerPrizeInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const VDCPlayerPrizeInfo& vdcPrizeInfo)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Encode(buf, size, offset, vdcPrizeInfo.nRoleID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcPrizeInfo.nObtainPrizeTimes);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcPrizeInfo.szLastGetPrizeTime, sizeof(vdcPrizeInfo.szLastGetPrizeTime));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcPrizeInfo.nLastObtainPrizeAmount);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcPrizeInfo.nTotalObtainPrizeAmount);
	if(0>ret)
	{
		return ret;
	}
	return S_OK;
}

int32_t CVDCPlayerPrizeInfo::VDCPlayerPrizeInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, VDCPlayerPrizeInfo& vdcPrizeInfo)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Decode(buf, size, offset, vdcPrizeInfo.nRoleID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcPrizeInfo.nObtainPrizeTimes);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcPrizeInfo.szLastGetPrizeTime, sizeof(vdcPrizeInfo.szLastGetPrizeTime));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcPrizeInfo.nLastObtainPrizeAmount);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcPrizeInfo.nTotalObtainPrizeAmount);
	if(0>ret)
	{
		return ret;
	}
	return S_OK;
}

void CVDCPlayerPrizeInfo::VDCPlyaerPrizeInfoDump(char* buf, const uint32_t size, uint32_t& offset, const VDCPlayerPrizeInfo& vdcPrizeInfo)
{
	sprintf(buf + offset, "{roleid=%d, obtainPrizeTimes=%d, lastobtainPrizeTime=%s, lastobtainprizeAmount=%d, totalObtainPrizeCount=%d}\n", 
		vdcPrizeInfo.nRoleID, 
		vdcPrizeInfo.nObtainPrizeTimes,
		vdcPrizeInfo.szLastGetPrizeTime,
		vdcPrizeInfo.nLastObtainPrizeAmount,
		vdcPrizeInfo.nTotalObtainPrizeAmount);
	offset = (uint32_t)strlen(buf);
}

int32_t CVDCPlayerConsumeInfo::VDCPlayerConsumeInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const VDCPlayerConsumeInfo& vdcConsumeInfo)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Encode(buf, size, offset, vdcConsumeInfo.nRoleID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcConsumeInfo.nConsumeTimes);
	if(0>ret)
	{
		return ret;
	}
	//ret = CCodeEngine::Encode(buf, size, offset, vdcConsumeInfo.nLastConsumeTime);
	//if(0>ret)
	//{
	//	return ret;
	//}
	ret = CCodeEngine::Encode(buf, size, offset, vdcConsumeInfo.szLastConsumeTime, sizeof(vdcConsumeInfo.szLastConsumeTime));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcConsumeInfo.nLastConsumeAmount);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, vdcConsumeInfo.nTotalConsumed);
	if(0>ret)
	{
		return ret;
	}
	return S_OK;
}

int32_t CVDCPlayerConsumeInfo::VDCPlayerConsumeInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, VDCPlayerConsumeInfo& vdcConsumeInfo)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Decode(buf, size, offset, vdcConsumeInfo.nRoleID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcConsumeInfo.nConsumeTimes);
	if(0>ret)
	{
		return ret;
	}
	//ret = CCodeEngine::Decode(buf, size, offset, vdcConsumeInfo.nLastConsumeTime);
	//if(0>ret)
	//{
	//	return ret;
	//}
	ret = CCodeEngine::Decode(buf, size, offset, vdcConsumeInfo.szLastConsumeTime, sizeof(vdcConsumeInfo.szLastConsumeTime));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcConsumeInfo.nLastConsumeAmount);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, vdcConsumeInfo.nTotalConsumed);
	if(0>ret)
	{
		return ret;
	}
	return S_OK;
}

void CVDCPlayerConsumeInfo::VDCPlayerConsumeInfoDump(char* buf, const uint32_t size, uint32_t& offset, const VDCPlayerConsumeInfo& vdcConsumeInfo)
{
	sprintf(buf + offset, "{roleid=%d, consumetimes=%d, lastconsumetime=%s, lastconsumeamount=%d, totalconsumed=%d}\n", 
		vdcConsumeInfo.nRoleID, 
		vdcConsumeInfo.nConsumeTimes, 
		vdcConsumeInfo.szLastConsumeTime,
		vdcConsumeInfo.nLastConsumeAmount,
		vdcConsumeInfo.nTotalConsumed);
	offset = (uint32_t)strlen(buf);
}

void CVDCUserBaseInfo::VDCUserBaseInfoInit(RoleBaseInfo& usebaseinfo)
{
	usebaseinfo.nRoleID = -1;
	memset(usebaseinfo.szRoleName, 0, sizeof(usebaseinfo.szRoleName));
	usebaseinfo.ucGender = 0;
	usebaseinfo.ucUserLevel = 0;
	usebaseinfo.ucVipLevel = 0;
	usebaseinfo.uIdentity = 0;
	usebaseinfo.nMoney = 0;
	memset(usebaseinfo.szCreateTime, 0, sizeof(usebaseinfo.szCreateTime));
	usebaseinfo.nLastLoginTime = 0;
	usebaseinfo.nLastLogoutTime = 0;
	usebaseinfo.nBirthday = 19000101;
	memset(usebaseinfo.szHometownProvince, 0, sizeof(usebaseinfo.szHometownProvince));
	memset(usebaseinfo.szHometownCity, 0, sizeof(usebaseinfo.szHometownCity));
	memset(usebaseinfo.szHometownCounty, 0, sizeof(usebaseinfo.szHometownCounty));
	memset(usebaseinfo.szCurAddrProvince, 0, sizeof(usebaseinfo.szCurAddrProvince));
	memset(usebaseinfo.szCurAddrCity, 0, sizeof(usebaseinfo.szCurAddrCity));
	memset(usebaseinfo.szCurAddrCounty, 0, sizeof(usebaseinfo.szCurAddrCounty));
	usebaseinfo.nExperience = 0;
	memset(usebaseinfo.szLastLoginIP, 0,sizeof(usebaseinfo.szLastLoginIP));
	memset(usebaseinfo.szLoginIP, 0, sizeof(usebaseinfo.szLoginIP));
	memset(usebaseinfo.szHoroScope, 0, sizeof(usebaseinfo.szHoroScope));
	memset(usebaseinfo.szEmail, 0, sizeof(usebaseinfo.szEmail));
	memset(usebaseinfo.szTelephone, 0, sizeof(usebaseinfo.szTelephone));
	memset(usebaseinfo.szQQ, 0, sizeof(usebaseinfo.szQQ));
	usebaseinfo.nActiveEmail = 0;
	usebaseinfo.nActiveTele = 0;
	usebaseinfo.nBecomeVipTime = 0;
	usebaseinfo.nExpireVipTime = 0;
	usebaseinfo.nRmbUserType = 0;
	usebaseinfo.nAccountSafeType = 0;
	usebaseinfo.nLoginTimes = 0;
	usebaseinfo.nConsume = 0;
}

int32_t CVDCRoomBaseInfo::VDCRoomBaseInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const RoomBaseInfo& roombaseinfo)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.uRoomID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.szRoomName, sizeof(roombaseinfo.szRoomName));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.ucRoomType);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.ucChannelType);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.usChannelID);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.szChannelName, sizeof(roombaseinfo.szChannelName));
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.ucRoomType);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.usRegionID);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.szRegionName, sizeof(roombaseinfo.szRegionName));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.nRoomShowCapacity);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.nRoomRealCapacity);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.nRoomFund);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.nRoomFounderID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.nOwner);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.nRoomLevel);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.szRoomCreateTime, sizeof(roombaseinfo.szRoomCreateTime));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.szTempNotice, sizeof(roombaseinfo.szTempNotice));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.szRoomNotice, sizeof(roombaseinfo.szRoomNotice));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.szRoomTheme, sizeof(roombaseinfo.szRoomTheme));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.szRoomPassword, sizeof(roombaseinfo.szRoomPassword));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.nOption);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.nMicroTime);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.szRechargeNotice, sizeof(roombaseinfo.szRechargeNotice));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.szWelcomewords, sizeof(roombaseinfo.szWelcomewords));
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.nRobotPercent);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, roombaseinfo.nSendPercent);
	if(0 > ret)
	{
		return ret;
	}

	return S_OK;
}

int32_t CVDCRoomBaseInfo::VDCRoomBaseInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, RoomBaseInfo& roombaseinfo)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.uRoomID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.szRoomName, sizeof(roombaseinfo.szRoomName));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.ucRoomType);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.ucChannelType);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.usChannelID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.szChannelName, sizeof(roombaseinfo.szChannelName));
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.ucRegionType);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.usRegionID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.szRegionName, sizeof(roombaseinfo.szRegionName));
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.nRoomShowCapacity);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.nRoomRealCapacity);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.nRoomFund);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.nRoomFounderID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.nOwner);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.nRoomLevel);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.szRoomCreateTime, sizeof(roombaseinfo.szRoomCreateTime));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.szTempNotice, sizeof(roombaseinfo.szTempNotice));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.szRoomNotice, sizeof(roombaseinfo.szRoomNotice));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.szRoomTheme, sizeof(roombaseinfo.szRoomTheme));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.szRoomPassword, sizeof(roombaseinfo.szRoomPassword));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.nOption);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.nMicroTime);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.szRechargeNotice, sizeof(roombaseinfo.szRechargeNotice));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.szWelcomewords, sizeof(roombaseinfo.szWelcomewords));
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.nRobotPercent);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, roombaseinfo.nSendPercent);
	if(0 > ret)
	{
		return ret;
	}

	return S_OK;
}

void CVDCRoomBaseInfo::VDCRoomBaseInfoDump(char* buf, const uint32_t size, uint32_t& offset, const RoomBaseInfo& roombaseinfo)
{
	sprintf(buf + offset, "{roomid=%d, roomname=%s, roomtype=%d, channelid=%d, regionid=%d, showcapacity=%d, realcapacity=%d, \
						  roomfund=%d, roomfounderid=%d, ownerid=%d, roomlevel=%d, createtime=%s, tmpnotice=%s, roomnotice=%s, \
						  roomtheme=%s, roompassword=%s, option=%d, microtime=%d, rechargenotice=%s, robot_percent=%d, send_percent=%d}\n", 
		roombaseinfo.uRoomID,
		roombaseinfo.szRoomName,
		roombaseinfo.ucRoomType,
		roombaseinfo.usChannelID,
		roombaseinfo.usRegionID,
		roombaseinfo.nRoomShowCapacity,
		roombaseinfo.nRoomRealCapacity,
		roombaseinfo.nRoomFund,
		roombaseinfo.nRoomFounderID,
		roombaseinfo.nOwner,
		roombaseinfo.nRoomLevel,
		roombaseinfo.szRoomCreateTime,
		roombaseinfo.szTempNotice,
		roombaseinfo.szRoomNotice,
		roombaseinfo.szRoomTheme,
		roombaseinfo.szRoomPassword,
		roombaseinfo.nOption,
		roombaseinfo.nMicroTime,
		roombaseinfo.szRechargeNotice,
		roombaseinfo.nRobotPercent, 
		roombaseinfo.nSendPercent);
	offset = (uint32_t)strlen(buf);
}

int32_t CVDCUserBaseInfo::VDCUserBaseInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const RoleBaseInfo& usebaseinfo)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nRoleID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szAccountName, sizeof(usebaseinfo.szAccountName));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nAccountID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szRoleName, sizeof(usebaseinfo.szRoleName));
	if(0>ret)
	{
		return E_ENCODE_ROLE_NAME;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.ucGender);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.ucUserLevel);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.ucVipLevel);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.uIdentity);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nMoney);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szCreateTime, sizeof(usebaseinfo.szCreateTime));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nLastLoginTime);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nLastLogoutTime);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nBirthday);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szHometownProvince, sizeof(usebaseinfo.szHometownProvince));
	if(0>ret)
	{
		return E_ENCODE_ROLE_HOME_PROVINCE;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szHometownCity, sizeof(usebaseinfo.szHometownCity));
	if(0 > ret)
	{
		return E_ENCODE_ROLE_HOME_CITY;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szHometownCounty, sizeof(usebaseinfo.szHometownCounty));
	if(0 > ret)
	{
		return E_ENCODE_ROLE_HOME_COUNTY;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szCurAddrProvince, sizeof(usebaseinfo.szCurAddrProvince));
	if(0>ret)
	{
		return E_ENCODE_ROLE_CUR_PROVINCE;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szCurAddrCity, sizeof(usebaseinfo.szCurAddrCity));
	if(0 > ret)
	{
		return E_ENCODE_CUR_HOME_CITY;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szCurAddrCounty, sizeof(usebaseinfo.szCurAddrCounty));
	if(0 > ret)
	{
		return E_ENCODE_CUR_HOME_COUNTY;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nExperience);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szLastLoginIP, sizeof(usebaseinfo.szLastLoginIP));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szLoginIP, sizeof(usebaseinfo.szLoginIP));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szHoroScope, sizeof(usebaseinfo.szHoroScope));
	if(0>ret)
	{
		return E_ENCODE_HOROSCOPE;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szEmail, sizeof(usebaseinfo.szEmail));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szTelephone, sizeof(usebaseinfo.szTelephone));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.szQQ, sizeof(usebaseinfo.szQQ));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nActiveEmail);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nActiveTele);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nBecomeVipTime);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nExpireVipTime);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nRmbUserType);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nAccountSafeType);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nLoginTimes);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nMagnateLevel);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nConsume);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nNextLevelConusme);
	if(0 > ret)
	{
		return ret;
	}
//	ret = CCodeEngine::Encode(buf, size, offset, usebaseinfo.nGoldBean);
//	if(0 > ret)
//	{
//		return ret;
//	}
	return S_OK;
}

int32_t CVDCUserBaseInfo::VDCUserBaseInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, RoleBaseInfo& usebaseinfo)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nRoleID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szAccountName, sizeof(usebaseinfo.szAccountName));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nAccountID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szRoleName, sizeof(usebaseinfo.szRoleName));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.ucGender);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.ucUserLevel);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.ucVipLevel);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.uIdentity);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nMoney);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szCreateTime, sizeof(usebaseinfo.szCreateTime));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nLastLoginTime);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nLastLogoutTime);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nBirthday);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szHometownProvince, sizeof(usebaseinfo.szHometownProvince));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szHometownCity, sizeof(usebaseinfo.szHometownCity));
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szHometownCounty, sizeof(usebaseinfo.szHometownCounty));
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szCurAddrProvince, sizeof(usebaseinfo.szCurAddrProvince));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szCurAddrCity, sizeof(usebaseinfo.szCurAddrCity));
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szCurAddrCounty, sizeof(usebaseinfo.szCurAddrCounty));
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nExperience);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szLastLoginIP, sizeof(usebaseinfo.szLastLoginIP));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szLoginIP, sizeof(usebaseinfo.szLoginIP));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szHoroScope, sizeof(usebaseinfo.szHoroScope));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szEmail, sizeof(usebaseinfo.szEmail));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szTelephone, sizeof(usebaseinfo.szTelephone));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.szQQ, sizeof(usebaseinfo.szQQ));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nActiveEmail);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nActiveTele);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nBecomeVipTime);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nExpireVipTime);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nRmbUserType);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nAccountSafeType);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nLoginTimes);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nMagnateLevel);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nConsume);
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nNextLevelConusme);
	if(0 > ret)
	{
		return ret;
	}
//	ret = CCodeEngine::Decode(buf, size, offset, usebaseinfo.nGoldBean);
//	if(0 > ret)
//	{
//		return ret;
//	}
	return S_OK;
}

void CVDCUserBaseInfo::VDCUserBaseInfoDump(char* buf, const uint32_t size, uint32_t& offset, const RoleBaseInfo& usebaseinfo)
{
	sprintf(buf + offset, "nRoleID=%d, \
						  rolename=%s, \
						  Gender=%d, \
						  uselevel=%d,  \
						  viplevel=%d,  \
						  Identity=%d, \
						  money=%d,  \
						  szCreateTime=%s, \
						  nLastLoginTime=%ld, \
						  nLastLogoutTime=%ld, \
						  nBirthday=%d, \
						  szHomeProvince=%s, \
						  szHomeCity=%s, \
						  szHomeCounty=%s,\
						  szCurProvince=%s,\
						  szCurCity=%s,\
						  szCurCounty=%s,\
						  nExperience=%d,\
						  szLastLoginIP=%s,\
						  szLoginIP=%s,\
						  szHoroScope=%s,\
						  szEmail=%s,\
						  szTele=%s,\
						  szQQ=%s,\
						  nActiveEmail=%d,\
						  nActivieTele=%d,\
						  nBecomeVipTime=%lu,\
						  nExpVipTime=%lu,\
						  nRmbUserType=%d,\
						  nAccountSafeType=%d,\
						  nLoginTimes=%d, \
						  nMagnateLevel=%d, \
						  nConsume=%ld, \
						  nNextConsume=%ld",
	    usebaseinfo.nRoleID,
		usebaseinfo.szRoleName,
		usebaseinfo.ucGender,
		usebaseinfo.ucUserLevel,
		usebaseinfo.ucVipLevel,
		usebaseinfo.uIdentity,
		usebaseinfo.nMoney,
		usebaseinfo.szCreateTime,
		usebaseinfo.nLastLoginTime,
		usebaseinfo.nLastLogoutTime,
		usebaseinfo.nBirthday,
		usebaseinfo.szHometownProvince,
		usebaseinfo.szHometownCity,
		usebaseinfo.szHometownCounty,
		usebaseinfo.szCurAddrProvince,
		usebaseinfo.szCurAddrCity,
		usebaseinfo.szCurAddrCounty,
		usebaseinfo.nExperience,
		usebaseinfo.szLastLoginIP,
		usebaseinfo.szLoginIP,
		usebaseinfo.szHoroScope,
		usebaseinfo.szEmail,
		usebaseinfo.szTelephone,
		usebaseinfo.szQQ,
		usebaseinfo.nActiveEmail,
		usebaseinfo.nActiveTele,
		usebaseinfo.nBecomeVipTime,
		usebaseinfo.nExpireVipTime,
		usebaseinfo.nRmbUserType, 
		usebaseinfo.nAccountSafeType,
		usebaseinfo.nLoginTimes, 
		usebaseinfo.nMagnateLevel, 
		usebaseinfo.nConsume, 
		usebaseinfo.nNextLevelConusme);
	offset = (uint32_t)strlen(buf);
}

int32_t CVDCUserAccount::VDCUserAccountEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const UserAccount& useraccount)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Encode(buf, size, offset, useraccount.nRoleID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, useraccount.n179ID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, useraccount.sz179RegEmail, sizeof(useraccount.sz179RegEmail));
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, useraccount.szAccountFrom, sizeof(useraccount.szAccountFrom));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Encode(buf, size, offset, useraccount.szAccountID, sizeof(useraccount.szAccountID));
	if(0>ret)
	{
		return ret;
	}

	return S_OK;
}

int32_t CVDCUserAccount::VDCUserAccountDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, UserAccount& useraccount)
{
	int32_t ret = S_OK;
	ret = CCodeEngine::Decode(buf, size, offset, useraccount.nRoleID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, useraccount.n179ID);
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, useraccount.sz179RegEmail, sizeof(useraccount.sz179RegEmail));
	if(0 > ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf,size, offset, useraccount.szAccountFrom, sizeof(useraccount.szAccountFrom));
	if(0>ret)
	{
		return ret;
	}
	ret = CCodeEngine::Decode(buf, size, offset, useraccount.szAccountID, sizeof(useraccount.szAccountID));
	if(0>ret)
	{
		return ret;
	}

	return S_OK;
}

void CVDCUserAccount::VDCUserAccountDump(char* buf, const uint32_t size, uint32_t& offset, const UserAccount& useraccount)
{
	sprintf(buf + offset, "\nroleid=%d, \nn179id=%d, \n179RegEmail=%s, \naccountfrom=%s, \naccountid=%s\n", 
		useraccount.nRoleID,
		useraccount.n179ID,
		useraccount.sz179RegEmail, 
		useraccount.szAccountFrom,
		useraccount.szAccountID);
	offset = strlen(buf);
}

FRAME_DBPROXY_NAMESPACE_END
