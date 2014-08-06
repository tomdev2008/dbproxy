/********************************************************************
* filename:dbserver_config.cpp
* author:  xiaohq
* date:    2011-12-18 13:37
* version: x
* purpose: 加载数据库服务器的IP和端口
* revision:no
********************************************************************/


#include "dbserver_config.h"
#include "def/def_dbproxy_errorcode.h"
#include "tinyxml/tinyxml.h"
#include "frame_logengine.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

#define TAGNAME_MASTER                                 "master"
#define TAGNAME_SLAVE                                  "slave"
#define TAGNAME_DBSERVER					           "dbserver"
#define TAGNAME_RDDBSERVER                             "rddbserver"
#define ATTRIBUTE_DBSERVER_IP				           "dbserverip"
#define ATTRIBUTE_DBSERVER_PORT				           "dbserverport"
#define ATTRIBUTE_DBSERVER_USER                        "user"
#define ATTRIBUTE_DBSERVER_PASSWORD                    "password"
#define ATTRIBUTE_DBSERVER_DBNAME                      "dbname"


CDBConfig::CDBConfig()
{
	m_nLoadIndex = 0;
	m_nUseIndex = 1;
	m_nDBServerCount[m_nLoadIndex] = 0;
	m_nDBServerCount[m_nUseIndex] = 0;
	memset(&m_stDBConfigInfo[m_nLoadIndex], 0, sizeof(DBConfigInfo));
	memset(&m_stDBConfigInfo[m_nUseIndex], 0, sizeof(DBConfigInfo));
}

CDBConfig::~CDBConfig()
{

}

