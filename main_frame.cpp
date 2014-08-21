/*
 * main_frame.cpp
 *
 *  Created on: 2011-12-2
 *      Author: jimm
 */

#include "frame_logengine.h"
#include "main_frame.h"

#include "frame_eventid.h"
#include "frame_logengine.h"
#include "frame_netthread.h"
#include "def/def_dbproxy_errorcode.h"

#include "config/memcache_config.h"
#include "client_message_define.h"

#include "hallserver_message_define.h"
#include "bll/bll_event_hallget_msg.h"
#include "bll/bll_event_hallset_msg.h"
#include "bll/bll_event_roomget_msg.h"
#include "bll/bll_event_roomadd_msg.h"
#include "bll/bll_event_roomset_msg.h"
#include "bll/bll_event_itemget_msg.h"
#include "bll/bll_event_itemupdate_msg.h"
#include "bll/bll_event_publicasset_msg.h"
#include "bll/bll_event_itemget_msg.h"
#include "bll/bll_event_sysinfoget_msg.h"
#include "bll/bll_event_sysinfoset_msg.h"
#include "bll/bll_event_taskserver_msg.h"
#include "bll/bll_event_clientmsg.h"


FRAME_DBPROXY_NAMESPACE_BEGIN

bool CMainFrame::g_bMemcacheConnected = false;

CMainFrame::CMainFrame()
:CLightFrame(SERVER_NAME_STRING)
{
	m_nAppThreadCount = 0;
}

CMainFrame::~CMainFrame()
{
}

//框架初始话
int32_t CMainFrame::Initialize()
{
	int32_t ret=S_OK;

	//添加房间相关配置
	//AddConfigCenter(0, DEFAULT_KEY_CONFIGFILENAME, &g_KeyConfig);

	//添加客户端相关配置
	AddConfigCenter(0, DEFAULT_MEMCACHE_CONFIGFILENAME, &g_MemCacheConfig);

	AddConfigCenter(0, DEFAULT_DBCONFIG_FILENAME, &GET_DBCONFIG_INSTANCE());

	ret = CLightFrame::Initialize();
	if(0 > ret)
	{
		//WriteLog
		return ret;
	}
	//创建db引擎
	CreateDBEngine(enmMaxAppThreadCount);



	//初始化memcache连接检测线程
	ret = GET_MEMCACHE_CONN_CHECK_THREAD_INSTANCE().Initialize();
	if(0 > ret)
	{
		g_FrameLogEngine.WriteBaseLog( enmLogLevel_Error, "[%s:%d]initialize memcache connection check thread failed! errorcode=0x%08X\n",
			__FILE__, __LINE__, ret);
		return ret;
	}
	g_FrameLogEngine.WriteBaseLog( enmLogLevel_Notice, "[%s:%d]initialize memcache connection check thread success!\n",
		__FILE__, __LINE__);

	//注册所有的消息
	RegistMsg();
	WRITE_NOTICE_LOG("server init success!");
	return S_OK;
}

//恢复框架
int32_t CMainFrame::Resume()
{
	return CLightFrame::Resume();

}
//销毁框架
int32_t CMainFrame::Terminate()
{
	g_eServerCommand = enmServerCommand_Terminate;
	return S_OK;
}

void CMainFrame::Run()
{
	CLightFrame::Run();
}

void CMainFrame::Reload()
{
	g_eServerCommand = enmServerCommand_Reload;
}

