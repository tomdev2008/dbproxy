/************************************************************************
* filename: vdc_memcache.cpp
* create:   2011-12-27 11:45
* author:   xiaohq
* purpose:  memcache方法的实现
* version:  1.0
* revision: not yet
************************************************************************/

#include "vdc_memcache.h"
#include "../def/def_dbproxy_errorcode.h"
#include "../config/memcache_config.h"
#include "../util.h"
#include "frame_logengine.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

CVDCMemcache::CVDCMemcache()
{
	m_memc = memcached_create(NULL);
	m_servers = memcached_servers_parse(GET_MEMCACHECONFIG_INSTANCE().GetMemcacheIPAndPort());
	memcached_server_push(m_memc, m_servers);

	//设置为非阻塞
	memcached_behavior_set(m_memc, MEMCACHED_BEHAVIOR_NO_BLOCK, 1);
}

CVDCMemcache::~CVDCMemcache()
{
	if(m_memc != NULL)
	{
		memcached_free(m_memc);
		m_memc = NULL;
	}

	if(NULL != m_servers)
	{
		memcached_server_list_free(m_servers);
		m_servers = NULL;
	}
}

int32_t CVDCMemcache::init_m_memc()
{
	//如果m_memc对象不为空则先清空
	if(NULL != m_memc)
	{
		memcached_free(m_memc);
		m_memc = NULL;
	}

	//重新创建m_memc对象
	m_memc = memcached_create(NULL);

	//创建失败则返回错误
	if(NULL == m_memc)
	{
		g_FrameLogEngine.WriteBaseLog( enmLogLevel_Error, "[%s:%d]Error: initialize m_memc failed! errorcode=0x%08X\n",
			__FILE__, __LINE__, E_DBPROXY_MALLOC_MEMC);
//		g_bMemcacheConnected = false;
		return E_DBPROXY_MALLOC_MEMC;
	}
	g_FrameLogEngine.WriteBaseLog( enmLogLevel_Notice, "[%s:%d]initialize m_memc success!\n", __FILE__, __LINE__);

	//设置为非阻塞
	memcached_behavior_set(m_memc, MEMCACHED_BEHAVIOR_NO_BLOCK, 1);

	return S_OK;
}

int32_t CVDCMemcache::init_m_servers()
{
	//重置 m_servers， 如果不为空，则先释放，置空，保证m_servers为空
	if(NULL != m_servers)
	{
		memcached_server_list_free(m_servers);
		m_servers = NULL;
	}

	//重新创建m_servers对象
	m_servers = memcached_servers_parse(GET_MEMCACHECONFIG_INSTANCE().GetMemcacheIPAndPort());
	memcached_server_push(m_memc, m_servers);

	//创建m_servers对象失败，返回错误
	if(NULL == m_servers)
	{
		g_FrameLogEngine.WriteBaseLog( enmLogLevel_Error, "[%s:%d]ERROR: allocate memcached server failed! errorcode=0x%08X\n",
			__FILE__, __LINE__, E_DBPROXY_MALLOC_MEMCACHE_SERVERS);
//		g_bMemcacheConnected = false;
		return E_DBPROXY_MALLOC_MEMCACHE_SERVERS;
	}
	g_FrameLogEngine.WriteBaseLog( enmLogLevel_Notice, "[%s:%d]initialize m_servers success!\n", __FILE__, __LINE__);

	return S_OK;
}

int32_t CVDCMemcache::Initialize()
{
	DB_MUTEX_GUARD(cf_mutex, m_dbcrit);
	//首先初始化m_memc对象
	int32_t ret = S_OK;
	ret = init_m_memc();
	if(0 > ret)
	{
		g_FrameLogEngine.WriteBaseLog( enmLogLevel_Error, "[%s:%d]Error: initialize m_memc(memcached_st) failed! errorcode=0x%08X\n",
			__FILE__, __LINE__, ret);
		return ret;
	}
	g_FrameLogEngine.WriteBaseLog( enmLogLevel_Notice, "[%s:%d]initialize m_memc success!\n", __FILE__, __LINE__);

	//初始化m_servers对象
	ret = init_m_servers();
	if(0 > ret)
	{
		g_FrameLogEngine.WriteBaseLog( enmLogLevel_Error, "[%s:%d]initialize m_servers failed! errorcode=0x%08X\n",
			__FILE__, __LINE__, ret);
		return ret;
	}
	g_FrameLogEngine.WriteBaseLog( enmLogLevel_Notice, "[%s:%d]initialize m_servers success!\n", __FILE__, __LINE__);

	//更改连接状态为已连接上
	g_Frame.g_bMemcacheConnected = true;

	return S_OK;
}

