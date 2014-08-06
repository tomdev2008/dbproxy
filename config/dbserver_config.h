/*******************************************************************
* fileanme: dbserver_config.h
* date:     2011-12-13  10:10
* author:   xiaohq
* version:  x
* purpose:  ���ݿ��������������Ϣ����Ҫ����IP�Ͷ˿ں�
* revision: no
*******************************************************************/

#ifndef _DBPROXY_DBSERVER_CONFIG_H
#define _DBPROXY_DBSERVER_CONFIG_H

#include "common/common_singleton.h"
#include "common/common_def.h"
#include "../def/server_namespace.h"
#include "lightframe_impl.h"
#include "frame_typedef.h"
#include "../def/def_dbproxy.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

//�������ݿ������ļ����� ע�������ļ����������ռ����涨��
#define  DEFAULT_DBCONFIG_FILENAME     "./config/dbconfig.xml"

typedef struct tagDBConfigInfo
{
	char szDBIP[enmMaxIPAddressLength];
	uint16_t nDBPort;
	char szUser[enmMaxRoleNameLength];
	char szPassword[enmMaxDBPasswordLength];
	char szDBName[enmMaxDBNameLength];
}DBConfigInfo;

enum
{
	enmMaxDBServerCount		= 4,
};

class CDBConfig: public  IConfigCenter
{
public:
	CDBConfig();
	virtual ~CDBConfig();

public:
	int32_t Initialize(const char* szFileName = NULL, const int32_t type=0);
	int32_t Reload(const char* szFileName = NULL, const int32_t type=0);
	int32_t Uninitialize();

public:
	//��ȡд��������ݿ�����
	int32_t GetDBConfigInfo(int32_t& nDBServerCount, DBConfigInfo arrDBConfigInfo[]);
	//��ȡ����������ݿ����ã� Ҳ����slave�ڵ���������ݿ�����
	int32_t GetRDDBConfig(int32_t& nRDDBServerCount, DBConfigInfo  arrRDDBConfigInfo[]);

private:
	DBConfigInfo  m_stDBConfigInfo[2][enmMaxDBServerCount];
	DBConfigInfo  m_arrRDDBConfigInfo[2][enmMaxDBServerCount];
	int32_t	 m_nDBServerCount[2];
	int32_t  m_nRDDBServerCount[2];
	int32_t  m_nLoadIndex;
	int32_t  m_nUseIndex;
};

#define CREATE_DBCONFIG_INSTANCE     CSingleton<CDBConfig>::CreateInstance
#define GET_DBCONFIG_INSTANCE        CSingleton<CDBConfig>::GetInstance
#define DESTROY_DBCONFIG_INSTANCE    CSingleton<CDBConfig>::DestroyInstance

FRAME_DBPROXY_NAMESPACE_END

#endif
