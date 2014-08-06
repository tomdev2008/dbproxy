/************************************************************
* filename: utils.h
* date:     2011-12-09  9:41
* author:   xiaohq
* version:  1.0
* purpose:  ����DB�����������Ҫ��һЩ�����ӿڣ���Ҫ������
            ���ַ���֮���໥ת���Ľӿ�
* revision: no
************************************************************/

#ifndef   __DBPROXY_UTILS_H__
#define   __DBPROXY_UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "def/def_dbproxy.h"
#include "common/common_typedef.h"
#include "../public/public_typedef.h"
#include "auth/md5.h"

FRAME_DBPROXY_NAMESPACE_BEGIN


//���ȡMessageHeadSS��ͷ������
int32_t GetMsgHeadSSMinHeadLen();

//��DB�����������������ݰ��н������������ݰ���
int32_t GetPackageLenFromStream(const uint8_t* pBuffer);

//��DB�����������������ݰ��н�����������Ϣͷ����һ���ڴ������
int32_t GetMsgHeadIdxFromRecvBuf(const uint8_t* pBuffer, const int32_t nLength, int32_t& msgHeadIdx);

int32_t GetMsgIDFromRecvBuf(const uint8_t* pBuf, const int32_t nLen);

void GetRoomCountAndID(const char* szRoomInfo, const int32_t roomInfoLen, int32_t& roomCount, int32_t  szRoomID[]);

void ConsRoomInfoBuf(char* szRoomInfo, int32_t& roominfolen, const int32_t roomCount, const int32_t szRoomID[]);

void DumpMemWithByte(char* szBinaryBuf, int32_t& binaryBufLen, const char* szTmpBuf, const int32_t nBufLen, const char* szContent);

void GenerateMemcacheKey(char key[], const size_t max_key_len, size_t& keylen, const int32_t storeType, const int32_t storeVal);

void GenerateMemcacheKeyForWeb(char key[], const size_t max_key_len, size_t& keylen, const int32_t storeType, const int32_t storeVal);

void GenerateRoomChannelKey(char key[], const size_t max_key_len, size_t& keylen, const int32_t storetype, const ChannelID channelid);

void GenerateRecommandRoomChannelKey(char key[], const size_t max_key_len, size_t& keylen, const int32_t storetype, const ChannelID channelid);

/*
*  ���ɷ��䵱ǰ����������key
*/
void GeneratePlayerCountKey(char key[], const size_t max_key_len, size_t& keylen, const int32_t storetype, const RoomID roomid);

/*
*  ����lastweek.sh���shell�ű��ĺ������ýű���vdc_dbproxyserver_dbg����ͬһ��Ŀ¼
*/
void GetLastWeek(int32_t& lastWeek);

/*
*  ����clear_memcache.sh����ű��ĺ������ýű���vdc_dbproxyserver_dbg����ͬһ��Ŀ¼�����ܵ��ã���Ϊ��ʹ�����޷�������
*  ������ʱ�����ֶ���ջ��濿��
*/
int32_t ClearMemcache();

/*
*  IPת���� ������ת�������ʮ���Ƹ�ʽ���ַ���
*/
char * IPTrans_from_int_to_string(const int32_t nIP);

/*
*  IPת���� ������ת�������ʮ���Ƹ�ʽ���ַ���(���̰߳�ȫ)
*/
char * inet_ntoa_s(uint32_t ip, char* strIP);
/*
* IPת���� �ӵ��ʮ����ת��������
*/
int32_t IPTrans_from_string_to_int(const char* szIP);

/*
*  ����һ����������ȡ���ַ�����Ӧ��MD5��
*/
char* GetMD5(const RoleID roleid);

/*
*  �ַ�������
*/
void strcpy_safe(char* szDest, const int32_t max_dest_buf_len, const char* szSrc, const int32_t src_buf_len);

/*
* ���ַ����л�ȡIP��ַ�Ͷ�����
*/
void get_ip_ratio(char* ip_ratio, char ip[], int32_t& ratio);

/*
* ���ַ����л�ȡ�������ͣ� ip�� ������
*/
void get_collect_info(char* content, char szNetName[], const int32_t max_name_len, char szIP[], const int32_t max_ip_len, int32_t& nRatio);

/*
* ���������еĸ����ֶ�
*/
void get_content(char* content, char res[][MaxFieldLenInNetStatus],const int32_t max_field_count,  int32_t& count);

//��ȡip����������
void get_ip_nettype(char* content, char res[][MaxFieldLenInNetStatus],const int32_t max_field_count,  int32_t& count, char* ip, char* nettype);

//�����ͻ�������״��ͳ������
 void analysis_content(char* content, char res[][MaxFieldLenInNetStatus],const int32_t max_field_count,  int32_t& count, const int32_t updown, int32_t nLost[]);

FRAME_DBPROXY_NAMESPACE_END

#endif/*__DBPROXY_UTILS_H__*/

