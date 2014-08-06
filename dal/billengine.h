/*********************************************************************
* filename: billengine.h
* date:     2011-12-31 15:22
* author:   xiaohq
* purpose:  账单类
* version:  1.0
* revision: not yet
*********************************************************************/

#ifndef DBPROXY_BILL_ENGINE_H_
#define DBPROXY_BILL_ENGINE_H_

#include "common/common_typedef.h"
#include "common/common_singleton.h"
#include "../def/def_dbproxy.h"
#include "../def/def_dbproxy_errorcode.h"
#include "hallserver_message_define.h"
#include "itemserver_message_define.h"
#include "public_typedef.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

class CBillEngine
{
public:
	CBillEngine();
	virtual ~CBillEngine();

public:
	//购买道具请求账单
	//void WritePurchaseItemReqBill(const char* szTransID, const CItemPurchaseItemReq& purchaseitemreq);
	//用户获奖账单
	void WritePlayerPrizeBill(const char* szTransID, const RoleID giverID, const RoleID receiverID, const char* szPrizeTime, const int32_t prizeAmount);
	//用户赠送记录
	void WriteLargessBill(const char* szTransID, const RoleID giverID, const RoleID receiverID, const char* szPrizeTime, const int32_t totalLargessAmount, const int32_t prizeAmountPerPlayer);

	/*
	* function: void WriteUpdateAssetBill, 资产更新账单
	* param causeid: 资产更新原因
	* param szUpdateTime: 资产更新发生的时间
	* param updateAmount: 资产更新数量
	*/
	void WriteUpdateAssetBill(const char* szTransID, const RoleID roleid, const CauseID causeid, const char* szUpdateTime, const int32_t updateAmount, const int32_t nLeftMoney);

	/*
	* function: WriteItemUpdateBill, 玩家物品信息更新账单
	* param: const char* szTranseID, 账单流水号
	* param: const RoleID roleid, 玩家uin
	* param: const ItemUint& itemuint, 物品信息结构
	* param: const CauseID causeid, 物品更新的原因
	* return value:  no return
	*/
	void WriteItemUpdateBill(const char* szTransID, const RoleID roleid, const char* szUpdateTime, const CauseID causeid, const ItemUnit& itemuint);

	/* 更新资产失败账单， 方便查询资产更新失败的原因 */
	void WriteUpdateAssetFailBill(const char* szTransID, 
		const RoleID roleid, 
		const uint32_t left_money, 
		const UpdateAssetOperate opmode, 
		const uint32_t change_value, 
		const CauseID causeid, 
		const char* szUpdateTime, 
		const int32_t errorcode);

public:
	//生成流水号
	void GenerateTransID(char *szTransID);

protected:
	static int32_t ms_nBillNumber;
};

#define  CREATE_BILLENGINE_INSTANCE    CSingleton<CBillEngine>::CreateInstance
#define  GET_BILLENGINE_INSTANCE       CSingleton<CBillEngine>::GetInstance
#define  DESTROY_BILLENGINE_INSTANCE   CSingleton<CBillEngine>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END

#endif
