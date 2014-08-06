/*******************************************************************************
*  filename:  def_dbproxy.h
*  date:      2011-12-08   21:42
*  author:    xiaohq
*  version:   x.y
*  purpose:   定义一些db代理服务器需要的常量、枚举类型等等
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

//定义消息头索引
typedef int32_t MsgHeadIndex;

//数据库的连接状态
enum
{
	enmMysqlConnectionStatus_Closed = 0,
	enmMysqlConnectionStatus_Connected = 1,
};

//处理消息时需要的数据库操作类型：读和写, 有些消息处理中只涉及到读，那么其类型就是0， 如果涉及到写，其类型就是1
typedef uint8_t DBIOType;
enum
{
	enmDBOPType_Read = 0,
	enmDBOPType_Write = 1, 
};

//定义六字节流格式的数据的包长度字段所占用的空间
enum
{
	enmStreamPackageLen   = 6,  //流格式数据包中的前六个字节标示整个数据包的长度
};

//定义一些最大数量枚举
enum
{
	enmMaxSocketBufferLength = 4096,         //最长的socket缓冲区的长度
	enmMaxRecvBufLen         = 4096,         //最长的接收缓冲区的长度
	enmMaxOptionalSize       = 64,           //optional字段的最大长度
	enmMaxSSQueueSize        = 0x40000,      //262144
	enmMaxMemcacheKeyLen     = 256,          //memcache中key的最大长度
	enmMaxMemcacheValueLen   = 1024 * 8,  //memcache中value
	enmMaxIPAndPortLength    = 64,           //IP+端口字符串的最大长度, 形如192.168.69.49:11556这样的字符串
	enmMaxMemcacheServerCount = 32,          //最多有多少台memcache服务器
	enmMaxDBIOThreadCount     = 8,           //数据库访问线程的最大个数
};

//一些无效值定义
enum
{
	enmInvalidMsgHeadIndex = -1,         //无效的消息头索引
	enmInvalidThreadIndex  = -1,         //无效的线程索引
	enmInvalidCommandQueueKey = -1,      //无效的命令队列的键值
	enmInvalidPort            = -1,      //无效的端口号
};


//最大个数定义
enum
{
	enmMaxAppThreadCount = 8,              //业务线程最大个数
};

enum
{
	enmUpdateAssetType_None  = 0x01,     //无效的资产更新方式
	enmUpdateAssetType_Plus  = 0x01,     //资产更新方式： 增加
	enmUpdateAssetType_Minus = 0x02,     //资产更新方式，减少
};

//在memcache中的存储类型，1为玩家信息，2为房间信息
typedef int8_t StoreType;
enum
{
	enmStoreType_RoleID = 1,
	enmStoreType_RoomID,
	enmStoreType_RoomPlayerCount, 
	enmStoreType_RoomRobotCount,
	enmStoreType_RoomChannel,       //房间所在的频道key
};

//enum CollectInfoKey
//{
//	CollectInfoKey_NetInfo	=	"NetInof",	  //格式：广东电信，上海长城宽带…
//	CollectInfoKey_PackLost	=	"PackLost",   //格式：60.130.43.1:9.5%…
//};

const char szCollectionKeyUpPackLost[100] = "UpPackLost";
const char szCollectionKeyDownPackLost[100] = "DownPackLost"; 
const char szProcess[100] = "Process";

typedef int8_t TransDirection;
enum 
{
	enmTransDirection_Up = 0,       //上传
	enmTransDirection_Down = 1,     //下载
};

#define MSGID_MEMCACHE_TEST            0x00000000
#define MSGID_GET_USERACCOUNT_BUFFER   0x00000001
#define MD5_TEST_COMMAND               0x00000002

FRAME_DBPROXY_NAMESPACE_END

#endif /*DEF_DBPROXY_H*/
