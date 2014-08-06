/******************************************************************
* filename: vdc_memcache.h
* create:   2011-12-27 11:34
* author:   xiaohq
* purpose:  �������memcache�Ľӿڣ�����set�� get��delete�����ӿ�
			�������棬�洢�����Ϣ���Լ�������Ϣ��
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

	//��ʼ��m_memc
	int32_t init_m_memc();

	//��ʼ��m_servers
	int32_t init_m_servers();

public:
	//��ӻ��߸���memcache�� ����˳Ѱ����һ�����Ϣ����ֵ��Ϣ��������Ϣ������Ϣ
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

