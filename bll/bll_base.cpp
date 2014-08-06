/*
 * bll_base.cpp
 *
 *  Created on: 2011-12-2
 *      Author: jimm
 */

#include "bll_base.h"
#include "public_typedef.h"
#include "../util.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

static ConnUin g_arrConnUin[MaxUserCountPerRoom];

/*
 发送请求消息
 */
void CBllBase::SendMessageRequest(uint32_t nMsgID, IMsgBody* pMsgBody, const RoomID nRoomID, TransType nTransType,
		RoleID nRoleID, EntityType nDestType, const int32_t nDestID, const SessionIndex nSessionIndex,
		const uint16_t nOptionLen, const char *pOptionData, const char *szDumpContent)
{
	MessageHeadSS stSendMessage;
//	memset(&stSendMessage, 0, sizeof(stSendMessage));
	stSendMessage.nMessageID = nMsgID;
	stSendMessage.nMessageType = enmMessageType_Request;
	stSendMessage.nSourceType = g_FrameConfigMgt.GetFrameBaseConfig().GetServerType();
	stSendMessage.nDestType = nDestType;
	stSendMessage.nSourceID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	stSendMessage.nDestID = nDestID;
	stSendMessage.nRoleID = nRoleID;
	stSendMessage.nSequence = 0;
	stSendMessage.nTransType = nTransType;
	stSendMessage.nSessionIndex = nSessionIndex;
	stSendMessage.nRoomID = nRoomID;
	stSendMessage.nZoneID = 0;
	stSendMessage.nRouterIndex = 0;

	g_Frame.PostMessage(&stSendMessage, pMsgBody, nOptionLen, pOptionData);
	DumpMessage(szDumpContent, &stSendMessage, pMsgBody, nOptionLen, pOptionData);
}
/*
 发送相应消息
 */
void CBllBase::SendMessageResponse(uint32_t nMsgID,MessageHeadSS * pMsgHead,IMsgBody* pMsgBody,
		TransType nTransType, const uint16_t nOptionLen, const char *pOptionData,const char *szDumpContent)
{
	MessageHeadSS stSendMessage;
//	memset(&stSendMessage, 0, sizeof(stSendMessage));

	stSendMessage.nMessageID = nMsgID;
	stSendMessage.nMessageType = enmMessageType_Response;
	stSendMessage.nSourceType = g_FrameConfigMgt.GetFrameBaseConfig().GetServerType();
	stSendMessage.nDestType = pMsgHead->nSourceType;
	stSendMessage.nSourceID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	stSendMessage.nDestID = pMsgHead->nSourceID;
	stSendMessage.nRoleID = pMsgHead->nRoleID;
	stSendMessage.nSequence = pMsgHead->nSequence;
	stSendMessage.nTransType = nTransType;
	stSendMessage.nSessionIndex = pMsgHead->nSessionIndex;
	stSendMessage.nRoomID = pMsgHead->nRoomID;
	stSendMessage.nZoneID = pMsgHead->nZoneID;
	if(stSendMessage.nDestType == enmEntityType_Tunnel)
	{
		ConnUin stInfo;
		uint32_t offset = 0;
		int32_t ret = stInfo.MessageDecode((uint8_t *)pOptionData, (uint32_t)nOptionLen, offset);
		if(ret < 0)
		{
			WRITE_ERROR_LOG("send message response:decode connuin failed!{ret=0x%08x, nRoleID=%d}\n", ret, pMsgHead->nRoleID);
			return;
		}
		stSendMessage.nRouterIndex = 0;//g_FrameConfigMgt.GetRouterIndex(stInfo.nRouterAddress, stInfo.nRouterPort);
	}
	else
	{
		stSendMessage.nRouterIndex = pMsgHead->nRouterIndex;
	}

	g_Frame.PostMessage(&stSendMessage, pMsgBody, nOptionLen, pOptionData);
	DumpMessage(szDumpContent,&stSendMessage, pMsgBody, nOptionLen, pOptionData);
}

void CBllBase::SendMessageNotifyToServer(uint32_t nMsgID,IMsgBody* pMsgBody,const RoomID nRoomID,const TransType nTransType,
		const RoleID nRoleID,const EntityType nDestType,const int32_t nDestID,
		const uint16_t nOptionLen, const char *pOptionData,const char *szDumpContent)
{
	MessageHeadSS stSendMessage;
//	memset(&stSendMessage, 0, sizeof(stSendMessage));
	stSendMessage.nMessageID = nMsgID;
	stSendMessage.nMessageType = enmMessageType_Notify;
	stSendMessage.nSourceType = g_FrameConfigMgt.GetFrameBaseConfig().GetServerType();
	stSendMessage.nDestType = nDestType;
	stSendMessage.nSourceID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	stSendMessage.nDestID = nDestID;
	stSendMessage.nRoleID = nRoleID;
	stSendMessage.nSequence = 0;
	stSendMessage.nTransType = nTransType;
	stSendMessage.nSessionIndex = enmInvalidSessionIndex;
	stSendMessage.nRoomID = nRoomID;
	stSendMessage.nZoneID = 0;//g_FrameConfigMgt.GetZoneID(enmRouterType_Sync);
	stSendMessage.nRouterIndex = 0;

	g_Frame.PostMessage(&stSendMessage, pMsgBody, nOptionLen, pOptionData);
	DumpMessage(szDumpContent,&stSendMessage,pMsgBody, nOptionLen, pOptionData);
}

