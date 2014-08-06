/************************************************************
* filename: utils.h
* date:     2011-12-09  9:41
* author:   xiaohq
* version:  1.0
* purpose:  定义DB代理服务器需要的一些公共接口，主要是整数
            和字符串之间相互转换的接口
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


//或获取MessageHeadSS的头部长度
int32_t GetMsgHeadSSMinHeadLen();

//从DB服务器发过来的数据包中解析出整个数据包长
int32_t GetPackageLenFromStream(const uint8_t* pBuffer);

//从DB服务器发过来的数据包中解析出保存消息头的那一块内存的索引
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
*  生成房间当前在线人数的key
*/
void GeneratePlayerCountKey(char key[], const size_t max_key_len, size_t& keylen, const int32_t storetype, const RoomID roomid);

/*
*  调用lastweek.sh这个shell脚本的函数，该脚本和vdc_dbproxyserver_dbg处于同一个目录
*/
void GetLastWeek(int32_t& lastWeek);

/*
*  调用clear_memcache.sh这个脚本的函数，该脚本和vdc_dbproxyserver_dbg处于同一个目录：不能调用，因为会使进程无法启动，
*  所以暂时还是手动清空缓存靠谱
*/
int32_t ClearMemcache();

/*
*  IP转换： 从整数转换到点分十进制格式的字符串
*/
char * IPTrans_from_int_to_string(const int32_t nIP);

/*
*  IP转换： 从整数转换到点分十进制格式的字符串(多线程安全)
*/
char * inet_ntoa_s(uint32_t ip, char* strIP);
/*
* IP转换： 从点分十进制转换到整数
*/
int32_t IPTrans_from_string_to_int(const char* szIP);

/*
*  输入一个整数，获取其字符串对应的MD5码
*/
char* GetMD5(const RoleID roleid);

/*
*  字符串拷贝
*/
void strcpy_safe(char* szDest, const int32_t max_dest_buf_len, const char* szSrc, const int32_t src_buf_len);

/*
* 从字符串中获取IP地址和丢包率
*/
void get_ip_ratio(char* ip_ratio, char ip[], int32_t& ratio);

/*
* 从字符串中获取网络类型， ip， 丢包率
*/
void get_collect_info(char* content, char szNetName[], const int32_t max_name_len, char szIP[], const int32_t max_ip_len, int32_t& nRatio);

/*
* 解析内容中的各个字段
*/
void get_content(char* content, char res[][MaxFieldLenInNetStatus],const int32_t max_field_count,  int32_t& count);

//获取ip和网络类型
void get_ip_nettype(char* content, char res[][MaxFieldLenInNetStatus],const int32_t max_field_count,  int32_t& count, char* ip, char* nettype);

//解析客户端网络状况统计内容
 void analysis_content(char* content, char res[][MaxFieldLenInNetStatus],const int32_t max_field_count,  int32_t& count, const int32_t updown, int32_t nLost[]);

FRAME_DBPROXY_NAMESPACE_END

#endif/*__DBPROXY_UTILS_H__*/

