/************************************************************************
* filename: dal_player.h
* date:     2011-12-29 15:15
* author:   xiaohq
* purpose:  定义缓存中保存的玩家对象，也就是玩家的信息，如roleid， 等级
			资产，登入，登出时间等等
* revision: not yet
************************************************************************/

#ifndef _DBPROXY_DAL_PLAYER_H
#define _DBPROXY_DAL_PLAYER_H

#include <string.h>
#include <memory.h>
#include "common/common_def.h"
#include "common/common_errordef.h"
#include "../def/def_dbproxy.h"
#include "frame_typedef.h"
#include "../../public/public_typedef.h"
#include "common/common_typedef.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

//user_base_info表结构, 总共大概2K
typedef struct tagRoleBaseInfo
{
	RoleID      nRoleID;
	char        szRoleName[enmMaxRoleNameLength]; //enmMaxRoleNameLength = 32
	char		szAccountName[MaxAccountName];
	AccountID	nAccountID;
	Gender      ucGender;     //Gender : uint8_t
	UserLevel   ucUserLevel;  //UserLevel: uint8_t
	VipLevel    ucVipLevel;   //VipLevel: uint8_t
	IdentityType uIdentity;  //玩家身份
	int32_t	    nMoney;
	char        szCreateTime[enmMaxTimeStringLength]; //enmMaxTimeStringLength = 128
	int64_t     nLastLoginTime;
	int64_t     nLastLogoutTime;
	int32_t		nBirthday;
	char        szHometownProvince[enmMaxProvinceNameLength];
	char        szHometownCity[enmMaxCityNameLength];
	char        szHometownCounty[enmMaxCountyNameLength];
	char        szCurAddrProvince[enmMaxProvinceNameLength];
	char        szCurAddrCity[enmMaxCityNameLength];
	char        szCurAddrCounty[enmMaxCountyNameLength];
	int32_t     nExperience;
	char        szLastLoginIP[enmMaxIPAddressLength];  //enmMaxIPAddressLength = 20
	char        szLoginIP[enmMaxIPAddressLength];      //enmMaxIPAddressLength = 20
	char        szHoroScope[enmMaxHoroScopeNameLength];  //enmMaxHoroScopeNameLength = 32
	char        szEmail[enmMaxEmailAddrLength];   //enmMaxEmailAddrLength = 32
	char        szTelephone[enmMaxTelephoneLength];  //enmMaxTelephoneLength = 16
	char        szQQ[enmMaxQQLength];     //enmMaxQQLength = 16
	int32_t     nActiveEmail;
	int32_t     nActiveTele;
	uint64_t    nBecomeVipTime;
	uint64_t    nExpireVipTime;
	int32_t     nRmbUserType;       //是否是付费用户
	int32_t     nAccountSafeType;   //是否设置密保
	int32_t     nLoginTimes;        //登陆次数
	MagnateLevel 	nMagnateLevel;
	uint64_t    nConsume;           //消费的179币的数量
	uint64_t    nNextLevelConusme;   //达到下一个富豪等级需要消费的179币的数量
	tagRoleBaseInfo()
	{
		nRoleID = enmInvalidRoleID;
		memset(szRoleName, 0, sizeof(szRoleName));
		memset(szAccountName, 0, sizeof(szAccountName));
		nAccountID = enmInvalidAccountID;
		ucGender = 0;
		ucUserLevel = 0;
		ucVipLevel = 0;
		uIdentity = 0;
		nMoney = 0;
		memset(szCreateTime, 0, sizeof(0));
		nLastLoginTime = 0;
		nLastLogoutTime = 0;
		nBirthday = 19000101;
		memset(szHometownProvince, 0, sizeof(szHometownProvince));
		memset(szHometownCity, 0, sizeof(szHometownCity));
		memset(szHometownCounty, 0, sizeof(szHometownCounty));
		memset(szCurAddrProvince, 0, sizeof(szCurAddrProvince));
		memset(szCurAddrCity, 0, sizeof(szCurAddrCity));
		memset(szCurAddrCounty, 0, sizeof(szCurAddrCounty));
		nExperience = 0;
		memset(szLastLoginIP, 0,sizeof(szLastLoginIP));
		memset(szLoginIP, 0, sizeof(szLoginIP));
		memset(szHoroScope, 0, sizeof(szHoroScope));
		memset(szEmail, 0, sizeof(szEmail));
		memset(szTelephone, 0, sizeof(szTelephone));
		memset(szQQ, 0, sizeof(szQQ));
		nActiveEmail = 0;
		nActiveTele = 0;
		nBecomeVipTime = 0;
		nExpireVipTime = 0;
		nRmbUserType = 0;       //是否是付费玩家
		nAccountSafeType = 0;   //是否设置密保
		nLoginTimes = 0;
		nMagnateLevel = 0;        //富豪等级
		nConsume = 0;           //179币的数量
		nNextLevelConusme = 0;  //达到下一个富豪等级需要消费的179币的数量
	}
}RoleBaseInfo;

