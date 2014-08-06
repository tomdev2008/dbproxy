/*
 * bll_event_taskserver_msg.h
 *
 *  Created on: 2013-3-8
 *      Author: liufl
 */

#ifndef BLL_EVENT_TASKSERVER_MSG_H_
#define BLL_EVENT_TASKSERVER_MSG_H_

#include "../def/def_dbproxy.h"
#include "common/common_singleton.h"
#include "frame_session.h"
#include "frame_protocolhead.h"
#include "lightframe_impl.h"
#include "bll_base.h"
#include "../dal/common_query.h"

#include "../util.h"
#include "../dal/dal_player.h"
#include "../dal/billengine.h"
#include "../def/def_dbproxy_errorcode.h"
#include "taskserver_message_define.h"


FRAME_DBPROXY_NAMESPACE_BEGIN

class CFromTaskserverEvent: public CBllBase
{
public:
	CFromTaskserverEvent(){}
	virtual ~CFromTaskserverEvent(){}

public:
	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL)
	{
		return S_OK;
	}
public:
	int32_t OnMessage_GetTaskInfo(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t ProcessTaskGetTaskInfoReq(const CGetTaskInfoReq& reqbody, CGetTaskInfoResp& respbody);
	int32_t QueryRoleCreateTime(const RoleID roleid, uint32_t& createtime);
	int32_t QueryInRoomTime(const RoleID roleid, uint32_t& inroomtime);
	int32_t GetEmailBindStatus(const RoleID roleid, EmailStat& emailstatus);
	int32_t QueryTaskInfo(const RoleID roleid, int32_t& taskcount, TaskID taskid[], TaskStatType taskstatustype[], uint32_t updatetime[]);
	int32_t QueryModifyName(const RoleID roleid, NickNameStat& nicknamestatus);
	int32_t QueryIfRecharge(const RoleID roleid, RechargeStat& rechargestatus);

	int32_t OnMessage_UpdateAsset(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UpdateRoleAsset(const RoleID roleid, const UpdateAssetOperate opmod, const uint32_t nMoney, const char* szTransID, uint32_t& nLeftMoney);

	int32_t OnMessage_UpdateTaskStat(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UpdateTaskStat(const RoleID roleid, const TaskID taskid, const TaskStatType stattype);


};

#define	CREATE_FROM_TASKSERVER_EVENT_INSTANCE	        CSingleton<CFromTaskserverEvent>::CreateInstance
#define	GET_FROM_TASKSERVER_EVENT_INSTANCE		        CSingleton<CFromTaskserverEvent>::GetInstance
#define	DESTROY_FROM_TASKSERVER_EVENT_INSTANCE	        CSingleton<CFromTaskserverEvent>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END
#endif /* BLL_EVENT_TASKSERVER_MSG_H_ */
