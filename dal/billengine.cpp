/*********************************************************************
* filename:billengine.cpp
* date:    2011-12-31 16:31
* author:  xiaohq
* purpose: 账单类各方法的实现
* version: 1.0
* revision: not yet
*********************************************************************/

#include "common/common_def.h"
#include "common/common_datetime.h"
#include "common/common_api.h"
#include "billengine.h"
#include "frame_configmgt.h"
#include "../../public/public_typedef.h"
#include "../../public/itemserver_message_define.h"
#include "../../public/hallserver_message_define.h"
#include "../dal/dal_player.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

#define AWARDREQ_BILL_PREFIX			"award-request-bill"
#define AWARDRES_BILL_PREFIX			"award-response-bill"

#define YYGIFT_REQUEST_BLL				"./bill/yygift/update-item-request-bill"
#define YYGIFT_RESPONSE_BLL				"./bill/yygift/update-item-response-bill"

#define UPDATEITEMREQ_BILL_PREFIX		"./bill/activity/update-item-request-bill"

#define PURCHASE_ITEM_REQ_BILL           "purchase_item_request_bill"

#define PLAYER_PRIZE_BILL                "player_prize_bill"

#define PLAYER_LARGESS_BILL              "player_largess_bill"

#define USER_ASSET_UPDATE_BILL    "player_asset_update_bill"

/* 玩家物品更新账单宏定义 */
#define USER_ITEM_UPDATE_BILL     "player_item_update_bill"

/* 资产更新失败账单宏定义： 记录更新失败时的场景，便于查询失败的原因 */
#define UPDATE_ASSET_FAIL_BILL   "asset_update_fail_bill"

int32_t  CBillEngine::ms_nBillNumber = 0;

CBillEngine::CBillEngine()
{
}

CBillEngine::~CBillEngine()
{
}

void CBillEngine::GenerateTransID(char *szTransID)
{
	CDateTime dt = CDateTime::CurrentDateTime();
	CFrameBaseConfig& framecofig = g_FrameConfigMgt.GetFrameBaseConfig();
	sprintf(szTransID, "Z%02dTP%02dID%03dTM%08XSQ%04X", framecofig.GetZoneID(),enmEntityType_DBProxy,  framecofig.GetServerID(), (uint32_t)dt.Seconds(), ++ms_nBillNumber);
}

////参数依次为：流水号
//void CBillEngine::WritePurchaseItemReqBill(const char* szTransID, const CItemPurchaseItemReq& purchaseitemreq)
//{
//	char szFileName[enmMaxFileNameLength] = {0};
//	CDateTime dt = CDateTime::CurrentDateTime();
//	sprintf(szFileName, "./bill/dbproxy/%s_%04d_%02d_%02d_%d.txt", PURCHASE_ITEM_REQ_BILL, dt.Year(), dt.Month(), dt.Day(), dt.Hour());
//	//CFrameConfig& frameconfig = GET_FRAMECONFIG_INSTANCE();
//	WriteBill(szFileName, "%s, %d, %d, %s, %d, %s, %d\n", 
//		szTransID, 
//		purchaseitemreq.nPurchaseSeq, 
//		purchaseitemreq.nItemID,
//		purchaseitemreq.szItemName, 
//		purchaseitemreq.nFromRoleID,
//		purchaseitemreq.szFromRoleName,
//		purchaseitemreq.nToRoleIDCount);
//
//	for(int32_t i=0; i<purchaseitemreq.nToRoleIDCount; ++i)
//	{
//		WriteBill(szFileName, "%d", purchaseitemreq.arrToRoleIDs[i]);
//	}
//
//	WriteBill(szFileName, "%d, %d, %d, %s", 
//		purchaseitemreq.nPrice,
//		purchaseitemreq.nBenifit,
//		purchaseitemreq.nItemCount,
//		purchaseitemreq.szPurchaseTime);
//}

//用户获奖记录账单
void CBillEngine::WritePlayerPrizeBill(const char* szTransID, const RoleID giverID, const RoleID receiverID, const char* szPrizeTime, const int32_t prizeAmount)
{
	char szFileName[enmMaxFileNameLength] = {0};
	CDateTime dt = CDateTime::CurrentDateTime();
	sprintf(szFileName, "./bill/dbproxy/%s_%04d_%02d_%02d_%d.txt", PLAYER_PRIZE_BILL, dt.Year(), dt.Month(), dt.Day(), dt.Hour());
	WriteBill(szFileName, "%s, %s, %d, %d, %d\n",
		szTransID,
		szPrizeTime,
		giverID,
		receiverID,
		prizeAmount);
}