int32_t CDBConfig::Initialize(const char* szFileName /* = NULL */, const int32_t type/* =0 */)
{
	memset(&m_stDBConfigInfo[m_nLoadIndex], 0, sizeof(m_stDBConfigInfo[m_nLoadIndex]));
	m_nDBServerCount[m_nLoadIndex] = 0;
	memset(&m_arrRDDBConfigInfo[m_nLoadIndex], 0, sizeof(m_arrRDDBConfigInfo[m_nLoadIndex]));
	m_nRDDBServerCount[m_nLoadIndex] = 0;

	//加载配置文件
	TiXmlDocument doc(szFileName); 
	if (!doc.LoadFile(TIXML_ENCODING_UTF8))
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load dbconfig failed!\n", __FILE__, __LINE__);
		return E_LOAD_DBCONFIG_FILE;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load dbconfig success!\n",__FILE__, __LINE__);

	//获取根节点
	TiXmlElement *pRoot = doc.RootElement();
	if (NULL == pRoot)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load dbconfig root node failed!\n", __FILE__, __LINE__);
		return E_LOAD_DBCONFIG_ROOT;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load dbconfig root node success!\n", __FILE__, __LINE__);

	//获取master节点
	TiXmlElement* pMaster = pRoot->FirstChildElement(TAGNAME_MASTER);
	if(NULL == pMaster)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]tag master is null!\n", __FILE__, __LINE__);
		return E_TAG_MASTER_NULL;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]tag master is not null!\n", __FILE__, __LINE__);

	//获取dbserver节点
	TiXmlElement *pDBServer = pMaster->FirstChildElement(TAGNAME_DBSERVER);
	if (NULL == pDBServer)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load tag dbserver failed!\n", __FILE__, __LINE__);
		return E_LOAD_DBCONFIG_DBSERVER;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load tag dbserver success!\n", __FILE__, __LINE__);

	const char* pszValue = NULL;
	while(NULL != pDBServer && m_nDBServerCount[m_nLoadIndex] < enmMaxDBServerCount)
	{
		pszValue = pDBServer->Attribute(ATTRIBUTE_DBSERVER_IP);
		if(NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load master ip failed! dbindex=%d\n", __FILE__, __LINE__, m_nDBServerCount[m_nLoadIndex]);
			return E_LOAD_DBSERVER_IP;
		}
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load master ip success! dbindex=%d, serverip:%s\n", __FILE__, __LINE__, m_nDBServerCount[m_nLoadIndex], pszValue);
		strcpy(m_stDBConfigInfo[m_nLoadIndex][m_nDBServerCount[m_nLoadIndex]].szDBIP, pszValue);

		int32_t nTmpPort = 0;
		pszValue = pDBServer->Attribute(ATTRIBUTE_DBSERVER_PORT, &nTmpPort);
		if(NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load master port failed! dbindex=%d\n", __FILE__, __LINE__, m_nDBServerCount[m_nLoadIndex]);
			return E_LOAD_DBSERVER_PORT;
		}
		m_stDBConfigInfo[m_nLoadIndex][m_nDBServerCount[m_nLoadIndex]].nDBPort = (uint16_t)nTmpPort;
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load master port success! dbindex=%d, serverport:%d\n", __FILE__, __LINE__, m_nDBServerCount[m_nLoadIndex], nTmpPort);

		pszValue = pDBServer->Attribute(ATTRIBUTE_DBSERVER_USER);
		if(NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load master username failed! dbindex=%d\n", __FILE__, __LINE__, m_nDBServerCount[m_nLoadIndex]);
			return E_LOAD_DBSERVER_USER;
		}
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load master username success!dbindex=%d,  username=%s\n", __FILE__, __LINE__, m_nDBServerCount[m_nLoadIndex], pszValue);
		strcpy(m_stDBConfigInfo[m_nLoadIndex][m_nDBServerCount[m_nLoadIndex]].szUser, pszValue);

		pszValue = pDBServer->Attribute(ATTRIBUTE_DBSERVER_PASSWORD);
		if(NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load master dbpassword failed! dbindex=%d\n", __FILE__, __LINE__, m_nDBServerCount[m_nLoadIndex]);
			return E_LOAD_DBPASSWORD;
		}
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load master dbpassword success! dbindex=%d\n", __FILE__, __LINE__, m_nDBServerCount[m_nLoadIndex]);
		strcpy(m_stDBConfigInfo[m_nLoadIndex][m_nDBServerCount[m_nLoadIndex]].szPassword, pszValue);

		pszValue = pDBServer->Attribute(ATTRIBUTE_DBSERVER_DBNAME);
		if(NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load master dbname failed! dbindex=%d\n", __FILE__, __LINE__, m_nDBServerCount[m_nLoadIndex]);
			return E_LOAD_DBNAME;
		}
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load master dbname success! dbindex=%d, dbname=%s\n", __FILE__, __LINE__, m_nDBServerCount[m_nLoadIndex], pszValue);
		strcpy(m_stDBConfigInfo[m_nLoadIndex][m_nDBServerCount[m_nLoadIndex]].szDBName, pszValue);

		++m_nDBServerCount[m_nLoadIndex];
		pDBServer = pDBServer->NextSiblingElement();
	}

	//加载slave节点及其子节点
	TiXmlElement* pSlave = pRoot->FirstChildElement(TAGNAME_SLAVE);
	if(NULL == pSlave)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]tag slave is null!\n", __FILE__, __LINE__);
		return E_TAG_SLAVE_NULL;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]tag slave is not null!\n", __FILE__, __LINE__);

	TiXmlElement* pRDDBServer = pSlave->FirstChildElement(TAGNAME_RDDBSERVER);
	if(NULL == pRDDBServer)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]read dbserver tag is null!\n", __FILE__, __LINE__);
		return E_TAG_RDDBSERVER_NULL;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]read dbserver tag is not null!\n", __FILE__, __LINE__);

	pszValue = NULL;
	while(NULL != pRDDBServer && m_nRDDBServerCount[m_nLoadIndex] < enmMaxDBServerCount)
	{
		pszValue = pRDDBServer->Attribute(ATTRIBUTE_DBSERVER_IP);
		if(NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load slave ip failed! dbindex=%d\n", __FILE__, __LINE__, m_nRDDBServerCount[m_nLoadIndex]);
			return E_LOAD_DBSERVER_IP;
		}
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load slave ip success! dbindex=%d, serverip:%s\n", __FILE__, __LINE__, m_nRDDBServerCount[m_nLoadIndex], pszValue);
		strcpy(m_arrRDDBConfigInfo[m_nLoadIndex][m_nRDDBServerCount[m_nLoadIndex]].szDBIP, pszValue);

		int32_t nTmpPort = 0;
		pszValue = pRDDBServer->Attribute(ATTRIBUTE_DBSERVER_PORT, &nTmpPort);
		if(NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load slave port failed! dbindex=%d\n", __FILE__, __LINE__, m_nRDDBServerCount[m_nLoadIndex]);
			return E_LOAD_DBSERVER_PORT;
		}
		m_arrRDDBConfigInfo[m_nLoadIndex][m_nRDDBServerCount[m_nLoadIndex]].nDBPort = (uint16_t)nTmpPort;
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load slave port success! dbindex=%d, serverport:%d\n", __FILE__, __LINE__, m_nRDDBServerCount[m_nLoadIndex], nTmpPort);

		pszValue = pRDDBServer->Attribute(ATTRIBUTE_DBSERVER_USER);
		if(NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load slave username failed! dbindex=%d\n", __FILE__, __LINE__, m_nRDDBServerCount[m_nLoadIndex]);
			return E_LOAD_DBSERVER_USER;
		}
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load slave username success!dbindex=%d,  username=%s\n", __FILE__, __LINE__, m_nRDDBServerCount[m_nLoadIndex], pszValue);
		strcpy(m_arrRDDBConfigInfo[m_nLoadIndex][m_nRDDBServerCount[m_nLoadIndex]].szUser, pszValue);

		pszValue = pRDDBServer->Attribute(ATTRIBUTE_DBSERVER_PASSWORD);
		if(NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load slave dbpassword failed! dbindex=%d\n", __FILE__, __LINE__, m_nRDDBServerCount[m_nLoadIndex]);
			return E_LOAD_DBPASSWORD;
		}
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load slave dbpassword success! dbindex=%d\n", __FILE__, __LINE__, m_nRDDBServerCount[m_nLoadIndex]);
		strcpy(m_arrRDDBConfigInfo[m_nLoadIndex][m_nRDDBServerCount[m_nLoadIndex]].szPassword, pszValue);

		pszValue = pRDDBServer->Attribute(ATTRIBUTE_DBSERVER_DBNAME);
		if(NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]load slave dbname failed! dbindex=%d\n", __FILE__, __LINE__, m_nRDDBServerCount[m_nLoadIndex]);
			return E_LOAD_DBNAME;
		}
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]load slave dbname success! dbindex=%d, dbname=%s\n", __FILE__, __LINE__, m_nRDDBServerCount[m_nLoadIndex], pszValue);
		strcpy(m_arrRDDBConfigInfo[m_nLoadIndex][m_nRDDBServerCount[m_nLoadIndex]].szDBName, pszValue);

		++m_nRDDBServerCount[m_nLoadIndex];
		pRDDBServer = pRDDBServer->NextSiblingElement();
	}

	int32_t nIndex = m_nUseIndex;
	m_nUseIndex = m_nLoadIndex;
	m_nLoadIndex = nIndex;

	return S_OK;
}

