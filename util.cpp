/********************************************************************
* filename: util.cpp
* date:     2011-12-16 9:44
* author:   xiaohq
* version:  x
* purpose:  定义一些通用的接口
* revision: no
********************************************************************/

#include "common/common_errordef.h"
#include "main_frame.h"
#include "def/def_dbproxy.h"
#include "def/def_dbproxy_errorcode.h"
#include "def/def_dbproxy.h"
#include "../public/public_typedef.h"
#include "../lightframe/auth/md5.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

int32_t GetMsgHeadSSMinHeadLen()
{
	int32_t nTmpMsgHeadSize = 0;
	nTmpMsgHeadSize = 8 * sizeof(int32_t) + 1 * sizeof(int16_t) + 4 * sizeof(int8_t);
	return nTmpMsgHeadSize;
}

int32_t GetPackageLenFromStream(const uint8_t* pBuffer)
{
	int32_t nTmpPkgLen = 0; 

	//TODO:  
	return nTmpPkgLen;
}

int32_t GetMsgHeadIdxFromRecvBuf(const uint8_t* pBuffer, const int32_t nLength, int32_t& msgHeadIdx)
{
	if(pBuffer == NULL || nLength <= 0)
	{
		WRITE_ERROR_LOG("Error: get msghead index from recv buffer failed!\n");
		return E_INVALID_ARG_IN_GET_MSGHEAD_IDX;
	}

	//TODO: 从流格式的数据中得到消息头的索引，并赋值给msgHeadIdx

	return  S_OK;
}

int32_t GetMsgIDFromRecvBuf(const uint8_t* pBuf, const int32_t nLen)
{
	return S_OK;
}

void GetRoomCountAndID(const char* szRoomInfo, const int32_t roomInfoLen, int32_t& roomCount, int32_t szRoomID[])
{
	if(szRoomInfo == NULL)
	{
		WRITE_WARNING_LOG("room info in table user_base_info is null, maybe this user has no room!");
		return;
	}

	//因为roomInfoLen是用strlen算出来的，没有包含'\0', 因此，如果roomInfoLen = enmMaxRoomIDSBufLength, 那么字符串中已经没有'\0'的位置，
	//这种情况会导致下面的循环找不到'\0',从而进行无限循环，导致很严重的后果
	if(roomInfoLen >= (int32_t)enmMaxRoomIDSBufLength || roomInfoLen < 0)
	{
		WRITE_WARNING_LOG("Error: invalid room info length! roominfolen=%d\n", roomInfoLen);
		return;
	}

	char szRoomInfoBak[enmMaxRoomIDSBufLength] = {0};
	strcpy(szRoomInfoBak, szRoomInfo);
	szRoomInfoBak[roomInfoLen] = '\0';
	char* pHead = szRoomInfoBak;
	char* pTail = pHead;

	roomCount = 0;
	char szTmp[enmMaxRoomIDStringLength] = {0};
	
	while(*pHead != '\0')
	{
		pTail = pHead;
		while(*pTail != ';')
		{
			pTail++;
		}
		strncpy(szTmp, pHead, pTail - pHead);
		szRoomID[roomCount] = atoi(szTmp);
		roomCount++;
		memset(szTmp, 0, sizeof(szTmp));

		pTail++;
		pHead = pTail;
	}
}

void ConsRoomInfoBuf(char* szRoomInfo, int32_t& roomInfoLen, const int32_t roomCount, const int32_t szRoomID[])
{
	int32_t offset = 0;
	for(int32_t i=0; i<roomCount; ++i)
	{
		sprintf(szRoomInfo + offset, "%d;", szRoomID[i]);
		offset = strlen(szRoomInfo);
	}

	roomInfoLen = strlen(szRoomInfo);
}