//user_base_info表所有字段
enum
{
	enmUserBaseInfo_RoleID           = 0,
	enmUserBaseInfo_RoleName,
	enmUserBaseInfo_Gender,
	enmUserBaseInfo_UserLevel,
	enmUserBaseInfo_VipLevel,
	enmUserBaseInfo_Identity,
	enmUserBaseInfo_Money,
	enmUserBaseInfo_CreateTime,
	enmUserBaseInfo_LastLoginTime, 
	enmUserBaseInfo_LastLogoutTime,
	enmUserBaseInfo_Birthday,
	enmUserBaseInfo_HometownProvince,
	enmUserBaseInfo_HometownCity,
	enmUserBaseInfo_HometownCounty,
	enmUserBaseInfo_CurAddrProvince,
	enmUserBaseInfo_CurAddrCity,
	enmUserBaseInfo_CurAddrCounty,
	enmUserBaseInfo_Experince,
	enmUserBaseInfo_LastLoginIP,
	enmUserBaseInfo_LoginIP,
	enmUserBaseInfo_HoroScope,
	enmUserBaseInfo_Email,
	enmUserBaseInfo_Tele,
	enmUserBaseInfo_QQ,
	enmUserBaseInfo_ActiveEamil,
	enmUserBaseInfo_ActiveTele,
	enmUserBaseInfo_BecomeVipTime,
	enmUserBaseInfo_ExpireVipTime, 
	enmUserBaseInfo_RmbUserType,       //是否是付费玩家
	enmUserBaseInfo_AccountSafeType,   //是否设置密保
	enmUserBaseInfo_LoginTimes,        //登陆次数
	enmUserBaseInfo_MagnateLevel,      //富豪等级
	enmUserBaseInfo_Consume,          //消费的179币的数量
	enmUserBaseInfo_NextLevelConsume,          //达到下一个富豪等级需要消费的179币的数量
};

//用户账户信息表结构
typedef struct tagUserAccount
{
	RoleID    nRoleID;  //RoleID : int32_t 
	int32_t   n179ID;
	char      sz179RegEmail[enmMaxEmailAddrLength];   //注册179账号的邮箱
	char      szAccountFrom[enmMaxAccountSrcLength];  //enmMaxAccountSrcLength = 32
	char      szAccountID[enmMaxAccountIDLength];     //enmMaxAccountIDLength = 32
}UserAccount;

//user_account表所有字段
enum
{
	enmUserAccount_RoleID = 0,
	enmUserAccount_179ID,
	enmUserAccount_179RegEmail, 
	enmUserAccount_AccountFrom,
	enmUserAccount_AccountID,
};

//房间基本信息表对应的结构体，大约500字节
typedef struct tagRoomBaseInfo
{
	RoomID      uRoomID;
	char        szRoomName[MaxRoomNameLength];
	RoomType    ucRoomType;
	ChannelType ucChannelType;
	ChannelID   usChannelID;
	char        szChannelName[MaxChannelNameLength];
	RegionType  ucRegionType;
	RegionID    usRegionID;
	char        szRegionName[MaxRegionNameLength];
	int32_t     nRoomShowCapacity;
	int32_t     nRoomRealCapacity;
	int32_t     nRoomFund;            //房间公积金
	int32_t     nRoomFounderID;       //房间创立者ID
	int32_t     nOwner;               //房间拥有者ID
	int32_t     nRoomLevel;           //房间等级
	//int32_t     nAdminCount;          //房间管理员个数
	char        szRoomCreateTime[enmMaxTimeStringLength];   //房间创立时间
	char        szTempNotice[MaxRoomNoticeLength];        //最长临时公告
	char        szRoomNotice[MaxRoomNoticeLength];       //房间公告
	char        szRoomTheme[MaxRoomSignatureLength];         //房间主题
	char        szRoomPassword[MaxRoomPasswordLength];   //房间密码
	int32_t     nOption;                                    //房间里面的选项
	int32_t     nMicroTime;                                 //上麦时间限制，比如每个人上麦不能超过5分钟
	char        szRechargeNotice[MaxRoomNoticeLength];  //充值通知
	char        szWelcomewords[MaxRoomWelcomeLength];    //房间欢迎词
	uint32_t    nRobotPercent;                            //机器人比率
	uint32_t    nSendPercent;                             //机器人发送彩条的概率
}RoomBaseInfo;

