/**********************************************************************
* filename: memcache_conncheck_thread.cpp
* date:     2012-05-08 10:12
* author:   xiaohq
* purpose:  memcache连接检测线程的具体实现
*
***********************************************************************/

#include "memcache_conncheck_thread.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

CMemcacheConnCheckThread::CMemcacheConnCheckThread()
{
}

CMemcacheConnCheckThread::~CMemcacheConnCheckThread()
{
}

int32_t CMemcacheConnCheckThread::Initialize()
{
	int32_t ret = S_OK;

	ret = GET_VDCMEMCACHE_INSTANCE().Initialize();
	if(0 > ret)
	{
		g_Frame.g_bMemcacheConnected = false;
		return ret;
	}


	g_Frame.g_bMemcacheConnected = true;
	return  S_OK;
}

int32_t CMemcacheConnCheckThread::Resume()
{
	return S_OK;
}

int32_t CMemcacheConnCheckThread::Uninitialize()
{
	return S_OK;
}

void CMemcacheConnCheckThread::Execute()
{
	while( !GetTerminated() )
	{
		CheckMemcacheConnection();

		//休眠3s
		Delay(3000000);
	}
}

void   CMemcacheConnCheckThread::CheckMemcacheConnection()
{
	if(g_Frame.g_bMemcacheConnected == false)
	{
		////每次重新连接memcache服务器之前，先将memcache清空，防止数据库和cache中的数据不一致
		//if(S_OK == ClearMemcache())
		//{
		//	//初始化与memcache的连接
		//}

		//只能重新初始化，因为如果在程序中调用clear_memcache.sh，会有灭顶之灾，因为程序
		//会因为调用这个脚本而退出，所以，只能手动使用这个脚本
		GET_VDCMEMCACHE_INSTANCE().Initialize();
	}
}

FRAME_DBPROXY_NAMESPACE_END
//

