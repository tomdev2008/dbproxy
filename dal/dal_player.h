/************************************************************************
* filename: dal_player.h
* date:     2011-12-29 15:15
* author:   xiaohq
* purpose:  ���建���б������Ҷ���Ҳ������ҵ���Ϣ����roleid�� �ȼ�
			�ʲ������룬�ǳ�ʱ��ȵ�
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

//user_base_info��ṹ, �ܹ����2K
typedef struct tagRoleBaseInfo
{
	RoleID      nRoleID;
	char        szRoleName[enmMaxRoleNameLength]; //enmMaxRoleNameLength = 32
	char		szAccountName[MaxAccountName];
	AccountID	nAccountID;
	Gender      ucGender;     //Gender : uint8_t
	UserLevel   ucUserLevel;  //UserLevel: uint8_t
	VipLevel    ucVipLevel;   //VipLevel: uint8_t
	IdentityType uIdentity;  //������
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
	int32_t     nRmbUserType;       //�Ƿ��Ǹ����û�
	int32_t     nAccountSafeType;   //�Ƿ������ܱ�
	int32_t     nLoginTimes;        //��½����
	MagnateLevel 	nMagnateLevel;
	uint64_t    nConsume;           //���ѵ�179�ҵ�����
	uint64_t    nNextLevelConusme;   //�ﵽ��һ�������ȼ���Ҫ���ѵ�179�ҵ�����
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
		nRmbUserType = 0;       //�Ƿ��Ǹ������
		nAccountSafeType = 0;   //�Ƿ������ܱ�
		nLoginTimes = 0;
		nMagnateLevel = 0;        //�����ȼ�
		nConsume = 0;           //179�ҵ�����
		nNextLevelConusme = 0;  //�ﵽ��һ�������ȼ���Ҫ���ѵ�179�ҵ�����
	}
}RoleBaseInfo;

//user_base_info�������ֶ�
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
	enmUserBaseInfo_RmbUserType,       //�Ƿ��Ǹ������
	enmUserBaseInfo_AccountSafeType,   //�Ƿ������ܱ�
	enmUserBaseInfo_LoginTimes,        //��½����
	enmUserBaseInfo_MagnateLevel,      //�����ȼ�
	enmUserBaseInfo_Consume,          //���ѵ�179�ҵ�����
	enmUserBaseInfo_NextLevelConsume,          //�ﵽ��һ�������ȼ���Ҫ���ѵ�179�ҵ�����
};

//�û��˻���Ϣ��ṹ
typedef struct tagUserAccount
{
	RoleID    nRoleID;  //RoleID : int32_t 
	int32_t   n179ID;
	char      sz179RegEmail[enmMaxEmailAddrLength];   //ע��179�˺ŵ�����
	char      szAccountFrom[enmMaxAccountSrcLength];  //enmMaxAccountSrcLength = 32
	char      szAccountID[enmMaxAccountIDLength];     //enmMaxAccountIDLength = 32
}UserAccount;

//user_account�������ֶ�
enum
{
	enmUserAccount_RoleID = 0,
	enmUserAccount_179ID,
	enmUserAccount_179RegEmail, 
	enmUserAccount_AccountFrom,
	enmUserAccount_AccountID,
};

//���������Ϣ���Ӧ�Ľṹ�壬��Լ500�ֽ�
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
	int32_t     nRoomFund;            //���乫����
	int32_t     nRoomFounderID;       //���䴴����ID
	int32_t     nOwner;               //����ӵ����ID
	int32_t     nRoomLevel;           //����ȼ�
	//int32_t     nAdminCount;          //�������Ա����
	char        szRoomCreateTime[enmMaxTimeStringLength];   //���䴴��ʱ��
	char        szTempNotice[MaxRoomNoticeLength];        //���ʱ����
	char        szRoomNotice[MaxRoomNoticeLength];       //���乫��
	char        szRoomTheme[MaxRoomSignatureLength];         //��������
	char        szRoomPassword[MaxRoomPasswordLength];   //��������
	int32_t     nOption;                                    //���������ѡ��
	int32_t     nMicroTime;                                 //����ʱ�����ƣ�����ÿ���������ܳ���5����
	char        szRechargeNotice[MaxRoomNoticeLength];  //��ֵ֪ͨ
	char        szWelcomewords[MaxRoomWelcomeLength];    //���件ӭ��
	uint32_t    nRobotPercent;                            //�����˱���
	uint32_t    nSendPercent;                             //�����˷��Ͳ����ĸ���
}RoomBaseInfo;

//���������Ϣ���е������ֶ�
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
	enmRoomBaseInfo_RobotPercent,      //�����˱���
	enmRoomBaseInfo_SendPercent,       //�����˷��Ͳ����ĸ���
};

//��½������ȡ������Ϣ

//����ڷ����е���Ϣ
typedef struct tagVDCPlayerRoomInfo
{
	RoomID		    nRoomID;			//����ķ���
	uint8_t		    nPlayerState;		//�ڷ����״̬
	RoleRank		nRoleRank;			//�ڷ���Ĺ���Ȩ��
	uint32_t		nEnterRoomTime;		//���뷿���ʱ��
	uint64_t		nTotalTime;			//�ڱ�������ܹ�ʱ��
}VDCPlayerRoomInfo;

//�����Ʒ��Ϣ
typedef struct tagVDCPlayerItemInfo
{
	RoleID     nRoleID;
	ItemID    nItemID;
	UseMeans   nUseMeans;    //ʹ�÷�ʽ
	int32_t    nQuantity;    //��ʹ�÷�ʽΪ��ʱʹ��ʱ����ʾӵ��ʱ���� ���Ǽ���ʹ�ã����ʾӵ�и���
	int32_t    nGetTime;     //��õ��ߵ�ʱ��
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

//user_item�������ֶ�
enum
{
	enmUserItemField_RoleID		= 0,
	enmUserItemField_ItemID,
	enmUserItemField_UseMeans,  
	enmUserItemField_Quantity,
	enmUserItemField_GetTime,
};

//��ҳ�ֵ��Ϣ��ṹ
typedef struct tagVDCPlayerRechargeInfo
{
	RoleID   nRoleID;
	int32_t  nRechargeTimes;
	char     szLastRechargeTime[enmMaxTimeStringLength];  
	int32_t  nLastRechargeAmount;
	int32_t  nLastRechageChannel;
	int32_t  nTotalRechargeAmount;      
}VDCPlayerRechargeInfo;

//��ҳ�ֵ��Ϣ���е������ֶ�
enum
{
	enmPlayerRechargeInfo_RoleID = 0,
	enmPlayerRechargeInfo_RechargeTimes,
	enmPlayerRechargeInfo_LastRechargeTime,
	enmPlayerRechargeInfo_LastRechargeAmount,
	enmPlayerRechargeInfo_LastRechargeChannel,
	enmPlayerRechargeInfo_TotalRechargeAmount,
};

//���������Ϣ
typedef struct tagVDCPlayerConsumeInfo
{
	RoleID     nRoleID;
	int32_t    nConsumeTimes;                 //���Ѵ���
	char       szLastConsumeTime[enmMaxTimeStringLength];              //��һ������ʱ��
	int32_t    nLastConsumeAmount;            //�ϴ����ѽ��
	int32_t    nTotalConsumed;                //�����Ѷ�
}VDCPlayerConsumeInfo;

//������Ѽ�¼���е������ֶ�
enum
{
	enmPlayerConsumeInfo_RoleID = 0,
	enmPlayerConsumeInfo_ConsumeTimes,
	enmPlayerConsumeInfo_LastConsumeTime,
	enmPlayerConsumeInfo_LastConsumeAmount,
	enmPlayerConsumeInfo_TotalConsumed,
};

//��һ�ý�����Ϣ���н���
typedef struct tagVDCPlayerPrizeInfo
{
	RoleID   nRoleID;
	int32_t  nObtainPrizeTimes;              //��ý����Ĵ���
	char     szLastGetPrizeTime[enmMaxTimeStringLength]; //�ϴλ�ʱ��          //�ϴλ�ʱ��
	int32_t  nLastObtainPrizeAmount;         //�ϴλ�ý���������
	int32_t  nTotalObtainPrizeAmount;        //�ܹ���õĽ���������
}VDCPlayerPrizeInfo;

//��һ񽱼�¼�������ֶ�
enum
{
	enmPlayerPrizeInfo_RoleID = 0,
	enmPlayerPrizeInfo_ObtainPrizeTimes,
	enmPlayerPrizeInfo_LastObtainPrizeTime,
	enmPlayerPrizeInfo_LastObtainPrizeAmount,
	enmPlayerPrizeInfo_TotalObtainPrizeAmount,
};

////��ҹ�����߼�¼��
//���߹����¼item_purchase
//Id:�Զ�����������ID
//Itme_id:��Ʒ���ñ��е�id����
//From_id:�����ߵ�ID
//Price:����ʱ�ĵ����۸�
//To_id:ʹ�û������˵�ID����ö��Ÿ���
//Benefit:�����˵ĵ�������
//Count:����
//Time:ʱ��

//��ҹ�����߼�¼��
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

//��ҹ�����߼�¼�������ֶ�
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

//�������Ա��ṹ
typedef struct tagRoomAdminInfo
{
	RoomID  uRoomID;
	RoleID  nRoleID;
	RoleRank  usRoleRank;
}RoomAdminInfo;

//�������Ա������ֶ�
enum
{
	enmRoomAdminInfo_RoomID = 0,
	enmRoomAdminInfo_RoleID,
	enmRoomAdminInfo_RoleRank,
};

//�������Ա��
typedef struct tagRoomMember
{
	RoleID    nRoleID;
	RoomID    uRoomID;
	RegionID  usRegionID;
	ChannelID usChannelID;
	RoleRank  usRoleRank;                      
	char      szJoinTime[enmMaxTimeStringLength];                 //����ʱ��
	char      szLastEnterTime[enmMaxTimeStringLength];            //�ϴν��뷿��ʱ��
	int32_t   nTotalTimeInRoom;                                   //�ܹ��ڷ����ʱ��
	int32_t   nTotalOnMicTimeInRoom;                              //�ܹ�����ʱ��
	int32_t   nDevotePoint;                                       //���乱�׵�
	char      szLastGetDevoteTime[enmMaxTimeStringLength];        //�ϴ���ȡ���׵��ʱ��
};

//�������Ա����Ҫ��ѯ���ֶ�
enum
{
	enmRoomMember_RoleID = 0,         //����Ա��roleid
	enmRoomMember_RoleRank,           //����Ա��Ȩ��
	enmRoomMember_InRoomTime,         //�ܹ����ڷ����ʱ��
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
