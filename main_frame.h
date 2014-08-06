/*
 * main_frame.h
 *
 *  Created on: 2011-12-2
 *      Author: jimm
 */

#ifndef MAIN_FRAME_H_
#define MAIN_FRAME_H_

#include "memcache_dbproxy/memcache_conncheck_thread.h"
#include "memcache_dbproxy/vdc_memcache.h"
#include "lightframe.h"
#include "common/common_errordef.h"
#include "common/common_singleton.h"
#include "def/def_dbproxy.h"
#include "def/server_namespace.h"
#include "dbengine.h"


FRAME_DBPROXY_NAMESPACE_BEGIN

class CMainFrame : public CLightFrame
{
public:
	CMainFrame();
	virtual ~CMainFrame();

public:
	//¿ò¼Ü³õÊ¼»°
	int32_t Initialize();
	//»Ö¸´¿ò¼Ü
	int32_t Resume();
	//Ïú»Ù¿ò¼Ü
	int32_t Terminate();

	void Run();

	void Reload();

	CDBEngine * GetDBEngine(int32_t nDBIndex)
	{
		return m_arrDBEngine[nDBIndex];
	}


protected:

	void RegistMsg();

	int32_t CreateDBEngine(int32_t nCount);

public:
	static bool  g_bMemcacheConnected;
protected:
	int32_t				m_nDBEngineCount;
	CDBEngine			*m_arrDBEngine[enmMaxAppThreadCount];
};

#define	CREATE_MAINFRAME_INSTANCE	CSingleton<CMainFrame>::CreateInstance
#define	GET_MAINFRAME_INSTANCE		CSingleton<CMainFrame>::GetInstance
#define	DESTROY_MAINFRAME_INSTANCE	CSingleton<CMainFrame>::DestroyInstance

#define g_Frame	GET_MAINFRAME_INSTANCE()


#define WRITE_LOG(level, fmt, ...) \
	do{ g_Frame.WriteLog(level, __FILE__,  __LINE__,""fmt"\n", ##__VA_ARGS__); } while (false)

#define WRITE_DEBUG_LOG(fmt, ...)		WRITE_LOG(enmLogLevel_Debug, fmt, ##__VA_ARGS__)
#define WRITE_INFO_LOG(fmt, ...)	WRITE_LOG(enmLogLevel_Info, fmt, ##__VA_ARGS__)
#define WRITE_NOTICE_LOG(fmt, ...)	WRITE_LOG(enmLogLevel_Notice, fmt, ##__VA_ARGS__)
#define WRITE_WARNING_LOG(fmt, ...)	WRITE_LOG(enmLogLevel_Warning, fmt, ##__VA_ARGS__)
#define WRITE_ERROR_LOG(fmt, ...)	WRITE_LOG(enmLogLevel_Error, fmt, ##__VA_ARGS__)

FRAME_DBPROXY_NAMESPACE_END


#endif /* MAIN_FRAME_H_ */
