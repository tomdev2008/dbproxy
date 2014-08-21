/*
 * bll_event_publicasset_msg.cpp
 *
 *  Created on: 2013-3-5
 *      Author: liufl
 */

#include "bll_event_publicasset_msg.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

int32_t CFromPublicUpdateEvent::OnMessagePublicAssetEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer: public update asset!{pMsgHead=0x%08X, pMsgBody=0x%08X}\n",
				pMsgHead, pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//	int32_t threadindex =         需要获取线程ID
	CUpdateAssetReq* pTmpBody = dynamic_cast<CUpdateAssetReq*>(pMsgBody);
	if(pTmpBody == NULL)
	{
		WRITE_ERROR_LOG("get msg body failed!");
		return E_NULLPOINTER;
	}
	CUpdateAssetResp public_updateasset;

	MessageHeadSS resphead;
	ObtainRespHead(pMsgHead, resphead,MSGID_DBOT_UPDATEASSET_RESP);


	ret = ProcessPubUpAssetReq(*pTmpBody, public_updateasset);
	public_updateasset.nResult = ret;
	if(0 > ret)
	{
		WRITE_ERROR_LOG("Error: process public update asset request failed! errorcode=0x%08X\n",
				ret);
		//直接返回

		g_Frame.PostMessage(&resphead, &public_updateasset, nOptionLen, (const char*)pOptionData);
		DumpMessage("", &resphead, &public_updateasset, nOptionLen, (const char*)pOptionData);
		return ret;
	}
	WRITE_DEBUG_LOG( "process public update asset success! \n");

	//直接返回
	public_updateasset.nResult = S_OK;
	g_Frame.PostMessage(&resphead, &public_updateasset, nOptionLen, (const char*)pOptionData);
	DumpMessage("", &resphead, &public_updateasset, nOptionLen, (const char*)pOptionData);

	return S_OK;
}
int32_t CFromPublicUpdateEvent::ProcessPubUpAssetReq(const CUpdateAssetReq& reqbody, CUpdateAssetResp& respbody)
{
	int32_t ret = S_OK;

	respbody.nResult = S_OK;
	respbody.nRoleID = reqbody.nRoleID;
	respbody.strTransID = reqbody.strTransID;
	respbody.nUpdateCount = 0;

	//账单引擎对象
	CBillEngine& billEngine = GET_BILLENGINE_INSTANCE();
	//char szBillTransID[enmMaxBillTransIDLen] = {0};

	//时间类
	CDateTime dt = CDateTime::CurrentDateTime();
	char szDateTime[enmMaxTimeStringLength] = {0};
	dt.ToDateTimeString(szDateTime);

	//缓存操作必须的参数声明
	char szKey[enmMaxMemcacheKeyLen] = {0};
	size_t keylen = 0;
	char szVal[enmMaxMemcacheValueLen] = {0};
	size_t vallen = 0;
	uint32_t uTmpValLen = 0;

	//memcache处理类对象
	CVDCMemcache& cacheobj = GET_VDCMEMCACHE_INSTANCE();

	//用户基本信息表结构
	RoleBaseInfo rolebaseinfo;

	//更新用户资产前，需要先查询用户的剩余资产，然后做下一步决定
	uint32_t nLeftMoney = 0;
	if (reqbody.nCauseID == enmCauseID_AcceptRequestSong)
	{
		ret = UpdateMagnate(reqbody.nRoleID, reqbody.nMagnateLevel, reqbody.nSpendMoneySum, reqbody.nNextLevelSpend);
		if(0 > ret)
		{
			WRITE_ERROR_LOG("update player magnatelevel about field failed! errorcode=0x%08X, roleid=%d\n",
					ret, reqbody.nRoleID);
			return ret;
		}
	}
	else {
		//循环更新个人信息
		for(int32_t i=0; i<reqbody.nUpdateCount; ++i)
		{
			//更新数据库
			if(reqbody.arrAssetType[i] != enmAssetType_Money)
			{
				WRITE_WARNING_LOG( "asset type is not money, so just ignore it and continue! roleid=%d, assettype=%d, assetValue=%d \n",
						reqbody.nRoleID,
						reqbody.arrAssetType[i],
						reqbody.arrAssetValue[i]);
				continue;
			}
//			WRITE_DEBUG_LOG( "asset type is money, and we will update it! roleid=%d, assetType=%d, update amonut is %d \n",
//					reqbody.nRoleID,
//					reqbody.arrAssetType[i],
//					reqbody.arrAssetValue[i]);

			ret = UpdateRoleAsset(reqbody.nRoleID, reqbody.arrUpdateAssetOperate[i], reqbody.arrAssetValue[i], reqbody.strTransID.GetString(), nLeftMoney);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "update role asset failed! roleid=%d, asset value=%d, errorcode=0x%08X \n",
						reqbody.nRoleID,
						reqbody.arrAssetValue[i],
						ret);
				//此处需写资产更新失败账单
				billEngine.WriteUpdateAssetFailBill(
						reqbody.strTransID.GetString(),
						reqbody.nRoleID,
						nLeftMoney,
						reqbody.arrUpdateAssetOperate[i],
						reqbody.arrAssetValue[i],
						reqbody.nCauseID,
						szDateTime,
						ret);
				return ret;
			}
//			WRITE_DEBUG_LOG( "update role asset success! roleid=%d, assettype=%d, asset value=%d \n",
//					reqbody.nRoleID,
//					reqbody.arrAssetType[i],
//					reqbody.arrAssetValue[i]);

			//更新富豪等级相关字段
			ret = UpdateMagnate(reqbody.nRoleID, reqbody.nMagnateLevel, reqbody.nSpendMoneySum, reqbody.nNextLevelSpend);
			if(0 > ret)
			{
				WRITE_ERROR_LOG( "update player magnatelevel about field failed! errorcode=0x%08X, roleid=%d\n",
						ret, reqbody.nRoleID);
				return ret;
			}

			respbody.arrAssetType[respbody.nUpdateCount] = reqbody.arrAssetType[i];
			respbody.arrAssetValue[respbody.nUpdateCount] = reqbody.arrAssetValue[i];
			respbody.arrUpdateAssetOperate[respbody.nUpdateCount] = reqbody.arrUpdateAssetOperate[i];
			respbody.nUpdateCount++;

			//写资产更新账单
			billEngine.WriteUpdateAssetBill(reqbody.strTransID.GetString(), reqbody.nRoleID, reqbody.nCauseID, szDateTime, reqbody.arrAssetValue[i], nLeftMoney);
		}
	}