void CBillEngine::WriteLargessBill(const char* szTransID, const RoleID giverID, const RoleID receiverID, const char* szPrizeTime, const int32_t totalLargessAmount, const int32_t prizeAmountPerPlayer)
{
	char szFileName[enmMaxFileNameLength] = {0};
	CDateTime dt = CDateTime::CurrentDateTime();
	sprintf(szFileName, "./bill/dbproxy/%s_%04d_%02d_%02d_%d.txt", PLAYER_LARGESS_BILL, dt.Year(), dt.Month(), dt.Day(), dt.Hour());
	WriteBill(szFileName, "%s, %s, %d, %d, %d, %d\n",
		szTransID,
		szPrizeTime,
		giverID,
		receiverID,
		totalLargessAmount,
		prizeAmountPerPlayer);	
}

void CBillEngine::WriteUpdateAssetBill(const char* szTransID, const RoleID roleid, const CauseID causeid, const char* szUpdateTime, const int32_t updateAmount, const int32_t nLeftMoney)
{
	char szFileName[enmMaxFileNameLength] = {0};
	CDateTime dt = CDateTime::CurrentDateTime();
	sprintf(szFileName, "./bill/dbproxy/%s_%04d_%02d_%02d_%02d.txt", USER_ASSET_UPDATE_BILL, dt.Year(), dt.Month(), dt.Day(), dt.Hour());
	WriteBill(szFileName, "transid=%s,roleid=%d,leftmoney=%d,causeid=%d,updateamount=%d\n", szTransID,  roleid, nLeftMoney, causeid, updateAmount);
}

void CBillEngine::WriteItemUpdateBill(const char* szTransID, const RoleID roleid, const char* szUpdateTime, const CauseID causeid, const ItemUnit& itemuint)
{
	char szFileName[enmMaxFileNameLength] = {0};
	CDateTime dt = CDateTime::CurrentDateTime();
	sprintf(szFileName, "./bill/dbproxy/%s_%04d_%02d_%02d_%02d.txt", USER_ITEM_UPDATE_BILL, dt.Year(), dt.Month(), dt.Day(), dt.Hour());
	WriteBill(szFileName, "%s, %d, %d, %d, %d, %d, %d\n", 
		szTransID, 
		roleid, 
		causeid, 
		itemuint.nItemID,
		itemuint.nUseMeans,
		itemuint.nQuantity,
		itemuint.nOwnTick);
}

//void CBillEngine::WriteUpdateAssetFailBill(reqbody.strTransID.GetString(), 
//									nLeftMoney, 
//									reqbody.nRoleID, reqbody.arrUpdateAssetOperate[i], 
//									reqbody.arrAssetValue[i], 
//									reqbody.nCauseID, 
//									szDateTime, 
//									ret);

void CBillEngine::WriteUpdateAssetFailBill(const char* szTransID, 
										   const RoleID roleid, 
										   const uint32_t left_money, 
										   const UpdateAssetOperate opmode, 
										   const uint32_t change_value, 
										   const CauseID causeid, 
										   const char* szUpdateTime, 
										   const int32_t errorcode)
{
	char szFileName[enmMaxFileNameLength] = {0};
	CDateTime dt = CDateTime::CurrentDateTime();
	sprintf(szFileName, "./bill/dbproxy/%s_%04d_%02d_%02d_%02d.txt", UPDATE_ASSET_FAIL_BILL, dt.Year(), dt.Month(), dt.Day(), dt.Hour());
	char szFailReason[MaxAssetUpdateFailReasonLength] = {0};
	switch(errorcode)
	{
	case E_OVERRUN_MAX_PLAYER_MONEY:
		strcpy(szFailReason, "user left money plus change amount is above maximun player money!");
		break;
	case E_LEFTMONEY_NOT_ENOUGH:
		strcpy(szFailReason, "user left money is not enough!");
		break;
	case E_UPDATE_ZERO_ROW_AFFETCTED:
		strcpy(szFailReason, "no error happens while execute update sql, but 0 rows affected!");
		break;
	default:
		strcpy(szFailReason, "unknown error happend while update user asset!");
		break;
	}
	WriteBill(szFileName, "szTransID=%s, \
						  player 179uin=%d, \
						  before update, the players left money=%d, \
						  update mode=%d(0:invalid, 1:add, 2:decrease), \
						  change amount=%d, \
						  causeid=%d, \
						  update time=%s, \
						  fail reason=%s\n", 
						  szTransID,  
						  roleid,
						  left_money, 
						  opmode, 
						  change_value, 
						  causeid, 
						  szUpdateTime, 
						  szFailReason);
}