void DumpMemWithByte(char* szBinaryBuf, int32_t& binaryBufLen, const char* szTmpBuf, const int32_t nBufLen, const char* szContent)
{
	if(NULL == szTmpBuf)
	{
		WRITE_ERROR_LOG("Error: the tmpbuf is null!\n");
		return;
	}

	if(nBufLen >= enmMaxMemcacheValueLen)
	{
		WRITE_ERROR_LOG("Error: the tmebuf size is too large! more than max memcache value length! max memcache value length is %d, but the tmpbuf size is: %d\n", enmMaxMemcacheValueLen, nBufLen);
		return;
	}
	
	binaryBufLen = 0;
	sprintf(szBinaryBuf + binaryBufLen, "%s", szContent);
	binaryBufLen = (int32_t)strlen(szBinaryBuf);

	for(int32_t i=0; i<nBufLen; ++i)
	{
		if(i % 8 == 0)
		{
			sprintf(szBinaryBuf + binaryBufLen, "\n");
			binaryBufLen = (int32_t)strlen(szBinaryBuf);
		}
		sprintf(szBinaryBuf + strlen(szBinaryBuf), "%02X", szTmpBuf[i]);
	}
}

void GenerateMemcacheKey(char key[], const size_t max_key_len, size_t& keylen, const int32_t storeType, const int32_t storeVal)
{
	memset(key, 0, max_key_len);
	sprintf(key + strlen(key), "%d", storeType);
	sprintf(key + strlen(key), "%d", storeVal);

	keylen = strlen(key);
}

void GenerateMemcacheKeyForWeb(char key[], const size_t max_key_len, size_t& keylen, const int32_t storeType, const int32_t storeVal)
{
	memset(key, 0, max_key_len);
	switch(storeType)
	{
	case enmStoreType_RoleID:
		sprintf(key + strlen(key), "user_");
		sprintf(key + strlen(key), "%d", storeVal);
		keylen = strlen(key);
		break;
	default:
		break;
	}
}

void GenerateRoomChannelKey(char key[], const size_t max_key_len, size_t& keylen, const int32_t storetype, const ChannelID channelid)
{
	memset(key, 0, max_key_len);
	sprintf(key + strlen(key), "channelRoomList%d", channelid);
	keylen = strlen(key);
}

void GenerateRecommandRoomChannelKey(char key[], const size_t max_key_len, size_t& keylen, const int32_t storetype, const ChannelID channelid)
{
	memset(key, 0, max_key_len);
	sprintf(key + strlen(key), "channelRecomRoomID%d", channelid);
	keylen = strlen(key);
}

