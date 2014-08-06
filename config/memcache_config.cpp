/*******************************************************************
* filename: memcache_config.cpp
* date:     2012-01-01 16:42
* author:   xiaohq
* purpose:  memcache的IP和端口配置
* version:  1.0
* revision: not yet
*******************************************************************/

#include "common/common_errordef.h"    //define S_OK
#include "tinyxml/tinyxml.h"
#include "memcache_config.h"
#include "def/def_dbproxy.h"
#include "def/def_dbproxy_errorcode.h"
#include "frame_logengine.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

#define TAGNAME_MEMCACHE  "memcache"
#define ATTRIBUTE_MEMCACHE_IP    "memcacheip"
#define ATTRIBUTE_MEMCACHE_PORT  "memcacheport"

CMemcachConfig::CMemcachConfig()
{
	m_nLoadIndex = 0;
	m_nUseIndex = 1;
	memset(m_szMemcacheIP, 0, sizeof(m_szMemcacheIP));
	memset(m_szMemcahceIPAndPort, 0, sizeof(m_szMemcahceIPAndPort));
	memset(m_nMemcachePort, 0, sizeof(m_nMemcachePort));
}

CMemcachConfig::~CMemcachConfig()
{
}

int32_t CMemcachConfig::Initialize(const char* szFileName/* = NULL*/, const int32_t type/*=0*/)
{
	m_szMemcacheIP[m_nLoadIndex][0] = '\0';
	m_nMemcachePort[m_nLoadIndex] = enmInvalidPort;
	m_szMemcahceIPAndPort[m_nLoadIndex][0] = '\0';

	//加载配置文件
	TiXmlDocument doc(szFileName);
	if (!doc.LoadFile(TIXML_ENCODING_UTF8))
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load memcacheconfig failed!\n", __FILE__, __LINE__);
		return E_LOAD_MEMCACHE_FILE;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load memcacheconfig success!\n",__FILE__, __LINE__);

	//获取根节点
	TiXmlElement *pRoot = doc.RootElement();
	if (NULL == pRoot)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load memcacheconfig root node failed!\n", __FILE__, __LINE__);
		return E_LOAD_MEMCACHE_ROOT;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load memcacheconfig root node success!\n", __FILE__, __LINE__);

	//获取memcache节点
	TiXmlElement *pMemcache = pRoot->FirstChildElement(TAGNAME_MEMCACHE);
	if (NULL == pMemcache)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load node memcache failed!\n", __FILE__, __LINE__);
		return E_LOAD_MEMCACHECONFIG_MEMCACHE;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load node key success!\n", __FILE__, __LINE__);

	const char* pszValue = NULL;
	pszValue = pMemcache->Attribute(ATTRIBUTE_MEMCACHE_IP);
	if(NULL == pszValue)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load memcache ip failed!\n", __FILE__, __LINE__);
		return E_LOAD_MEMCACHE_IP;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load memcache ip success!\n", __FILE__, __LINE__);
	strcpy(m_szMemcacheIP[m_nLoadIndex], pszValue);

	pszValue = pMemcache->Attribute(ATTRIBUTE_MEMCACHE_PORT, &m_nMemcachePort[m_nLoadIndex]);
	if(NULL == pszValue)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load attribute port failed!\n",__FILE__, __LINE__);
		return E_LOAD_MEMCACHE_PORT;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load attribute port success!\n", __FILE__, __LINE__);

	//产生形如192.168.69.49:11556类似格式的字符串
	sprintf(m_szMemcahceIPAndPort[m_nLoadIndex], "%s:%d", m_szMemcacheIP[m_nLoadIndex], m_nMemcachePort[m_nLoadIndex]);

	int32_t nIndex = m_nUseIndex;
	m_nUseIndex = m_nLoadIndex;
	m_nLoadIndex = nIndex;

	return S_OK;
}

int32_t CMemcachConfig::Uninitialize()
{
	return S_OK;
}

int32_t CMemcachConfig::Reload(const char* szFileName/* = NULL*/, const int32_t type/*=0*/)
{
	return Initialize(szFileName, type);
}

char* CMemcachConfig::GetMemcacheIPAndPort()
{
	return m_szMemcahceIPAndPort[m_nUseIndex];
}

FRAME_DBPROXY_NAMESPACE_END