/*
 发送通知消息
 */
void CBllBase::SendMessageNotify(const RoleID nRoleID, IMsgBody* pMsgBody,uint32_t nMsgID,
		const char *szDumpContent, const RoomID nRoomID, const uint16_t nOptionLen, const char *pOptionData)
{
	MessageHeadSS stSendMessage;
//	memset(&stSendMessage, 0, sizeof(stSendMessage));
	stSendMessage.nMessageID = nMsgID;
	stSendMessage.nMessageType = enmMessageType_Notify;
	stSendMessage.nSourceType = g_FrameConfigMgt.GetFrameBaseConfig().GetServerType();
	stSendMessage.nDestType = enmEntityType_Hall;
	stSendMessage.nSourceID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	stSendMessage.nDestID = 0;
	stSendMessage.nRoleID = nRoleID;
	stSendMessage.nSequence = 0;
	stSendMessage.nTransType = enmTransType_ByKey;
	stSendMessage.nSessionIndex = enmInvalidSessionIndex;
	stSendMessage.nRoomID = nRoomID;
	stSendMessage.nZoneID = 0;
	stSendMessage.nRouterIndex = 0;

	g_Frame.PostMessage(&stSendMessage, pMsgBody, nOptionLen, pOptionData);
	DumpMessage(szDumpContent,&stSendMessage,pMsgBody, nOptionLen, pOptionData);
}

//发送广播消息给房间内所有玩家
void CBllBase::SendMessageNotifyToClient(uint32_t nMsgID, IMsgBody* pMsgBody, RoomID nRoomID, BroadcastType nType,
		RoleID nRoleID, const uint16_t nOptionLen,const char *pOptionData, const char *szDumpContent)
{
	MessageHeadSS stSendMessage;
//	memset(&stSendMessage, 0, sizeof(stSendMessage));
	stSendMessage.nMessageID = nMsgID;
	stSendMessage.nMessageType = enmMessageType_Notify;
	stSendMessage.nSourceType = g_FrameConfigMgt.GetFrameBaseConfig().GetServerType();
	stSendMessage.nDestType = enmEntityType_Tunnel;
	stSendMessage.nSourceID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	//	stSendMessage.nDestID = 0;
	stSendMessage.nSequence = 0;
	stSendMessage.nTransType = enmTransType_P2P;
	stSendMessage.nSessionIndex = enmInvalidSessionIndex;
	stSendMessage.nRoomID = nRoomID;
	stSendMessage.nRoleID = enmInvalidRoleID;
	stSendMessage.nZoneID = 0;//g_FrameConfigMgt.GetZoneID(enmRouterType_Default);

	if(enmBroadcastType_ExpectPlayr == nType)
	{
//		CPlayer *pPlayer = NULL;
//		PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
//		ret = g_PlayerMgt.GetPlayer(nRoleID, pPlayer);
//		if(ret < 0)
//		{
//			WRITE_ERROR_LOG("get expect player object error!{RoomID=%d, ret=0x%08x}\n", nRoomID, ret);
//			return;
//		}
//
//		stSendMessage.nRoleID = nRoleID;
//		stSendMessage.nDestID = pPlayer->GetConnInfo().nServerID;
//		stSendMessage.nRouterIndex = 0;//g_FrameConfigMgt.GetRouterIndex(pPlayer->GetConnInfo().nRouterAddress, pPlayer->GetConnInfo().nRouterPort);

		g_Frame.PostMessage(&stSendMessage, pMsgBody, nOptionLen, pOptionData);
		DumpMessage(szDumpContent,&stSendMessage,pMsgBody, nOptionLen, pOptionData);
	}
	else if(enmBroadcastType_ExceptPlayr == nType)
	{
		stSendMessage.nTransType = enmTransType_Broadcast;
		stSendMessage.nDestID = enmInvalidServerID;

		stSendMessage.nRoleID = nRoleID;
		stSendMessage.nRouterIndex = 0;

		g_Frame.PostMessage(&stSendMessage, pMsgBody, nOptionLen, pOptionData);
		DumpMessage(szDumpContent,&stSendMessage,pMsgBody, nOptionLen, pOptionData);
	}
	else if(enmBroadcastType_All == nType)
	{
		stSendMessage.nTransType = enmTransType_Broadcast;
		stSendMessage.nDestID = enmInvalidServerID;
		stSendMessage.nRouterIndex = 0;

		g_Frame.PostMessage(&stSendMessage, pMsgBody, nOptionLen, pOptionData);
		DumpMessage(szDumpContent,&stSendMessage,pMsgBody, nOptionLen, pOptionData);
	}
}