//	//删除网站的cache信息: 首先生成web那边的key的格式，然后删除之
//	GenerateMemcacheKeyForWeb(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoleID, reqbody.nRoleID);
//	ret = cacheobj.MemcacheDel(cacheobj.m_memc, szKey, keylen, 0);
//	if(0 > ret)
//	{
//		WRITE_ERROR_LOG( "memcache delete failed! errorcode=0x%08X, key=%s, keylen=%d\n",
//				ret,
//				szKey,
//				keylen);
//		return ret;
//	}
//	WRITE_DEBUG_LOG( "memcache delete success! key=%s, keylen=%d\n",
//			szKey,
//			keylen);

	//将用户基本信息结构恢复到初始状态
	CVDCUserBaseInfo::VDCUserBaseInfoInit(rolebaseinfo);

	//查询该玩家基本信息
	ret = QueryRoleBaseInfo(reqbody.nRoleID, rolebaseinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "query role base info failed! roleid=%d, errorcode=0x%08X \n",
				reqbody.nRoleID, ret);
		return ret;
	}
//	WRITE_DEBUG_LOG( "query role base info from db success! roleid=%d \n",
//			reqbody.nRoleID);

	//构造缓存信息
	memset(szVal, 0, sizeof(szVal));
	memset(szKey, 0, sizeof(szKey));
	keylen = 0;
	vallen = 0;
	uTmpValLen = 0;
	GenerateMemcacheKey(szKey, enmMaxMemcacheKeyLen, keylen, enmStoreType_RoleID, reqbody.nRoleID);
	ret = CVDCUserBaseInfo::VDCUserBaseInfoEncode((uint8_t*)szVal, enmMaxMemcacheValueLen, uTmpValLen, rolebaseinfo);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "encode role base info into memcache buffer faield! errorcode=0x%08X, roleid=%d \n",
				ret, reqbody.nRoleID);
		return ret;
	}
