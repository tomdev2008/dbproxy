/*************************************************************************
* filename: dbengine_merge.h
* date:     2011-12-26 19:53
* author:   xiaohq
* purpose:  将基础数据库引擎与dbproxy的dbengine合并，减少代码复杂度，同时
			功能得到保障
*************************************************************************/
#ifndef _DBPROXY_BASE_DBENGINE_H_
#define _DBPROXY_BASE_DBENGINE_H_

#include "common/common_typedef.h"
#include "common/common_mysqlengine.h"
#include "def/def_dbproxy.h"
#include "def/server_namespace.h"
#include "frame_typedef.h"

#include "config/dbserver_config.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

class CDBEngine
{
public:
	CDBEngine();
	virtual ~CDBEngine();

public:
	int32_t Initialize(const char* szHost, const char* szUser, const char* szPassword, const char* szDBName,int32_t port, int32_t nListIndex);
	int32_t Resume(const char* szHost, const char* szUser, const char* szPassword, const char* szDBName, int32_t port, int32_t nListIndex);
	int32_t Uninitialize();

public:
	int32_t   GetLastMysqlError();
	int32_t   Connect();
	bool      IsConnected();
	int32_t   Disconnect();
	void      BakSql(const char* szSql);

public:
	CMysqlEngine			m_mysqlEngine;
	int32_t					m_nListIndex;  //其实就是db线程的索引，如果db线程有4个，则m_nListIndex的值就是从0到3
	uint8_t					m_mysqlConnectionStatus;

	//读取引擎相关
	CMysqlEngine            m_mysqlReadEngine;
	int32_t                 m_nReadDBServerCount;
	DBConfigInfo            m_arrDBConfigInfo[enmMaxDBServerCount];

	//具体业务
};

class CMysqlGuard
{
public:
	CMysqlGuard(CMysqlEngine& engine):m_stMysqlEngine(engine)
	{
	}

	~CMysqlGuard()
	{
		m_stMysqlEngine.ExecuteQueryEnd();
	}

	int32_t ExecuteQuery(const char* szSql, uint32_t& nFieldCount, uint32_t& nRowCount)
	{
		return m_stMysqlEngine.ExecuteQueryBegin(szSql, nFieldCount, nRowCount);
	}


public:
	CMysqlEngine &m_stMysqlEngine;
};

FRAME_DBPROXY_NAMESPACE_END

#endif



