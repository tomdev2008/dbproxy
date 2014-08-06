/******************************************************************
* filename: vdc_memcache.h
* create:   2011-12-27 11:34
* author:   xiaohq
* purpose:  定义访问memcache的接口，包括set， get，delete三个接口
			用做缓存，存储玩家信息，以及房间信息等
* version:  1.0
* revision: not yet
******************************************************************/

#ifndef _DBPROXY_VDC_MEMCACHE_H_
#define _DBPROXY_VDC_MEMCACHE_H_

#include <sys/time.h>
#include <libmemcached/memcached.h>
#include <stdlib.h>
#include <common/common_singleton.h>
#include "common/common_typedef.h"
#include "../def/def_dbproxy.h"
#include "../def/def_dbproxy.h"
#include "../main_frame.h"            //write log
#include "common/common_errordef.h"   //define S_OK and so on
#include "frame_logengine.h"
#include "../dbproxy_thread_mutex.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

class CVDCMemcache
{
public:
	CVDCMemcache();
	virtual ~CVDCMemcache();

public:
	int32_t Initialize();

	//初始化m_memc
	int32_t init_m_memc();

	//初始化m_servers
	int32_t init_m_servers();

public:
	//添加或者更新memcache， 编码顺寻：玩家基本信息，充值信息，消费信息，获奖信息
	int32_t  MemcacheSet(memcached_st* memc, const char* szKey, const size_t keyLen, const char* szValue, const size_t valueLen, const time_t expiration);
	int32_t  MemcacheGet(memcached_st* memc, const char* szKey, const size_t keyLen, char*& szValue, size_t& valueLen);
	int32_t  MemcacheDel(memcached_st* memc, const char* szKey, const size_t keyLen, time_t expiration);
	int32_t  MemcacheIfKeyExist(memcached_st* memc, const char* szKey, const size_t keyLen);
	int32_t  MemcacheFlush(memcached_st* memc, time_t expiration);

public:
	memcached_st*        m_memc;
	uint32_t             m_unFlags;
//	memcached_return_t   m_ret;
	memcached_server_st* m_servers;
	CDBCriticalSection   m_dbcrit;
};

#define CREATE_VDCMEMCACHE_INSTANCE      CSingleton<CVDCMemcache>::CreateInstance
#define GET_VDCMEMCACHE_INSTANCE         CSingleton<CVDCMemcache>::GetInstance
#define DESTROY_VDCMEMCACHE_INSTANCE     CSingleton<CVDCMemcache>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END

#endif