//房间基本信息表中的所有字段
enum
{
	enmRoomBaseInfo_roomid,
	enmRoomBaseInfo_roomname,
	enmRoomBaseInfo_roomtype,
	enmRoomBaseInfo_roomchanneltype,
	enmRoomBaseInfo_roomchannelid,
	enmRoomBaseInfo_roomchannelname,
	enmRoomBaseInfo_roomregiontype,
	enmRoomBaseInfo_roomregionid,
	enmRoomBaseInfo_roomregionname,
	enmRoomBaseInfo_roomshowcapacity,
	enmRoomBaseInfo_roomrealcapacity,
	enmRoomBaseInfo_roomfund,
	enmRoomBaseInfo_roomfounderid,
	enmRoomBaseInfo_roomowner,
	enmRoomBaseInfo_roomlevel,
	//enmRoomBaseInfo_roomadmincount,
	enmRoomBaseInfo_roomcreatetime,
	enmRoomBaseInfo_roomtmpnotice,
	enmRoomBaseInfo_roomnotice,
	enmRoomBaseInfo_roomtheme,
	enmRoomBaseInfo_roompassword,
	enmRoomBaseInfo_roomoption,
	enmRoomBaseInfo_roommicrotime,
	enmRoomBaseInfo_roomrechargenotice,
	enmRoomBaseInfo_roomwelcomewords,
	enmRoomBaseInfo_RobotPercent,      //机器人比例
	enmRoomBaseInfo_SendPercent,       //机器人发送彩条的概率
};

//登陆大厅获取房间信息

//玩家在房间中的信息
typedef struct tagVDCPlayerRoomInfo
{
	RoomID		    nRoomID;			//进入的房间
	uint8_t		    nPlayerState;		//在房间的状态
	RoleRank		nRoleRank;			//在房间的管理权限
	uint32_t		nEnterRoomTime;		//进入房间的时间
	uint64_t		nTotalTime;			//在本房间的总共时间
}VDCPlayerRoomInfo;

//玩家物品信息
typedef struct tagVDCPlayerItemInfo
{
	RoleID     nRoleID;
	ItemID    nItemID;
	UseMeans   nUseMeans;    //使用方式
	int32_t    nQuantity;    //当使用方式为计时使用时，标示拥有时长； 若是计数使用，则标示拥有个数
	int32_t    nGetTime;     //获得道具的时间
}VDCPlayerItemInfo;

typedef struct tagItemPurchaseRecord
{
	int32_t  nPurchaseID;    
	ItemID   nItemID;
	char     szItemName[enmMaxItemNameLength];
	RoleID   nFromID; 
	char     szFromRoleName[enmMaxRoleNameLength];
	int32_t  nPrice;         
	RoleID   nToID;          
	int32_t  nBenifit;          
	int32_t  nCount;            
	char     szPurchaseTime[enmMaxTimeStringLength];  
}ItemPurchaseRecord;

//user_item表所有字段
enum
{
	enmUserItemField_RoleID		= 0,
	enmUserItemField_ItemID,
	enmUserItemField_UseMeans,  
	enmUserItemField_Quantity,
	enmUserItemField_GetTime,
};

//玩家充值信息表结构
typedef struct tagVDCPlayerRechargeInfo
{
	RoleID   nRoleID;
	int32_t  nRechargeTimes;
	char     szLastRechargeTime[enmMaxTimeStringLength];  
	int32_t  nLastRechargeAmount;
	int32_t  nLastRechageChannel;
	int32_t  nTotalRechargeAmount;      
}VDCPlayerRechargeInfo;

//玩家充值信息表中的所有字段
enum
{
	enmPlayerRechargeInfo_RoleID = 0,
	enmPlayerRechargeInfo_RechargeTimes,
	enmPlayerRechargeInfo_LastRechargeTime,
	enmPlayerRechargeInfo_LastRechargeAmount,
	enmPlayerRechargeInfo_LastRechargeChannel,
	enmPlayerRechargeInfo_TotalRechargeAmount,
};

//玩家消费信息
typedef struct tagVDCPlayerConsumeInfo
{
	RoleID     nRoleID;
	int32_t    nConsumeTimes;                 //消费次数
	char       szLastConsumeTime[enmMaxTimeStringLength];              //上一次消费时间
	int32_t    nLastConsumeAmount;            //上次消费金额
	int32_t    nTotalConsumed;                //总消费额
}VDCPlayerConsumeInfo;

