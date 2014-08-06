/***************************************************************************************************
*	file name:	memcache_conncheck_thread.h
*	created:	2012-05-08 9:58
*	author:		xiaohq
*	version:	1.0
*	purpose:	memcache连接检测线程，定时检测（比如每隔3s检测一次）与memcache的连接是否断开，如果
*               断开，则重新建立连接
*
*	revision:   not yet
*
***************************************************************************************************/

#ifndef _DBPROXY_MEMCACHE_CONN_CHECK_THREAD_H_
#define _DBPROXY_MEMCACHE_CONN_CHECK_THREAD_H_

#include "common/common_singleton.h"
#include "common/common_thread.h"
#include "common/common_def.h"
#include "common/common_typedef.h"
#include "common/common_errordef.h"
#include "frame_typedef.h"

#include "frame_protocolhead.h"
#include "vdc_memcache.h"
#include <stdlib.h>




FRAME_DBPROXY_NAMESPACE_BEGIN

class CMemcacheConnCheckThread : public CThread
{
public:
	CMemcacheConnCheckThread();
	virtual ~CMemcacheConnCheckThread();

public:
	int32_t Initialize();
	int32_t Resume();
	int32_t Uninitialize();

public:
	//线程入口
	virtual void Execute();

protected:
	void CheckMemcacheConnection();

};

#define CREATE_MEMCACHE_CONN_CHECK_THREAD_INSTANCE      CSingleton<CMemcacheConnCheckThread>::CreateInstance
#define GET_MEMCACHE_CONN_CHECK_THREAD_INSTANCE         CSingleton<CMemcacheConnCheckThread>::GetInstance
#define DESTROY_MEMCACHE_CONN_CHECK_THREAD_INSTANCE     CSingleton<CMemcacheConnCheckThread>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END

#endif