int32_t CDBConfig::Uninitialize()
{
	return S_OK;
}

int32_t CDBConfig::Reload(const char* szFileName /* = NULL */, const int32_t type/* =0 */)
{
	return Initialize(szFileName, type);
}

int32_t CDBConfig::GetDBConfigInfo(int32_t& nDBServerCount, DBConfigInfo arrDBConfigInfo[])
{
	int32_t tmpUseIndex = m_nUseIndex;
	nDBServerCount = m_nDBServerCount[tmpUseIndex];
	if(nDBServerCount > enmMaxDBServerCount)
	{
		return E_MORE_THAN_MAX_DBSERVER_COUNT;
	}

	memset(arrDBConfigInfo, 0, sizeof(DBConfigInfo)*nDBServerCount);

	for(int32_t i=0; i<nDBServerCount; ++i)
	{
		arrDBConfigInfo[i].nDBPort = m_stDBConfigInfo[tmpUseIndex][i].nDBPort;
		strcpy(arrDBConfigInfo[i].szDBIP, m_stDBConfigInfo[tmpUseIndex][i].szDBIP);
		strcpy(arrDBConfigInfo[i].szUser, m_stDBConfigInfo[tmpUseIndex][i].szUser);
		strcpy(arrDBConfigInfo[i].szPassword, m_stDBConfigInfo[tmpUseIndex][i].szPassword);
		strcpy(arrDBConfigInfo[i].szDBName, m_stDBConfigInfo[tmpUseIndex][i].szDBName);
	}

	return S_OK;
}

int32_t CDBConfig::GetRDDBConfig(int32_t& nRDDBServerCount, DBConfigInfo arrRDDBConfigInfo[])
{
	int32_t tmpUseIndex = m_nUseIndex;
	nRDDBServerCount = m_nRDDBServerCount[tmpUseIndex];
	if(0 == nRDDBServerCount)
	{
		return E_NO_SLAVE_DB;
	}
	if(nRDDBServerCount > enmMaxDBServerCount)
	{
		return E_MORE_THAN_MAX_DBSERVER_COUNT;
	}
	memset(arrRDDBConfigInfo, 0, sizeof(DBConfigInfo) * nRDDBServerCount);
	for(int32_t i=0; i<nRDDBServerCount; ++i)
	{
		arrRDDBConfigInfo[i].nDBPort = m_arrRDDBConfigInfo[tmpUseIndex][i].nDBPort;
		strcpy(arrRDDBConfigInfo[i].szDBIP, m_arrRDDBConfigInfo[tmpUseIndex][i].szDBIP);
		strcpy(arrRDDBConfigInfo[i].szUser, m_arrRDDBConfigInfo[tmpUseIndex][i].szUser);
		strcpy(arrRDDBConfigInfo[i].szPassword, m_arrRDDBConfigInfo[tmpUseIndex][i].szPassword);
		strcpy(arrRDDBConfigInfo[i].szDBName, m_arrRDDBConfigInfo[tmpUseIndex][i].szDBName);
	}
	return S_OK;
}

FRAME_DBPROXY_NAMESPACE_END