//玩家消费记录表中的所有字段
enum
{
	enmPlayerConsumeInfo_RoleID = 0,
	enmPlayerConsumeInfo_ConsumeTimes,
	enmPlayerConsumeInfo_LastConsumeTime,
	enmPlayerConsumeInfo_LastConsumeAmount,
	enmPlayerConsumeInfo_TotalConsumed,
};

//玩家获得奖励信息（中奖）
typedef struct tagVDCPlayerPrizeInfo
{
	RoleID   nRoleID;
	int32_t  nObtainPrizeTimes;              //获得奖励的次数
	char     szLastGetPrizeTime[enmMaxTimeStringLength]; //上次获奖时间          //上次获奖时间
	int32_t  nLastObtainPrizeAmount;         //上次获得奖励的数量
	int32_t  nTotalObtainPrizeAmount;        //总共获得的奖励的数量
}VDCPlayerPrizeInfo;

//玩家获奖纪录表所有字段
enum
{
	enmPlayerPrizeInfo_RoleID = 0,
	enmPlayerPrizeInfo_ObtainPrizeTimes,
	enmPlayerPrizeInfo_LastObtainPrizeTime,
	enmPlayerPrizeInfo_LastObtainPrizeAmount,
	enmPlayerPrizeInfo_TotalObtainPrizeAmount,
};

////玩家购买道具记录表
//道具购买记录item_purchase
//Id:自动增长、主键ID
//Itme_id:物品配置表中的id属性
//From_id:购买者的ID
//Price:购买时的单个价格
//To_id:使用或赠送人的ID多个用逗号隔开
//Benefit:赠送人的单个收益
//Count:个数
//Time:时间

//玩家购买道具记录表
typedef struct tagUserItemPurchase
{
	int32_t nPurchaseID;
	ItemID  nItemID;
	RoleID  nFromID;
	int32_t nPrice;
	int32_t nToIDCount;
	int32_t nToIDS[MaxUserCountPerRoom];
	int32_t nBenifit;
	int32_t nItemCount;
	char    szPurchaseTime[enmMaxTimeStringLength];
}UserItemPurchase;

//玩家购买道具记录表所有字段
enum
{
	enmUserItemPurchase_PurchaseID = 0,
	enmUserItemPurchase_ItemID,
	enmUserItemPurchase_FromID,
	enmUserItemPurchase_Price,
	enmUserItemPurchase_ToIDS,
	enmUserItemPurchase_Benefit,
	enmUserItemPurchase_ItemCount,
	enmUserItemPurchase_PurchaseTime,
};

//房间管理员表结构
typedef struct tagRoomAdminInfo
{
	RoomID  uRoomID;
	RoleID  nRoleID;
	RoleRank  usRoleRank;
}RoomAdminInfo;

//房间管理员表各个字段
enum
{
	enmRoomAdminInfo_RoomID = 0,
	enmRoomAdminInfo_RoleID,
	enmRoomAdminInfo_RoleRank,
};

//房间管理员表
typedef struct tagRoomMember
{
	RoleID    nRoleID;
	RoomID    uRoomID;
	RegionID  usRegionID;
	ChannelID usChannelID;
	RoleRank  usRoleRank;                      
	char      szJoinTime[enmMaxTimeStringLength];                 //加入时间
	char      szLastEnterTime[enmMaxTimeStringLength];            //上次进入房间时间
	int32_t   nTotalTimeInRoom;                                   //总共在房间的时间
	int32_t   nTotalOnMicTimeInRoom;                              //总共在麦时长
	int32_t   nDevotePoint;                                       //房间贡献点
	char      szLastGetDevoteTime[enmMaxTimeStringLength];        //上次领取贡献点的时间
};

//房间管理员表需要查询的字段
enum
{
	enmRoomMember_RoleID = 0,         //管理员的roleid
	enmRoomMember_RoleRank,           //管理员的权限
	enmRoomMember_InRoomTime,         //总共的在房间的时长
};

class CVDCRoomBaseInfo
{
public:
	CVDCRoomBaseInfo(){}
	virtual ~CVDCRoomBaseInfo(){}

public:
	static int32_t VDCRoomBaseInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const RoomBaseInfo& roombaseinfo);
	static int32_t VDCRoomBaseInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, RoomBaseInfo& roombaseinfo);
	static void    VDCRoomBaseInfoDump(char* buf, const uint32_t size, uint32_t&  offset, const RoomBaseInfo& roombaseinfo);
};

