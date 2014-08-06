/*
 * common_query.h
 *
 *  Created on: 2013-2-28
 *      Author: liufl
 */

#ifndef COMMON_QUERY_H_
#define COMMON_QUERY_H_
#include "../def/def_dbproxy.h"
#include "common/common_singleton.h"
#include "frame_session.h"
#include "frame_protocolhead.h"
#include "lightframe_impl.h"
#include "../main_frame.h"
#include "../bll/bll_base.h"
#include "../def/def_dbproxy_errorcode.h"

#include "../util.h"
#include "../dal/dal_player.h"


FRAME_DBPROXY_NAMESPACE_BEGIN

int32_t QueryRoleBaseInfo(const RoleID roleid, RoleBaseInfo& rolebaseinfo);
int32_t QueryRole179ID(const RoleID roleid, uint32_t& n179id);
int32_t QueryRoomBaseInfo(const RoomID roomid, RoomBaseInfo& roombaseinfo);
int32_t QueryRoomHostRoleID(const RoomID roomid, RoleID& roleid);

FRAME_DBPROXY_NAMESPACE_END
#endif /* COMMON_QUERY_H_ */
