/**********************************************************************
* filename: memcache_conncheck_thread.cpp
* date:     2012-05-08 10:12
* author:   xiaohq
* purpose:  memcache���Ӽ���̵߳ľ���ʵ��
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

		//����3s
		Delay(3000000);
	}
}

void   CMemcacheConnCheckThread::CheckMemcacheConnection()
{
	if(g_Frame.g_bMemcacheConnected == false)
	{
		////ÿ����������memcache������֮ǰ���Ƚ�memcache��գ���ֹ���ݿ��cache�е����ݲ�һ��
		//if(S_OK == ClearMemcache())
		//{
		//	//��ʼ����memcache������
		//}

		//ֻ�����³�ʼ������Ϊ����ڳ����е���clear_memcache.sh��������֮�֣���Ϊ����
		//����Ϊ��������ű����˳������ԣ�ֻ���ֶ�ʹ������ű�
		GET_VDCMEMCACHE_INSTANCE().Initialize();
	}
}

FRAME_DBPROXY_NAMESPACE_END
//

