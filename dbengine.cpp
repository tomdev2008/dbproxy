/***********************************************************************
* filename: dbengine_merge.cpp
* date:     2011-12-26 19:56
* author:   xiaohq
* purpose:  数据库引擎实现
***********************************************************************/

#include "common/common_datetime.h"
#include "common/common_errordef.h"
#include "common/common_def.h"
#include "common/common_typedef.h"
#include "dbengine.h"
#include "def/def_dbproxy.h"
#include "def/def_dbproxy_errorcode.h"
#include "main_frame.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

CDBEngine::CDBEngine()
{
	m_mysqlConnectionStatus = enmMysqlConnectionStatus_Closed;
	m_nReadDBServerCount = 0;
	memset(m_arrDBConfigInfo, 0, sizeof(m_arrDBConfigInfo));
}

CDBEngine::~CDBEngine()
{

}

//初始化数据库引擎对象
int32_t CDBEngine::Initialize(const char* szHost, const char* szUser, const char* szPassword, const char* szDBName,int32_t port, int32_t threadIndex)
{
	m_nReadDBServerCount = 0;
	memset(m_arrDBConfigInfo, 0, sizeof(m_arrDBConfigInfo));

	//初始化
	int32_t ret = m_mysqlEngine.Initialize(szHost, szUser, szPassword, szDBName,port);
	if (0 > ret)
	{
		return ret;
	}


	//初始化 m_mysqlReadEngine
	ret = GET_DBCONFIG_INSTANCE().GetRDDBConfig(m_nReadDBServerCount, m_arrDBConfigInfo);
	if(0 == m_nReadDBServerCount)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d] no slave database!\n", __FILE__, __LINE__);
		return E_NO_SLAVE_DB;
	}
	ret = m_mysqlReadEngine.Initialize(m_arrDBConfigInfo[0].szDBIP, m_arrDBConfigInfo[0].szUser, m_arrDBConfigInfo[0].szPassword, m_arrDBConfigInfo[0].szDBName, m_arrDBConfigInfo[0].nDBPort);
	if(0 > ret)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]initialize read dbengine failed! errorcode=0x%08X\n",
			__FILE__, __LINE__, ret);
		return ret;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]initialize read dbengine success!\n", __FILE__, __LINE__);
	m_mysqlReadEngine.InitializeLogPrefix("./log/dbproxyserver_mysql");
	m_mysqlReadEngine.SetLogLevel(g_FrameConfigMgt.GetFrameBaseConfig().GetLogLevel());

	//建立连接
	ret = Connect();
	if (0 > ret)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]build connection with mysql server failed! \nszHost=%s, \nszUser=%s, \nszPassword=%s, \nszDBName=%s, \nnPort=%d, \nthreadindex=%d, errorcode=0x%08X\n",
			__FILE__, 
			__LINE__, 
			szHost, 
			szUser, 
			szPassword, 
			szDBName, 
			port, 
			threadIndex, 
			ret);

		m_mysqlConnectionStatus = enmMysqlConnectionStatus_Closed;
	}
	else
	{
		m_mysqlConnectionStatus = enmMysqlConnectionStatus_Connected;
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]build connection with mysql server success! \nszHost=%s, \nszUser=%s, \nszPassword=%s, \nszDBName=%s, \nnPort=%d, \nthreadindex=%d\n",
			__FILE__, 
			__LINE__, 
			szHost, 
			szUser, 
			szPassword, 
			szDBName, 
			port, 
			threadIndex);
	}
	m_nListIndex = threadIndex;

	return S_OK;
}

int32_t CDBEngine::Connect()
{
	int32_t ret = m_mysqlEngine.Connect();
	if (0 > ret)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]connect mysql server failed! errorcode=0x%08X\n", __FILE__, __LINE__, ret);
		return ret;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]connect mysql server success! threadindex=%d\n", __FILE__, __LINE__);

	ret = m_mysqlReadEngine.Connect();
	if(0 > ret)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]read dbengine connect mysql server failed! errorcode=0x%08X\n",
			__FILE__, __LINE__, ret);
		return ret;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "[%s:%d]read dbengine connect mysql server success!\n", __FILE__, __LINE__);

	m_mysqlConnectionStatus = enmMysqlConnectionStatus_Connected;
	return S_OK;
}

//恢复数据库引擎对象
int32_t CDBEngine::Resume(const char* szHost, const char* szUser, const char* szPassword, const char* szDBName, int32_t nPort, int32_t threadIndex)
{
	return Initialize(szHost, szUser, szPassword, szDBName, nPort, threadIndex);
}

//注销数据库引擎对象
int32_t CDBEngine::Uninitialize()
{
	//	m_mysqlReserver.Uninitialize();     //考虑备份的话，程序变得很复杂，暂不予考虑
	int32_t ret = S_OK;
	ret = m_mysqlEngine.Disconnect();
	if(0 > ret)
	{
		return ret;
	}
	ret = m_mysqlReadEngine.Disconnect();
	if(0 > ret)
	{
		return ret;
	}

	return S_OK;
}

////导入备份数据到数据库中，备份会大大增加程序的复杂度，而且效果也不见得很好，暂不予考虑
//bool CDBEngine::ImportReserve()
//{
//	//生成SQL语句
//	char szSql[enmMaxSqlStringLength] = { 0 };
//	uint32_t uLen = 0;
//	int32_t ret = S_OK;
//	bool	bSuccess = true;
//
//	////先将备份数据更新到数据库中
//	if(m_mysqlReserver.GetReserveCount())
//	{
//		while(S_OK == m_mysqlReserver.GetReserve(szSql, uLen))
//		{
//			uint64_t nAffectedRows = 0;
//			ret = m_mysqlEngine.ExecuteUpdate(szSql, nAffectedRows);
//			if(0 > ret)
//			{
//				bSuccess = false;
//				return bSuccess;
//			}
//			BakSql(szSql);
//			//todo: do log 
//			//WRITE_DBACCESS_LOG(m_nListIndex, enmLogLevel_Debug, "write sql to mysql from reserve file success,sql = %s\n", szSql);
//			m_mysqlReserver.NextReserve();
//		}
//		if(!m_mysqlReserver.GetReserveCount())
//		{
//			m_mysqlReserver.ClearReserve();
//		}
//
//
//	}
//	return bSuccess;
//}

int32_t CDBEngine::GetLastMysqlError()
{
	return m_mysqlEngine.GetLastMysqlError();
}

void CDBEngine::BakSql(const char* szSql)
{
	CDateTime dt = CDateTime::CurrentDateTime();
	char szLog[enmMaxLogInfoLength]/* = { 0 }*/;
	memset(szLog, 0, sizeof(szLog));
	sprintf(szLog, "%04d-%02d-%02d %02d:%02d:%02d | %s\n", dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second(),szSql);

	//m_sqlLogger.DoLog(enmLogLevel_Notice,"%s",szLog);
}

int32_t CDBEngine::Disconnect()
{
	m_mysqlConnectionStatus = enmMysqlConnectionStatus_Closed;
	return m_mysqlEngine.Disconnect();
}

bool CDBEngine::IsConnected()
{
	return m_mysqlConnectionStatus == enmMysqlConnectionStatus_Connected;
}

FRAME_DBPROXY_NAMESPACE_END