class CVDCPlayerRoomInfo
{
public:
	CVDCPlayerRoomInfo(){}
	virtual ~CVDCPlayerRoomInfo(){}
public:
	static int32_t VDCPlayerRoomInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const VDCPlayerRoomInfo& vdcPlayerRoomInfo);
	static int32_t VDCPlayerRoomInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, VDCPlayerRoomInfo& vdcplayerRoomInfo);
	static void    VDCPlayerRoomInfoDump(char* buf, const uint32_t size, uint32_t& offset, const VDCPlayerRoomInfo& vdcplayerRoomInfo);
};

class CVDCPlayerItemInfo
{
public:
	CVDCPlayerItemInfo(){}
	virtual ~CVDCPlayerItemInfo(){}
public:
	static int32_t VDCPlayerItemInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const VDCPlayerItemInfo& vdcItemInfo);
	static int32_t VDCPlayerItemInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, VDCPlayerItemInfo& vdcItemInfo);
	static void    VDCPlayerItemInfoDump(char* buf, const uint32_t size, uint32_t& offset, const VDCPlayerItemInfo& vdcItemInfo);
};

class CVDCPlayerPrizeInfo
{
public:
	CVDCPlayerPrizeInfo(){}
	virtual ~CVDCPlayerPrizeInfo(){}

public:
	static int32_t VDCPlayerPrizeInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const VDCPlayerPrizeInfo& vdcPrizeInfo);
	static int32_t VDCPlayerPrizeInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, VDCPlayerPrizeInfo& vdcPrizeInfo);
	static void    VDCPlyaerPrizeInfoDump(char* buf, const uint32_t size, uint32_t& offset, const VDCPlayerPrizeInfo& vdcPrizeInfo);
};

class CVDCPlayerConsumeInfo
{
public:
	CVDCPlayerConsumeInfo(){}
	virtual ~CVDCPlayerConsumeInfo(){}

public:
	static int32_t VDCPlayerConsumeInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const VDCPlayerConsumeInfo& vdcConsumeInfo);
	static int32_t VDCPlayerConsumeInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, VDCPlayerConsumeInfo& vdcConsumeInfo);
	static void    VDCPlayerConsumeInfoDump(char* buf, const uint32_t size, uint32_t& offset, const VDCPlayerConsumeInfo& vdcConsumeInfo);
};

class CVDCPlayerRechargeInfo
{
public:
	CVDCPlayerRechargeInfo(){}
	virtual ~CVDCPlayerRechargeInfo(){}

public:
	static int32_t VDCPlayerRechargeInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const VDCPlayerRechargeInfo& vdcRechargeInfo);
	static int32_t VDCPlayerRechargeInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, VDCPlayerRechargeInfo& vdcRechargeInfo);
	static void    VDCPlayerRechargeInfoDump(char* buf, const uint32_t size, uint32_t& offset, const VDCPlayerRechargeInfo& vdcRechargeInfo);
};

//class CVDCPlayerInfo
//{
//public:
//	CVDCPlayerInfo(){}
//	virtual ~CVDCPlayerInfo(){}
//public:
//	static int32_t VDCPlayerInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const VDCPlayer& vdcplayer);
//	static int32_t VDCPlayerInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, VDCPlayer& vdcplayer);
//	static void    VDCPlayerInfoDump(char* buf, const uint32_t size, uint32_t& offset, const VDCPlayer& vdcPlayer);
//};

class CVDCUserBaseInfo
{
public:
	CVDCUserBaseInfo(){}
	virtual ~CVDCUserBaseInfo(){}
public:
	static void    VDCUserBaseInfoInit(RoleBaseInfo& usebaseinfo);
	static int32_t VDCUserBaseInfoEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const RoleBaseInfo& usebaseinfo);
	static int32_t VDCUserBaseInfoDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, RoleBaseInfo& usebaseinfo);
	static void    VDCUserBaseInfoDump(char* buf, const uint32_t size, uint32_t& offset, const RoleBaseInfo& usebaseinfo);
};

class CVDCUserAccount
{
public:
	CVDCUserAccount(){}
	virtual ~CVDCUserAccount(){}
public:
	static int32_t VDCUserAccountEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const UserAccount& useraccount);
	static int32_t VDCUserAccountDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, UserAccount& useraccount);
	static void    VDCUserAccountDump(char* buf, const uint32_t size, uint32_t& offset, const UserAccount& useraccount);
};

FRAME_DBPROXY_NAMESPACE_END

#endif
