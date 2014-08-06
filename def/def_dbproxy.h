/*******************************************************************************
*  filename:  def_dbproxy.h
*  date:      2011-12-08   21:42
*  author:    xiaohq
*  version:   x.y
*  purpose:   ����һЩdb�����������Ҫ�ĳ�����ö�����͵ȵ�
*******************************************************************************/
#ifndef DEF_DBPROXY_H
#define DEF_DBPROXY_H

#include "server_namespace.h"
#include "frame_typedef.h"
#include "common/common_list.h"
#include "common/common_def.h"


FRAME_DBPROXY_NAMESPACE_BEGIN


#define SERVER_NAME_STRING		"dbproxyserver"
#define SERVER_VERSION			"Version: 1.0.build1, Release Date: yyyy-mm-dd"

//������Ϣͷ����
typedef int32_t MsgHeadIndex;

//���ݿ������״̬
enum
{
	enmMysqlConnectionStatus_Closed = 0,
	enmMysqlConnectionStatus_Connected = 1,
};

//������Ϣʱ��Ҫ�����ݿ�������ͣ�����д, ��Щ��Ϣ������ֻ�漰��������ô�����;���0�� ����漰��д�������;���1
typedef uint8_t DBIOType;
enum
{
	enmDBOPType_Read = 0,
	enmDBOPType_Write = 1, 
};

//�������ֽ�����ʽ�����ݵİ������ֶ���ռ�õĿռ�
enum
{
	enmStreamPackageLen   = 6,  //����ʽ���ݰ��е�ǰ�����ֽڱ�ʾ�������ݰ��ĳ���
};

//����һЩ�������ö��
enum
{
	enmMaxSocketBufferLength = 4096,         //���socket�������ĳ���
	enmMaxRecvBufLen         = 4096,         //��Ľ��ջ������ĳ���
	enmMaxOptionalSize       = 64,           //optional�ֶε���󳤶�
	enmMaxSSQueueSize        = 0x40000,      //262144
	enmMaxMemcacheKeyLen     = 256,          //memcache��key����󳤶�
	enmMaxMemcacheValueLen   = 1024 * 8,  //memcache��value
	enmMaxIPAndPortLength    = 64,           //IP+�˿��ַ�������󳤶�, ����192.168.69.49:11556�������ַ���
	enmMaxMemcacheServerCount = 32,          //����ж���̨memcache������
	enmMaxDBIOThreadCount     = 8,           //���ݿ�����̵߳�������
};

//һЩ��Чֵ����
enum
{
	enmInvalidMsgHeadIndex = -1,         //��Ч����Ϣͷ����
	enmInvalidThreadIndex  = -1,         //��Ч���߳�����
	enmInvalidCommandQueueKey = -1,      //��Ч��������еļ�ֵ
	enmInvalidPort            = -1,      //��Ч�Ķ˿ں�
};


//����������
enum
{
	enmMaxAppThreadCount = 8,              //ҵ���߳�������
};

enum
{
	enmUpdateAssetType_None  = 0x01,     //��Ч���ʲ����·�ʽ
	enmUpdateAssetType_Plus  = 0x01,     //�ʲ����·�ʽ�� ����
	enmUpdateAssetType_Minus = 0x02,     //�ʲ����·�ʽ������
};

//��memcache�еĴ洢���ͣ�1Ϊ�����Ϣ��2Ϊ������Ϣ
typedef int8_t StoreType;
enum
{
	enmStoreType_RoleID = 1,
	enmStoreType_RoomID,
	enmStoreType_RoomPlayerCount, 
	enmStoreType_RoomRobotCount,
	enmStoreType_RoomChannel,       //�������ڵ�Ƶ��key
};

//enum CollectInfoKey
//{
//	CollectInfoKey_NetInfo	=	"NetInof",	  //��ʽ���㶫���ţ��Ϻ����ǿ����
//	CollectInfoKey_PackLost	=	"PackLost",   //��ʽ��60.130.43.1:9.5%��
//};

const char szCollectionKeyUpPackLost[100] = "UpPackLost";
const char szCollectionKeyDownPackLost[100] = "DownPackLost"; 
const char szProcess[100] = "Process";

typedef int8_t TransDirection;
enum 
{
	enmTransDirection_Up = 0,       //�ϴ�
	enmTransDirection_Down = 1,     //����
};

#define MSGID_MEMCACHE_TEST            0x00000000
#define MSGID_GET_USERACCOUNT_BUFFER   0x00000001
#define MD5_TEST_COMMAND               0x00000002

FRAME_DBPROXY_NAMESPACE_END

#endif /*DEF_DBPROXY_H*/