void CMainFrame::RegistMsg()
{
	INITIALIZE_MESSAGEMAP_DECL(hallserver);
	INITIALIZE_MESSAGEMAP_DECL(message_from_roomserver);
	INITIALIZE_MESSAGEMAP_DECL(roomserver);
	INITIALIZE_MESSAGEMAP_DECL(itemserver);
	INITIALIZE_MESSAGEMAP_DECL(public_message);
	INITIALIZE_MESSAGEMAP_DECL(gameserver);
	INITIALIZE_MESSAGEMAP_DECL(client_message);
	INITIALIZE_MESSAGEMAP_DECL(tunnelserver);
	INITIALIZE_MESSAGEMAP_DECL(sysinfoserver);

	//大厅相关消息
	RegistMsgHandler(MSGID_HSDB_GETROLELOGININFO_REQ, &GET_FROM_HALLGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromHallGetEvent::OnMessage_HallGetRoleInfo));
	RegistMsgHandler(MSGID_HSDB_GETROOM_ADMIN_COUNT_REQ, &GET_FROM_HALLGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromHallGetEvent::OnMessage_HallGetRoomAdmcount));
	RegistMsgHandler(MSGID_HSDB_GETROOMINFO_REQ, &GET_FROM_HALLGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromHallGetEvent::OnMessage_HallGetRoomInfo));

	//RegistMsgHandler(MSGID_HSTA_ONLINE_NOTI, &GET_FROM_HALLSET_EVENT_INSTANCE(),static_cast<SS_MSG_PROC>(&CFromHallSetEvent::OnMessage_OnlineNotice));
	RegistMsgHandler(MSGID_HSDB_SETLOGININFO_NOTI, &GET_FROM_HALLSET_EVENT_INSTANCE(),static_cast<SS_MSG_PROC>(&CFromHallSetEvent::OnMessage_SetLoginInfo));
	RegistMsgHandler(MSGID_HSDB_SETLOGOUTINFO_NOTI, &GET_FROM_HALLSET_EVENT_INSTANCE(),static_cast<SS_MSG_PROC>(&CFromHallSetEvent::OnMessage_SetLogoutInfo));
	RegistMsgHandler(MSGID_HSDB_SET_TITLE_REQ, &GET_FROM_HALLSET_EVENT_INSTANCE(),static_cast<SS_MSG_PROC>(&CFromHallSetEvent::OnMessage_SetRoleRank));
	RegistMsgHandler(MSGID_HSDB_SETSELFINFO_REQ, &GET_FROM_HALLSET_EVENT_INSTANCE(),static_cast<SS_MSG_PROC>(&CFromHallSetEvent::OnMessage_SetSelfInfo));


	//房间相关
	RegistMsgHandler(MSGID_RSDB_GETROLEINFO_REQ, &GET_FROM_ROOMGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomGetEvent::OnMessageGetRoleInfo));
	RegistMsgHandler(MSGID_RSDB_GETROOMINFO_REQ, &GET_FROM_ROOMGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomGetEvent::OnMessageGetRoomInfo));
	RegistMsgHandler(MSGID_RSDB_GETSONGLISTFROMDB_REQ, &GET_FROM_ROOMGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomGetEvent::OnMessageGetSongList));
	RegistMsgHandler(MSGID_RSDB_GETROOMARTIST_REQ, &GET_FROM_ROOMGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomGetEvent::OnMessageGetRoomArtistList));
	RegistMsgHandler(MSGID_RSDB_GETROLEIDENTITY_REQ, &GET_FROM_ROOMGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomGetEvent::OnMessageGetArtistInfo));

	RegistMsgHandler(MSGID_RSDB_ADD_BLACK_REQ, &GET_FROM_ROOMADD_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomAddEvent::OnMessage_AddBlacklist));
	RegistMsgHandler(MSGID_RSDB_ADD_ONLINE_TIME_INROOM_NOTICE, &GET_FROM_ROOMADD_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomAddEvent::OnMessage_AddInRoomTime));
	RegistMsgHandler(MSGID_RSDB_ADD_ONMIC_TIME_INROOM_NOTICE, &GET_FROM_ROOMADD_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomAddEvent::OnMessage_AddOnMike));

	RegistMsgHandler(MSGID_RSMS_ENTER_ROOM_NOTICE, &GET_FROM_ROOMSET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomSetEvent::OnMessage_EnterRoomNotice));
	RegistMsgHandler(MSGID_RSDB_SET_ROOM_REQ, &GET_FROM_ROOMSET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomSetEvent::OnMessage_SetAttr));
	RegistMsgHandler(MSGID_RSDB_STA_PLAYER_COUNT_NOTIFY, &GET_FROM_ROOMSET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomSetEvent::OnMessage_SetPlayerCount));
	RegistMsgHandler(MSGID_RSDB_SET_TITLE_REQ, &GET_FROM_ROOMSET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomSetEvent::OnMessage_SetTitle));
	RegistMsgHandler(MSGID_RSDB_STA_PLAYER_MIC_STATUS, &GET_FROM_ROOMSET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomSetEvent::OnMessage_UpdownMike));
	RegistMsgHandler(MSGID_RSDB_UPDATESONGLIST_NOTI, &GET_FROM_ROOMSET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomSetEvent::OnMessage_SetSongList));
	RegistMsgHandler(MSGID_RSDB_UPDATESONGORDERINFO_NOTI, &GET_FROM_ROOMSET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromRoomSetEvent::OnMessage_SetSongOrder));


	//itemserver 相关
	RegistMsgHandler(MSGID_ISDB_BATCHUPDATEASSET_REQ, &GET_FROM_ITEMUPDATE_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemUpdateEvent::OnMessageBatchUpdateAsset));
	RegistMsgHandler(MSGID_ISOS_UPDATEIDENTITY_NOTI, &GET_FROM_ITEMUPDATE_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemUpdateEvent::OnMessageSetAchieve));
	RegistMsgHandler(MSGID_ISDB_SETITEMJACKPOTINFO_REQ, &GET_FROM_ITEMUPDATE_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemUpdateEvent::OnMessageSetJackpot));
	RegistMsgHandler(MSGID_ISDB_SETSOFA_NOTI, &GET_FROM_ITEMUPDATE_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemUpdateEvent::OnMessageSetSofa));
	RegistMsgHandler(MSGID_ISDB_SETSTOREPOOLINFO_REQ, &GET_FROM_ITEMUPDATE_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemUpdateEvent::OnMessageSetStorepool));
	RegistMsgHandler(MSGID_ISDB_UPDATEITEM_REQ, &GET_FROM_ITEMUPDATE_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemUpdateEvent::OnMessageSetUpdateItem));
	RegistMsgHandler(MSGID_ISDB_SETLUCKYUSER_NOTI, &GET_FROM_ITEMUPDATE_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemUpdateEvent::OnMessageSetLuckyuser));
	RegistMsgHandler(MSGID_ISDB_SETJACKPOTMONEY_NOTI, &GET_FROM_ITEMUPDATE_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemUpdateEvent::OnMessageSetPriceJackpot));


	RegistMsgHandler(MSGID_ISDB_GETROLEITEMINFO_REQ, &GET_FROM_ITEMGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemGetEvent::OnMessage_getiteminfo));
	RegistMsgHandler(MSGID_ISDB_GETSTOREPOOLINFO_REQ, &GET_FROM_ITEMGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemGetEvent::OnMessage_getstorepool));
	RegistMsgHandler(MSGID_ISDB_GETITEMJACKPOTINFO_REQ, &GET_FROM_ITEMGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemGetEvent::OnMessage_getjackpot));
	RegistMsgHandler(MSGID_ISDB_GETONLINEPLAYERMONEY_REQ, &GET_FROM_ITEMGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemGetEvent::OnMessage_getPlayerMoney));
	RegistMsgHandler(MSGID_ISDB_GETSOFA_REQ, &GET_FROM_ITEMGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemGetEvent::OnMessage_getSofa));
	RegistMsgHandler(MSGID_ISDB_GETSTARPLAYER_REQ, &GET_FROM_ITEMGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemGetEvent::OnMessage_getStarPlayer));
	RegistMsgHandler(MSGID_ISDB_GETLUCKYUSER_REQ, &GET_FROM_ITEMGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemGetEvent::OnMessage_getluckyuser));
	RegistMsgHandler(MSGID_ISDB_GETJACKPOTMONEY_REQ, &GET_FROM_ITEMGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromItemGetEvent::OnMessage_getpricejackpot));


	//client 相关
	RegistMsgHandler(MSGID_CLIENTDB_GETFANS_REQ, &GET_FROM_CLIENT_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromClientEvent::OnMessage_get_fans));
	RegistMsgHandler(MSGID_TSDB_ROOM_COLLECTION_INF0_NOTICE, &GET_FROM_CLIENT_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromClientEvent::OnMessage_netinfo));
	RegistMsgHandler(MESGIID_GAMESVR_CURRENCY_EXCHANGE_REQ, &GET_FROM_CLIENT_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromClientEvent::OnMessage_gold_exchange));
	RegistMsgHandler(MESGIID_GAMESVR_GET_EXCHANGE_LOG_REQ, &GET_FROM_CLIENT_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromClientEvent::OnMessage_get_exchangelog));
	RegistMsgHandler(MSGID_GAMESVR_GET_ROLE_BASEINFO_REQ, &GET_FROM_CLIENT_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromClientEvent::OnMessage_get_rolebaseinfo));
	RegistMsgHandler(MSGID_GAMESVR_UPDATE_ASSET_REQ, &GET_FROM_CLIENT_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromClientEvent::OnMessage_update_asset));
	RegistMsgHandler(MESGID_SVRGAME_GET_RECHARGE_MSG_REQ, &GET_FROM_CLIENT_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromClientEvent::OnMessage_GetRecharge));


	RegistMsgHandler(MSGID_OTDB_UPDATEASSET_REQ, &GET_FROM_PUBLICUPDATE_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromPublicUpdateEvent::OnMessagePublicAssetEvent));

	//sysinfo相关
	RegistMsgHandler(MSGID_SSDS_GETMSGCNT_REQ, &GET_FROM_SYSINFOGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromSysinfoGetEvent::OnMessage_getmsgcount));
	RegistMsgHandler(MSGID_SSDS_GETNOTICE_REQ, &GET_FROM_SYSINFOGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromSysinfoGetEvent::OnMessage_getnoti));
	RegistMsgHandler(MSGID_SSDS_GETNOTICEVER_REQ, &GET_FROM_SYSINFOGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromSysinfoGetEvent::OnMessage_getNotiVersion));
	RegistMsgHandler(MSGID_SSDS_RANK_REQ, &GET_FROM_SYSINFOGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromSysinfoGetEvent::OnMessage_getRankInfo));
	RegistMsgHandler(MSGID_SSDS_SYPPLYMENT_REQ, &GET_FROM_SYSINFOGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromSysinfoGetEvent::OnMessage_getRecharge));
	RegistMsgHandler(MSGID_SSDS_VIPINFO_REQ, &GET_FROM_SYSINFOGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromSysinfoGetEvent::OnMessage_getVipInfo));
	RegistMsgHandler(MSGID_SSDS_GETMSG_REQ, &GET_FROM_SYSINFOGET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromSysinfoGetEvent::OnMessage_getMsg));


	RegistMsgHandler(MSGID_SSDS_CLEARTIMEOUTVIP_REQ, &GET_FROM_SYSINFOSET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromSysinfoSetEvent::OnMessage_ClearVip));
	RegistMsgHandler(MSGID_SSDS_DEGRADE_REQ, &GET_FROM_SYSINFOSET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromSysinfoSetEvent::OnMessage_Degrade));
	RegistMsgHandler(MSGID_SSDS_GROUPDEGRADE_REQ, &GET_FROM_SYSINFOSET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromSysinfoSetEvent::OnMessage_GroupDegrade));
	RegistMsgHandler(MSGID_SSDS_NEWMSG_REQ, &GET_FROM_SYSINFOSET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromSysinfoSetEvent::OnMessage_NewMsg));
	RegistMsgHandler(MSGID_SSDS_WEEKSYPPLYMENT_REQ, &GET_FROM_SYSINFOSET_EVENT_INSTANCE(), static_cast<SS_MSG_PROC>(&CFromSysinfoSetEvent::OnMessage_WeekRecharge));
}

