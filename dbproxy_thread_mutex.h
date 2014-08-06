/***********************************************************************
* filename: dbproxy_thread_mutex.h
* author:   zerok
* date:     2012-06-26 17:08
* purpose:  ���̼߳�������Щ��������ͬʱ�������̵߳��õ�������һЩ
			Ī��������������صĿ��ܵ���崻������Ա���Ҫ����
***********************************************************************/

#ifndef  __DBPROXY_THREADMUTEX_H
#define  __DBPROXY_THREADMUTEX_H

#if WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#endif 
#include <winsock2.h>
#include <windows.h>
#include <Winbase.h>

class CDBCriticalSection
{
public:
	CDBCriticalSection()
	{
		InitializeCriticalSection(&crit_);
	};

	~CDBCriticalSection()
	{
		DeleteCriticalSection(&crit_);
	};

	void enter()
	{
		EnterCriticalSection(&crit_);
	};

	bool try_enter()
	{
		return (TryEnterCriticalSection(&crit_) != FALSE) ? true : false;
	};

	void leave()
	{
		LeaveCriticalSection(&crit_);
	};

private:
	CRITICAL_SECTION crit_;
};

#else
#include <pthread.h>

class CDBCriticalSection
{
public:
	CDBCriticalSection()
	{
		pthread_mutexattr_t mutex_attribute;
		pthread_mutexattr_init(&mutex_attribute);
		pthread_mutexattr_settype(&mutex_attribute, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&mutex_, &mutex_attribute);
		pthread_mutexattr_destroy(&mutex_attribute);
	};

	~CDBCriticalSection()
	{
		pthread_mutex_destroy(&mutex_);
	};

	void enter()
	{
		pthread_mutex_lock(&mutex_);
	};

	bool try_enter()
	{
		return pthread_mutex_trylock(&mutex_) == 0 ? true : false;
	};

	void leave()
	{
		pthread_mutex_unlock(&mutex_);
	};

private:
	pthread_mutex_t mutex_;
};
#endif

class CDBThreadMutex
{
public:
	CDBThreadMutex(CDBCriticalSection &cirt) : crit_(cirt)
	{
		cirt.enter();
	};

	~CDBThreadMutex()
	{
		crit_.leave();
	};

private:
	CDBCriticalSection& crit_;
};

#define DB_MUTEX_GUARD(M, C)				CDBThreadMutex M(C);
#define DB_MUTEX_GUARD_RETURN(M, C, R)		if(!C.try_enter()){return (R);};\
	C.leave();\
	DB_MUTEX_GUARD(M, C);


#endif