//	WRITE_DEBUG_LOG( "encode role base info into memcache buffer success@! congratulations! roleid=%d \n",
//			reqbody.nRoleID);
	vallen = (size_t)uTmpValLen;

	//改写缓存
	ret = cacheobj.MemcacheSet(cacheobj.m_memc, szKey, keylen, szVal, vallen, 0);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "write player info into memcache failed! key=%s, errorcode=0x%08X \n",
				szKey, ret);
		return ret;
	}
	//WRITE_DEBUG_LOG( "write player info into memcache success! key=%s \n", szKey);

	//响应中的资产更新成功人数增加1个
	respbody.nResult = S_OK;

	return S_OK;
}
int32_t CFromPublicUpdateEvent::UpdateRoleAsset(const RoleID roleid, const UpdateAssetOperate opmod, const uint32_t nMoney, const char* szTransID, uint32_t& nLeftMoney)
{
	int32_t ret = S_OK;

	if(0 == nMoney)
	{
		WRITE_WARNING_LOG( "update amount is zero , so just ignore it and return! transid=%s\n", szTransID);
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "update amount is not zero, so need to update it! transid=%s\n", szTransID);

	char szSql[enmMaxSqlStringLength] = {0};
	uint32_t nFieldCount = 0;
	uint32_t nRowCount = 0;
	char arrRecordSet[enmMaxFieldCount][enmMaxFieldValueLength];
	//memset(arrRecordSet, 0, sizeof(arrRecordSet));

	//首先查询余额
	sprintf(szSql, "select `Money` from vdc_user.user_base_info where `RoleID`=%d", roleid);
	ret = MYSQLREADENGINE.ExecuteQuery(szSql, arrRecordSet, enmMaxFieldValueLength, nFieldCount, nRowCount);
	if(0 > ret)
	{
		WRITE_ERROR_LOG( "db operation of query player money failed! roleid=%d, errorcode=0x%08X, sql=%s\n",
				roleid, ret, szSql);
		return ret;
	}
	nLeftMoney = (uint32_t)atoi(arrRecordSet[0]);
//	WRITE_DEBUG_LOG( "db operation of query player left money success! roleid=%d, nLeftMoney=%u, "
//			"opmode=%d(1:add, 2:decrease, other: invalide), updateAmount=%u, sql=%s\n",
//			roleid, nLeftMoney, opmod, nMoney, szSql);

	//memset(szSql, 0, sizeof(szSql));
	switch(opmod)
	{
		case enmUpdateAssetOperate_Increase:
		if((nLeftMoney + nMoney) > (uint32_t)enmMaxPlayerMoney)
		{
			WRITE_ERROR_LOG( "player left money plus nMoney is more than max player money limit! \
												roleid=%d, leftmoney=%u, nMoney=%u, plusresult=%u, transid=%s\n",
					roleid, nLeftMoney, nMoney, nLeftMoney + nMoney, szTransID);
			return E_OVERRUN_MAX_PLAYER_MONEY;
		}
		sprintf(szSql, "update vdc_user.user_base_info set `Money`=`Money`+%u where `RoleID`=%d and `Money`+%u <= %u",
				nMoney, roleid, nMoney, (uint32_t)enmMaxPlayerMoney);
		break;
		case enmUpdateAssetOperate_Reduce:
		if(nLeftMoney < nMoney)
		{
			WRITE_ERROR_LOG( "player left money is not enough!\
												roleid=%d, leftmoney=%u, nMoneyNeed=%u, transid=%s\n",
					roleid, nLeftMoney, nMoney, szTransID);
			return E_LEFTMONEY_NOT_ENOUGH;
		}
		sprintf(szSql, "update vdc_user.user_base_info set `Money`=`Money`-%u where `RoleID`=%d and `Money`-%u <= %u",
				nMoney, roleid, nMoney, (uint32_t)enmMaxPlayerMoney);
		break;
		default:
		WRITE_WARNING_LOG( "undefined asset update opmode! opmode=%d\n", opmod);
		return S_OK;
	}

	uint64_t nAffectedRows = 0;
	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "Error: execute update on role asset failed! roleid=%d, errorcode=0x%08X, transid=%s, sql:%s\n",
				roleid, ret, szTransID, szSql);
		return ret;
	}
//	WRITE_DEBUG_LOG( "execute update on role asset success! roleid=%d, transid=%s, sql=%s\n",
//			roleid, szTransID, szSql);

	if(0 == nAffectedRows)
	{
		WRITE_ERROR_LOG( "Error: execute update role asset sql success but 0 rows affected! transid=%s, roleid=%d, sql=%s\n",
				szTransID, roleid, szSql);
		return E_UPDATE_ZERO_ROW_AFFETCTED;
	}
//	WRITE_DEBUG_LOG( "execute update role asset role asset sql success! szTransID=%s, roleid=%d, sql=%s\n",
//			szTransID, roleid, szSql);

	return S_OK;
}
int32_t CFromPublicUpdateEvent::UpdateMagnate(const RoleID roleid, const MagnateLevel magnatelevel,
		const uint64_t spendsum, const uint64_t nextlevelspend)
{
	int32_t ret = S_OK;

	if(magnatelevel==0 && spendsum==0 && nextlevelspend==0)
	{
		return S_OK;
	}

	//首先将该item的所有奖池的开关设置为关闭状态
	char szSql[enmMaxSqlStringLength] = {0};
	uint64_t nAffectedRows = 0;
	sprintf(szSql, "insert into vdc_user.user_base_info(`RoleID`, `MagnateLevel`, `ConsumeAmt`, `NextLevelConsume`) "
			"values(%d, %d, %ld, %ld) on duplicate key update `MagnateLevel`=%d, `ConsumeAmt`=%ld, `NextLevelConsume`=%ld",
			roleid,
			magnatelevel,
			spendsum,
			nextlevelspend,
			magnatelevel,
			spendsum,
			nextlevelspend);

	ret = MYSQLREADENGINE.ExecuteUpdate(szSql, nAffectedRows);
	if(0>ret)
	{
		WRITE_ERROR_LOG( "error: execute update user magnate level failure! errorcode=0x%08X, sql=%s\n",
				ret,
				szSql);
		return ret;
	}
	//WRITE_DEBUG_LOG( "execute update magnate level success! sql=%s\n", szSql);

	return S_OK;
}

FRAME_DBPROXY_NAMESPACE_END
