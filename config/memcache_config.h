/********************************************************************
* filename: memcache_config.h
* date:     2012-01-01 16:33
* author:   xiaohq
* purpose:  memcacheµÄIPºÍ¶Ë¿ÚÅäÖÃ
* version:  1.0
* revision: not yet
********************************************************************/

#ifndef _MEMCACHE_CONFIG_H
#define _MEMCACHE_CONFIG_H

#include "common/common_singleton.h"
#include "common/common_def.h"
#include "common/common_typedef.h"
#include "../def/def_dbproxy.h"
#include "../def/server_namespace.h"
#include "lightframe_impl.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

#define DEFAULT_MEMCACHE_CONFIGFILENAME   "./config/memcacheconfig.xml"

class CMemcachConfig : public IConfigCenter
{
public:
	CMemcachConfig();
	virtual ~CMemcachConfig();

public:
	int32_t Initialize(const char* szFileName = NULL, const int32_t type=0);
	int32_t Reload(const char* szFileName = NULL, const int32_t type=0);
	int32_t Uninitialize();

public:
	char* GetMemcacheIPAndPort();

protected:
	char m_szMemcacheIP[2][enmMaxIPAddressLength];
	int32_t m_nMemcachePort[2];
	char m_szMemcahceIPAndPort[2][enmMaxIPAndPortLength];

protected:
	int32_t m_nLoadIndex;
	int32_t m_nUseIndex;
};

#define CREATE_MEMCACHECONFIG_INSTANCE    CSingleton<CMemcachConfig>::CreateInstance
#define GET_MEMCACHECONFIG_INSTANCE       CSingleton<CMemcachConfig>::GetInstance
#define DESTROY_MEMCACHECONFIG_INSTANCE   CSingleton<CMemcachConfig>::DestroyInstance

#define g_MemCacheConfig					CSingleton<CMemcachConfig>::GetInstance()

FRAME_DBPROXY_NAMESPACE_END

#endif