//用户获奖记录账单
//void CBillEngine::WritePlayerPrizeBill(const char* szTransID, const VDCPlayerPrizeInfo& playerprize)
//{
//	char szFileName[enmMaxFileNameLength] = {0};
//	CDateTime dt = CDateTime::CurrentDateTime();
//	sprintf(szFileName, "./bill/dbproxy/%s_%04d_%02d_%02d_%d.txt", PLAYER_PRIZE_BILL, dt.Year(), dt.Month(), dt.Day(), dt.Hour());
//	WriteBill(szFileName, "%d, %d, %s, %d, ")
//}

//int CBllBillEngine::WriteMailRequestBill(const char* content, int32_t from) 
//{
//	char szFileName[enmMaxFileNameLength] = { 0 };
//	CDateTime dt = CDateTime::CurrentDateTime();
//
//	sprintf(szFileName, "./bill/activity/%s_%04d_%02d_%02d_%02d.txt", AWARDREQ_BILL_PREFIX, dt.Year(), dt.Month(), dt.Day(), dt.Hour());
//
//	WriteBill(szFileName, "%s\n", content);
//
//	return S_OK;
//}

////累计在线送礼活动 送游戏币代币51点经验值
//void CBllBillEngine::WriteTotalTimeAssetUpdateRequestBill(const RoleID roleID, const char* szTransID, const EGSMPrize stEGSMPrize)
//{
//	char szFileName[enmMaxFileNameLength] = { 0 };
//	CDateTime dt = CDateTime::CurrentDateTime();
//
//	sprintf(szFileName, "./bill/activity/%s_%04d_%02d_%02d_%02d.txt", AWARDREQ_BILL_PREFIX, dt.Year(), dt.Month(), dt.Day(), dt.Hour());
//
//	WriteBill(szFileName, "%s,%d,%d,%s,%s,%d,%d,%d,%d,%d,%d,%d,%s,%s\n",
//		szTransID, GET_CONFIG_INSTANCE().GetZoneID(),  roleID, "", "", 0, MSGID_CGDB_REQ_UPDATEASSET, roleID, stEGSMPrize.nExperience, stEGSMPrize.nGameCoin, stEGSMPrize.nScore , stEGSMPrize.nMoney, "", "");
//}

//void CBllBillEngine::WriteBuyReturnCoinRequestBill(const RoleID roleID, const char* szTransID, const int32_t nTotalGameCoinAmount, const int32_t nItemUnitCount, const SingleGoodsItem arrItemUnit[])
//{
//	char szFileName[enmMaxFileNameLength] = {0};
//	CDateTime dt = CDateTime::CurrentDateTime();
//
//	sprintf(szFileName, "./bill/activity/%s_%04d_%02d_%02d_%02d.txt", AWARDREQ_BILL_PREFIX, dt.Year(), dt.Month(), dt.Day(), dt.Hour());
//
//	char szMsg[128] = { 0 };
//	strncpy(szMsg, "buy return game coin activity", sizeof(szMsg));
//
//	for (int32_t i = 0; i < nItemUnitCount; ++i)
//	{
//		WriteBill(szFileName, 
//			"%s,%d,%d,%s,"
//			"%s,%d,%d,%d,%s,"
//			"%d,%d,%d,%d,"
//			"%d,%s\n", 
//			szTransID, GET_CONFIG_INSTANCE().GetZoneID(), roleID, "",
//			"", enmGender_Unknown, nTotalGameCoinAmount, MSGID_CIDB_REQ_ADDITEM, "buy return game coin activity", 
//			enmEntityType_Activity, enmUpdateMeans_Add, arrItemUnit[i].goodsID, arrItemUnit[i].useMeans, 
//			arrItemUnit[i].quantity, szMsg);
//
//	}
//}

FRAME_DBPROXY_NAMESPACE_END