void CBllBase::SendMessageNotify(const RoleID nRoleID, const EntityType nDestType ,
		const TransType nTransType, IMsgBody* pMsgBody,uint32_t nMsgID,const char *szDumpContent,
		const RoomID nRoomID, const uint16_t nOptionLen, const char *pOptionData)
{
	MessageHeadSS stSendMessage;
//	memset(&stSendMessage, 0, sizeof(stSendMessage));
	stSendMessage.nMessageID = nMsgID;
	stSendMessage.nMessageType = enmMessageType_Notify;
	stSendMessage.nSourceType = g_FrameConfigMgt.GetFrameBaseConfig().GetServerType();
	stSendMessage.nDestType = nDestType;
	stSendMessage.nSourceID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	stSendMessage.nDestID = 0;
	stSendMessage.nRoleID = nRoleID;
	stSendMessage.nSequence = 0;
	stSendMessage.nTransType = nTransType;
	stSendMessage.nSessionIndex = enmInvalidSessionIndex;
	stSendMessage.nRoomID = nRoomID;
	stSendMessage.nZoneID = 0;
	stSendMessage.nRouterIndex = 0;

	g_Frame.PostMessage(&stSendMessage, pMsgBody, nOptionLen, pOptionData);
	DumpMessage(szDumpContent,&stSendMessage,pMsgBody, nOptionLen, pOptionData);
}

void CBllBase::ObtainRespHead(const MessageHeadSS* reqHead, MessageHeadSS& respHead, const uint32_t nMsgID)
{
	respHead.nTotalSize = 0;
	respHead.nMessageID = nMsgID;
	respHead.nHeadSize = GetMsgHeadSSMinHeadLen();
	respHead.nMessageType = enmMessageType_Response;
	respHead.nSourceType = reqHead->nDestType;
	respHead.nSourceID = reqHead->nDestID;
	respHead.nDestType = reqHead->nSourceType;
	respHead.nDestID = reqHead->nSourceID;
	respHead.nRoleID = reqHead->nRoleID;
	respHead.nSequence = reqHead->nSequence;
	respHead.nTransType = enmTransType_P2P;
	respHead.nSessionIndex = reqHead->nSessionIndex;
	respHead.nRoomID = reqHead->nRoomID;
	respHead.nZoneID = reqHead->nZoneID;
	respHead.nRouterIndex = reqHead->nRouterIndex;

}

/*
 打印接受，发送的消息
 */
void CBllBase::DumpMessage(const char* szContent, MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const char *pOptionData)
{
	uint32_t offset = 0;
	char szLog[enmMaxLogInfoLength];
	szLog[0] = 0;
	sprintf(szLog + offset, szContent);
	offset = (uint32_t)strlen(szLog);

	sprintf(szLog + offset, " MessageHead=");
	offset = (uint32_t)strlen(szLog);

	sprintf(szLog + offset, "{nMessageID=0x%08x, nMessageType=%d, nSourceType=%d, nDestType=%d, "
			"nSourceID=%d, nDestID=%d, nRoleID=%d, nSequence=%d, nTransType=%d, nSessionIndex=%d, "
			"nRoomID=%d, nZoneID=%d, nRouterIndex=%d, nOptionLen=%d}",
			pMsgHead->nMessageID, pMsgHead->nMessageType, pMsgHead->nSourceType,
			pMsgHead->nDestType, pMsgHead->nSourceID, pMsgHead->nDestID, pMsgHead->nRoleID,
			pMsgHead->nSequence, pMsgHead->nTransType, pMsgHead->nSessionIndex, pMsgHead->nRoomID,
			pMsgHead->nZoneID, pMsgHead->nRouterIndex, nOptionLen);
	offset = (uint32_t)strlen(szLog);

	sprintf(szLog + offset, " MessageBody=");
	offset = (uint32_t)strlen(szLog);
	pMsgBody->Dump(szLog, enmMaxLogInfoLength, offset);

	WRITE_NOTICE_LOG("%s\n", szLog);
}

CMysqlEngine& CBllBase::GetMysqlReadEngine()
{
	CThread * pCurThread = g_FrameThreadMgt.GetCurThread();
//	if (NULL == pCurThread)
//	{
//		return NULL;
//	}
	return g_Frame.GetDBEngine(pCurThread->GetThreadIndex())->m_mysqlReadEngine;
}
FRAME_DBPROXY_NAMESPACE_END

