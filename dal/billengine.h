/*********************************************************************
* filename: billengine.h
* date:     2011-12-31 15:22
* author:   xiaohq
* purpose:  �˵���
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
	//������������˵�
	//void WritePurchaseItemReqBill(const char* szTransID, const CItemPurchaseItemReq& purchaseitemreq);
	//�û����˵�
	void WritePlayerPrizeBill(const char* szTransID, const RoleID giverID, const RoleID receiverID, const char* szPrizeTime, const int32_t prizeAmount);
	//�û����ͼ�¼
	void WriteLargessBill(const char* szTransID, const RoleID giverID, const RoleID receiverID, const char* szPrizeTime, const int32_t totalLargessAmount, const int32_t prizeAmountPerPlayer);

	/*
	* function: void WriteUpdateAssetBill, �ʲ������˵�
	* param causeid: �ʲ�����ԭ��
	* param szUpdateTime: �ʲ����·�����ʱ��
	* param updateAmount: �ʲ���������
	*/
	void WriteUpdateAssetBill(const char* szTransID, const RoleID roleid, const CauseID causeid, const char* szUpdateTime, const int32_t updateAmount, const int32_t nLeftMoney);

	/*
	* function: WriteItemUpdateBill, �����Ʒ��Ϣ�����˵�
	* param: const char* szTranseID, �˵���ˮ��
	* param: const RoleID roleid, ���uin
	* param: const ItemUint& itemuint, ��Ʒ��Ϣ�ṹ
	* param: const CauseID causeid, ��Ʒ���µ�ԭ��
	* return value:  no return
	*/
	void WriteItemUpdateBill(const char* szTransID, const RoleID roleid, const char* szUpdateTime, const CauseID causeid, const ItemUnit& itemuint);

	/* �����ʲ�ʧ���˵��� �����ѯ�ʲ�����ʧ�ܵ�ԭ�� */
	void WriteUpdateAssetFailBill(const char* szTransID, 
		const RoleID roleid, 
		const uint32_t left_money, 
		const UpdateAssetOperate opmode, 
		const uint32_t change_value, 
		const CauseID causeid, 
		const char* szUpdateTime, 
		const int32_t errorcode);

public:
	//������ˮ��
	void GenerateTransID(char *szTransID);

protected:
	static int32_t ms_nBillNumber;
};

#define  CREATE_BILLENGINE_INSTANCE    CSingleton<CBillEngine>::CreateInstance
#define  GET_BILLENGINE_INSTANCE       CSingleton<CBillEngine>::GetInstance
#define  DESTROY_BILLENGINE_INSTANCE   CSingleton<CBillEngine>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END

#endif