int32_t CMainFrame::CreateDBEngine(int32_t nCount)
{
	int32_t nRet = S_OK;

	for(int32_t i = 0; i < nCount; ++i)
	{
		m_arrDBEngine[i] = new CDBEngine();

		DBConfigInfo stDBConfig[enmMaxDBServerCount];
		int32_t nDBCount = 0;
		memset(&stDBConfig, 0, sizeof(DBConfigInfo));

		CDBConfig& dbconfig = GET_DBCONFIG_INSTANCE();
		nRet = dbconfig.GetDBConfigInfo(nDBCount, stDBConfig);

		for(int32_t j = 0; j<nDBCount; ++j)
		{
			nRet = m_arrDBEngine[i]->Initialize(stDBConfig[j].szDBIP, stDBConfig[j].szUser, stDBConfig[j].szPassword, stDBConfig[j].szDBName,stDBConfig[j].nDBPort, i);
			if (0 > nRet)
			{
				g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]initialize dbaccessthread failed!threadindex=%d, errorcode=0x%08X\n", __FILE__, __LINE__, i, nRet);
				return nRet;
			}
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]initialize dbaccess thread success! thread index=%d\n",__FILE__, __LINE__, i);
		}
	}
	return S_OK;
}

FRAME_DBPROXY_NAMESPACE_END