//原型：memcached_return memcached_set(memcached_st* ptr, const char* ptr, size_t key_length, const char* value, size_t value_length, time_t expiration, uint32_t flags);
int32_t CVDCMemcache::MemcacheSet(memcached_st* memc, const char* szKey, const size_t keyLen, const char* szValue, const size_t valueLen, const time_t expiration)
{
	DB_MUTEX_GUARD(cf_mutex, m_dbcrit);
	memcached_return rc;
	rc = memcached_set(memc, szKey, keyLen, szValue, valueLen, expiration, (uint32_t)0);
	if(rc != MEMCACHED_SUCCESS)
	{
		//16号错误码：NOT FOUND   46号错误码：operation in process
		if(16 == rc || 46==rc)
		{
			WRITE_WARNING_LOG( "warning: operation is in process! szKey=%s, keylen=%d, expiration=%d, errorcode=%d, errordesc=%s\n",
				szKey,
				(int32_t)keyLen,
				(int32_t)expiration,
				rc,
				memcached_strerror(m_memc,rc));
			return S_OK;
		}
		WRITE_ERROR_LOG( "Error: set memcache failed! szKey=%s, keyLen=%d, szValue=%s, valuelen=%d, expirtation=%d, errorcode=%d, errordesc=%s\n",
			szKey,
			(int32_t)keyLen,
			szValue,
			(int32_t)valueLen,
			(int32_t)expiration,
			rc,
			memcached_strerror(memc, rc));

		return E_MEMCACHED_SET;
	}

	//WRITE_DEBUG_LOG( "set memcached success! szKey=%s, keylen=%d\n", szKey, (int32_t)keyLen);

	return S_OK;
}

//原型： char* memcached_get(memcached_st* ptr, const char* key, size_t keylen, size_t* valuelen, uint32_t* flags, memcached_return* error);
int32_t CVDCMemcache::MemcacheGet(memcached_st* memc, const char* szKey, const size_t keyLen, char*& szValue, size_t& valueLen)
{
	DB_MUTEX_GUARD(cf_mutex, m_dbcrit);
	memcached_return rc;
	szValue = memcached_get(memc, szKey, keyLen, &valueLen, 0, &rc);
	if(rc != MEMCACHED_SUCCESS)
	{
		WRITE_ERROR_LOG( "Error: get memcache object failed! szKey=%s, keylen=%d, szValue=%s, valuelen=%d, errorcode=%d, errordesc=%s\n", szKey, (int32_t)keyLen, szValue, (int32_t)valueLen, rc, memcached_strerror(memc, rc));
		return E_MEMCACHED_GET;
	}

	//WRITE_DEBUG_LOG( "get memcache object success! szKey=%s, keylen=%d\n", szKey, (int32_t)keyLen);

	return S_OK;
}

//原型：memcached_return memcached_delete(memcached_st* ptr, const char* key, size_t keylen, time_t expiration);
int32_t CVDCMemcache::MemcacheDel(memcached_st* memc, const char* szKey, const size_t keyLen, time_t expiration)
{
	DB_MUTEX_GUARD(cf_mutex, m_dbcrit);
	int32_t ret = S_OK;
	memcached_return rc;
	ret = MemcacheIfKeyExist(memc, szKey, keyLen);
	if(0 > ret)
	{
		WRITE_WARNING_LOG( "key not exist in memcache! so cannot delete it! just ignore! key=%s, keylen=%d\n",
			szKey,
			keyLen);
		return S_OK;
	}
	//WRITE_DEBUG_LOG( "key exist in cache! need to delete! key=%s, keylen=%d\n",szKey,keyLen);

	rc = memcached_delete(memc, szKey, keyLen, expiration);
	if(rc != MEMCACHED_SUCCESS)
	{
		//errno 15: STORED
		if(15 == rc)
		{
			WRITE_WARNING_LOG( "warning: object stored! szKey=%s, keylen=%d, errorcode=%d, errordesc=%s\n",
				szKey,
				(int32_t)keyLen,
				rc,
				memcached_strerror(memc, rc));
			return S_OK;
		}
		WRITE_ERROR_LOG( "Error: delete memcache object failed! szKey=%s, keyLen=%d, time expiration=%d, errocode=%d, errordesc=%s\n",
			szKey,
			(int32_t)keyLen,
			(int32_t)expiration,
			rc,
			memcached_strerror(memc, rc));
		return E_MEMCACHED_DELETE;
	}
	//WRITE_DEBUG_LOG( "delete memcache object success! szKey=%s, keylen=%d\n", szKey, (int32_t)keyLen);

	return S_OK;
}

int32_t CVDCMemcache::MemcacheIfKeyExist(memcached_st* memc, const char* szKey, const size_t keyLen)
{
	DB_MUTEX_GUARD(cf_mutex, m_dbcrit);
	memcached_return rc;
	rc = memcached_exist(memc, szKey, keyLen);
	if(rc != MEMCACHED_SUCCESS)
	{
		WRITE_WARNING_LOG( "warning: key is not exist in memcache! errorcode=%d\n", rc);
		return E_MEMCACHE_KEY_NOTFOUND;
	}
	//WRITE_DEBUG_LOG( "the key is exist in memcache! key=%s, keylen=%d\n", szKey, keyLen);

	return S_OK;
}

int32_t CVDCMemcache::MemcacheFlush(memcached_st* memc, time_t expiration)
{
	DB_MUTEX_GUARD(cf_mutex, m_dbcrit);
	memcached_return rc;
	rc = memcached_flush(memc, expiration);
	if(rc != MEMCACHED_SUCCESS)
	{
		WRITE_WARNING_LOG( "wipe clean the contents of memcached servers failed! errorcode=%d, errordesc=%s\n", rc, memcached_strerror(memc, rc));
		return E_MEMCACHE_FLUSH;
	}
	WRITE_DEBUG_LOG( "wipe clean the contents of memcached servers!\n");

	return S_OK;
}

FRAME_DBPROXY_NAMESPACE_END