void GetLastWeek(int32_t& lastWeek)
{
	FILE* fp;
	char buffer[10];
	fp = fopen("./lastweek.txt", "r");
	if(fp == NULL)
	{
		return;
	}
	fgets(buffer, sizeof(buffer), fp);
	//printf("%s", buffer);
	lastWeek = atoi(buffer);
	fclose(fp);
}

 int32_t ClearMemcache()
 {
	 char temp[100] = "./clear_memcache.sh";
	 int32_t ret = S_OK;
	 ret = system(temp);
	 if(0 != ret)
	 {
		 return E_CALL_CLEAR_MEMCACHE_SH;
	 }

	 return S_OK;
 }

 /*
 *  IP转换： 从整数转换到点分十进制格式的字符串
 */
 char * IPTrans_from_int_to_string(const int32_t nIP)
 {
	 struct in_addr addr_char;
	 memcpy(&addr_char, &nIP, 4);
	 return inet_ntoa(addr_char);
 }

 char * inet_ntoa_s(uint32_t ip, char* strIP)
 {
 	char buf[20]={0};
 	uint8_t *str = (uint8_t *)&ip;
 	sprintf(buf, "%d.%d.%d.%d", str[0] & 0xff, str[1] & 0xff, str[2] & 0xff, str[3] & 0xff);
 	strcpy(strIP, buf);
 	return strIP;
 }

 /*
 * IP转换： 从点分十进制转换到整数
 */
 int32_t IPTrans_from_string_to_int(const char* szIP)
 {
	 return inet_addr(szIP);
 }

 /*
 *  生成房间当前在线人数的key
 */
 void GeneratePlayerCountKey(char key[], const size_t max_key_len, size_t& keylen, const int32_t storetype, const RoomID roomid)
 {
	memset(key, 0, max_key_len);
	switch(storetype)
	{
	case enmStoreType_RoomPlayerCount:
		sprintf(key, "player_count_%d", roomid);
		break;
	case enmStoreType_RoomRobotCount:
		sprintf(key, "robot_count_%d", roomid);
		break;
	default:
		break;
	}

	keylen = strlen(key);
 }

 /*
 *  输入一个整数，获取其字符串对应的MD5码
 */
 char* GetMD5(const RoleID roleid)
 {
	 char arrA[16] = {0};
	 sprintf(arrA, "%d", roleid);
	 int32_t iInputLength = strlen(arrA);

	 char arrOut[32] = {0};
	 char *pResult = NULL;

	 pResult = md5_string((const unsigned char*)arrA, arrOut, iInputLength);
	 return pResult;
 }

 /*
 *  字符串拷贝
 */
 void strcpy_safe(char* szDest, const int32_t max_dest_buf_len, const char* szSrc, const int32_t src_buf_len)
 {
	 if(src_buf_len >= max_dest_buf_len)
	 {
		 strncpy(szDest, szSrc, max_dest_buf_len - 1);
		 szDest[max_dest_buf_len - 1] = '\0';
		 return;
	 }
	 else
	 {
		 strcpy(szDest, szSrc);
	 }
 }

 /*
 * 从字符串中获取IP地址和丢包率
 */
 void get_ip_ratio(char* ip_ratio, char ip[], int32_t& ratio)
 {
	 if(ip_ratio == NULL)
	 {
		 return;
	 }

	 char* p = ip_ratio;
	 int32_t index = 0;
	 while(*p != ':')
	 {
		 p++;
		 index++;
	 }

	 strncpy(ip, ip_ratio, index);
	 char num[5] = {0};

	 index++;
	 strncpy(num, &ip_ratio[index], strlen(ip_ratio) - index);
	 ratio = atoi(num);
 }

 void get_collect_info(char* content, char szNetName[], const int32_t max_name_len, char szIP[], const int32_t max_ip_len, int32_t& nRatio)
 {
	 if(NULL == content || szNetName == NULL || szIP == NULL)
	 {
		 return;
	 }

	 char* p = content;
	 int32_t name_index = 0;
	 while(*p != ':')
	 {
		 p++;
		 name_index++;
	 }
	 if(name_index >= max_name_len)
	 {
		 //错误日志
		 return;
	 }
	 strncpy(szNetName, content, name_index);

	 int32_t ip_index = name_index;
	 p++;

	 while(*p != ':')
	 {
		 p++;
		 ip_index++;
	 }
	 if((ip_index - name_index) >= max_ip_len)
	 {
		 //error log
		 return;
	 }
	 strncpy(szIP, &content[name_index+1], ip_index - name_index);

	 char num[5] = {0};
	 ip_index++;

	 if((strlen(content) - ip_index - 1) >= 5)
	 {
		 return;
	 }
	 strncpy(num, &content[ip_index + 1], strlen(content) - ip_index - 1);
	 nRatio = atoi(num);

	 //printf("netname=%s, ip=%s, ratio=%d\n", szNetName, szIP, nRatio);
 }

 void get_content(char* content, char res[][MaxFieldLenInNetStatus],const int32_t max_field_count,  int32_t& count)
 {	
	 if(NULL == content)
	 {
		 return;
	 }

	 count = 0;
	 for(int32_t i=0; i<(int32_t)strlen(content); ++i)
	 {
		 if(content[i] == ':')
		 {
			 count++;
		 }
	 }

	 if(CurFieldCountInNetStatus != (count + 1))
	 {
		 return;
	 }

	 int32_t cnt_count = 0;
	 int32_t pindex = 0;
	 int32_t resindex = 0;
	 while(cnt_count < count)
	 {
		 if(content[pindex] == ':')
		 {
			 cnt_count++;
			 resindex= 0;
			 pindex++;
		 }
		 else
		 {
			 if(resindex >= MaxFieldLenInNetStatus)
			 {
				 return;
			 }
			 res[cnt_count][resindex] = content[pindex];
			 pindex++;
			 resindex++;
		 }
	 }

	 //拷贝最后一个字段
	 resindex = 0;

	 if(strlen(content) - pindex >= MaxFieldLenInNetStatus)
	 {
		 strncpy(res[count], &content[pindex], MaxFieldLenInNetStatus - 1);
	 }
	 else
	 {
		 strncpy(res[count], &content[pindex], strlen(content) - pindex);
	 }

	 //printf("count=%d\n", count);
	 //for(int32_t i=0; i<=count; ++i)
	 //{
		// printf("res[%d]=%s\n", i, res[i]);
	 //}
 }

 void get_ip_nettype(char* content, char res[][MaxFieldLenInNetStatus],const int32_t max_field_count,  int32_t& count, char* ip, char* nettype)
 {
	 if(NULL == content)
	 {
		 return;
	 }

	 count = 0;
	 for(int32_t i=0; i<(int32_t)strlen(content); ++i)
	 {
		 if(content[i] == ':')
		 {
			 count++;
		 }
	 }

	 if(CurFieldCountInNetStatus != (count + 1))
	 {
		 return;
	 }

	 int32_t cnt_count = 0;
	 int32_t pindex = 0;
	 int32_t resindex = 0;
	 while(cnt_count < count)
	 {
		 if(content[pindex] == ':')
		 {
			 cnt_count++;
			 resindex= 0;
			 pindex++;
		 }
		 else
		 {
			 if(resindex >= MaxFieldLenInNetStatus)
			 {
				 return;
			 }
			 res[cnt_count][resindex] = content[pindex];
			 pindex++;
			 resindex++;
		 }
	 }

	 //拷贝最后一个字段
	 resindex = 0;

	 if(strlen(content) - pindex >= MaxFieldLenInNetStatus)
	 {
		 strncpy(res[count], &content[pindex], MaxFieldLenInNetStatus - 1);
	 }
	 else
	 {
		 strncpy(res[count], &content[pindex], strlen(content) - pindex);
	 }

	strcpy_safe(ip, enmMaxIPAddressLength, res[2], MaxFieldLenInNetStatus);
	strcpy_safe(nettype, MaxNetTypeLen, res[0], MaxFieldLenInNetStatus);
 }

 void analysis_content(char* content, char res[][MaxFieldLenInNetStatus],const int32_t max_field_count,  int32_t& count, const int32_t updown, int32_t nLost[])
 {
	 if(NULL == content)
	 {
		 return;
	 }

	 count = 0;
	 for(int32_t i=0; i<(int32_t)strlen(content); ++i)
	 {
		 if(content[i] == ':')
		 {
			 count++;
		 }
	 }

	 if(CurFieldCountInNetStatus != (count + 1))
	 {
		 return;
	 }

	 int32_t cnt_count = 0;
	 int32_t pindex = 0;
	 int32_t resindex = 0;
	 while(cnt_count < count)
	 {
		 if(content[pindex] == ':')
		 {
			 cnt_count++;
			 resindex= 0;
			 pindex++;
		 }
		 else
		 {
			 if(resindex >= MaxFieldLenInNetStatus)
			 {
				 return;
			 }
			 res[cnt_count][resindex] = content[pindex];
			 pindex++;
			 resindex++;
		 }
	 }

	 //拷贝最后一个字段
	 resindex = 0;

	 if(strlen(content) - pindex >= MaxFieldLenInNetStatus)
	 {
		 strncpy(res[count], &content[pindex], MaxFieldLenInNetStatus - 1);
	 }
	 else
	 {
		 strncpy(res[count], &content[pindex], strlen(content) - pindex);
	 }

	if(updown == enmTransDirection_Up)//上传
	{
		if(strcmp(res[1], "Video") == 0)  //视频上传
		{
			nLost[0] = atoi(res[3]);
		}
		else
		{
			nLost[2] = atoi(res[3]);  //音频上传
		}
	}
	else //下载
	{
		if(strcmp(res[1], "Video") == 0)
		{
			nLost[1] = atoi(res[3]);
		}
		else
		{
			nLost[3] = atoi(res[3]);
		}
	}
 }


//char szTmpLog[enmMaxStringLength] = {0};
//int32_t nTmpLogLen = 0;
//for(uint32_t i=0; i<nMemcacheValLen; ++i)
//{
//	sprintf(szTmpLog + nTmpLogLen, "%02X", szTmpLog[i]);
//	if(i % 8 == 0)
//	{
//		sprintf(szTmpLog + nTmpLogLen, "\n");
//		nTmpLogLen = strlen(szTmpLog);
//	}
//	nTmpLogLen = strlen(szTmpLog);
//}
//WRITE_THREAD_DEBUG_LOG(m_nThreadIndex, "get giver info obtained from memcache buffer, the key is %s, keylen=%d, valuelength=%d, value is :\n%s\n", szKey, nKeyLen, nMemcacheValLen, szTmpLog);


FRAME_DBPROXY_NAMESPACE_END

