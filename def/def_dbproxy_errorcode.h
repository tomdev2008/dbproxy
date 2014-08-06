/***********************************************************
* filename:  def_dbproxy_errorcode.h
* date:      2011-12-09  10:58
* author:    xiaohq
* version:   1.0
* purpose:   定义DB代理服务器的错误码
* revision:  no
***********************************************************/

#ifndef  _DEF_DBPROXY_ERRORCODE_H_
#define  _DEF_DBPROXY_ERRORCODE_H_

#include "server_namespace.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

//DBProxy  servertype: 9

/*
* mysql错误：mysql服务器不可用， 该错误码可以在mysql的官方文档里面找到
*/
#define E_MYSQL_UNAVALIABLE   2006

/*
* mysql错误：查询过程中丢失了与mysql服务器的连接， 该错误码可以在mysql的官方文档里面找到
*/
#define E_MYSQL_LOST_CONNECTION_WITH_MYSQLSERVER  2013


/*
* 错误描述：打开套接字失败
*/
#define E_OPEN_SOCKET    0x80910001   

/*
* 错误描述：与数据库服务器建立连接失败
*/
#define E_CONNECT_TO_DBSERVER   0x80910002

/*
* 错误描述： 结束缓冲区为空
*/
#define E_NULL_RECV_POINTER     0x80910003

/*
* 想要接收的数据长度不对
*/
#define E_RECVBUF_LEN           0x80910004

/*
* 想要发送的数据长度不对
*/
#define E_SENDBUF_LEN           0x80910005

/*
* 从DB接收数据错误
*/
#define E_RECVDATA_FROM_DB      0x80910006

/*
* 向DB发送数据时，参数不正确
*/
#define E_INVALID_SENDDATA_ARGUMENT  0x80910007

/*
* 从DDRecvQueue中追加数据失败
*/
#define E_PUSH_DDRECVQUEUE           0x80910008

/*
* 从DDRecvQueue中取出数据失败
*/
#define E_POP_DDRECVQUEUE           0x80910009

/*
* 从DDSendQueue中追加数据失败
*/
#define E_PUSH_DDSENDQUEUE      0x8091000A

/*
* 从DDSendQueue中取出数据失败
*/
#define E_POP_DDSENDQUEUE      0x8091000B

/*
* 错误描述：	未接收到完整的消息
*/
#define S_NOMESSAGETORECEIVE	0x00D1000C

/*
* 使用CDDTcpClient::SendOneMessage时，参数不合法
*/
#define E_SOM_INVALID_ARG      0x8091000D

/*
* 错误描述： 没有完整的数据可以发送
*/
#define S_NODATATOSEND   0x00D1000E

/*
* 错误描述：获取数据库IP时，参数不正确
*/
#define E_GTDBIP_INVALID_ARG  0x8091000F

/*
* 错误描述： 在DDTHREAD的initialize方法中调用opensocket方法错误
*/
#define E_DD_INIT_OPENSOCKET  0x80910010

/*
* 错误描述：创建DBProxy与DB之间的网络线程失败
*/
#define E_CREATE_CTDB_NETTHREAD  0x80910011

/*
* 创建会话失败
*/
#define E_CREATE_SESSION 0x80910012

/*
* 往DDSendQueue队列中添加数据时，流的体积大小不对
*/
#define E_INVALID_STREAM_SIZE  0x80910013

/*
* 往DDSENDQUEUE中追加数据时，ddsendqueue已满
*/
#define E_DD_SENDQUEUE_FULL  0x80910014

/*
* 分配保存会话数据的空间
*/
#define E_ALLOCATE_SESSION_DATA_SPACE  0x80910015

/*
* 创建消息头失败
*/
#define E_DBPROXY_CREATE_MSGHEAD  0x80910016

/*
* 消息头没有找到
*/
#define E_MSGHEAD_NOT_FOUND  0x80910017

/*
* 根据索引删除消息头对象时， 消息头不存在
*/
#define E_MSGHEAD_NOT_EXIST 0x80910018

/*
* 处理查询玩家基本信息响应时参数不正确
*/
#define E_PRO_GET_ROLE_BASEINFO_RESP_INVALID_ARG 0x80910019

/*
* 加载文件失败
*/
#define E_LOAD_DBCONFIG_FILE 0x8091001A

/*
* 加载DBCONFIG文件根节点失败
*/
#define E_LOAD_DBCONFIG_ROOT  0x8091001B

/*
* 加载dbconfig的dbserver节点失败
*/
#define E_LOAD_DBCONFIG_DBSERVER 0x8091001C

/*
* 加载dbserver节点的ip失败
*/
#define E_LOAD_DBSERVER_IP  0x8091001D

/*
* 加载dbserver节点的端口失败
*/
#define E_LOAD_DBSERVER_PORT   0x8091001E

/*
* 非法的数据库下标
*/
#define E_INVALID_DBSERVER_INDEX  0x8091001F

/*
* 分发响应消息时，参数不正确
*/
#define E_DISPATCH_CSMSG_INVALID_ARG 0x80910020

/*
* 向DDRecvQueue中push数据时，参数不正确
*/
#define E_PUSH_DDRECVQUEUE_INVALID_ARG  0x80910021

/*
* 创建消息头管理器失败
*/
#define E_DBPROXY_CREATE_MSGHEAD_MGT 0x80910022

/*
* 消息头管理器初始化失败
*/
#define E_MSGHEAD_MGT_INIT 0x80910023

/*
* 恢复数据中心时，创建消息头管理器失败
*/
#define E_RESUME_DATACENTER_NEW_MSGHEAD_MGT 0x80910024

/*
*获取消息头索引时，参数不对
*/
#define E_INVALID_ARG_IN_GET_MSGHEAD_IDX 0x80910025

/*
* memcache中分配memcached_st失败
*/
#define E_DBPROXY_MALLOC_MEMC  0x80910026

/*
* memcache添加或者更新缓存对象失败
*/
#define E_MEMCACHED_SET  0x80910027

/*
* memcache 获取对象失败
*/
#define E_MEMCACHED_GET  0x80910028

/*
* memcache 删除对象失败
*/
#define E_MEMCACHED_DELETE  0x80910029

/*
* 创建memcache服务器失败
*/
#define E_DBPROXY_MALLOC_MEMCACHE_SERVERS   0x8091002A

/*
* 往数据库命令队列压入数据时，db访问线程下标不合法
*/
#define E_PUSHCOMMANDQUEUE_INVALIDTHREADINDEX  0x8091002B

/*
* 往数据库命令队列压入数据时，缓冲区指针或者缓冲区长度不合法
*/
#define E_PUSHCOMMANDQUEUE_INVALID_BUFFER_OR_BUFLEN  0x8091002C

/*
* 从数据库命令队列中取出数据时，dbaccess线程下标不合法
*/
#define E_POPCOMMANDQUEUE_INVALIDTHREADINDEX  0x8091002D

/*
* 向结果队列中追加数据时，数据库访问线程下标不合法
*/
#define E_PUSHRETURNQUEUE_INVALIDTHREADINDEX 0x8091002E

/*
* 从结果队列中取出数据时，数据库访问线程下标不合法
*/
#define E_POPRETURNQUEUE_INVALIDTHREADINDEX 0x8091002F

/*
* 初始化命令队列失败
*/
#define E_INIT_COMMANDQUEUE  0x80910030

/*
* 初始化结果队列失败
*/
#define E_INIT_RETURNQUEUE  0x80910031

/*
* 恢复命令队列失败
*/
#define E_RESUME_COMMANDQUEUE  0x80910032

/*
* 恢复结果队列失败 
*/
#define E_RESUME_RETURNQUEUE  0x80910033

/*
* 注销命令队列失败
*/
#define E_UNINIT_COMMANDQUEUE  0x80910034

/*
* 注销结果队列失败
*/
#define E_UNINIT_RETURNQUEUE  0x80910035

/*
* 向结果队列中追加数据时，指针或者缓冲区长度不合法
*/
#define E_PUSH_RETURNQUEUE_INVALID_BUF_OR_BUFLEN 0x80910036

/*
* 解码消息ID失败
*/
#define E_DECODE_COMMANDID  0x80910037

/*
* 解码会话索引失败
*/
#define E_DECODE_DIALOGINDEX  0x80910038

/*
* 从命令队列取出的消息缓冲区中解码玩家id失败
*/
#define E_DECODE_ROLEID  0x80910039

/*
* 从数据库中删除角色失败
*/
#define E_DELETEROLE_FROM_DB 0x8091003A

/*
* 创建数据库访问线程失败
*/
#define E_DBPROXY_CREATEDBACCESSTHREAD 0x8091003B

/*
* 数据库访问线程初始化时 下标不合法
*/
#define E_DBACCESSTHREAD_INIT_INVALID_THREADINDEX 0x8091003C

/*
* 键在memcache中没有找到，也就是在memcache中不存在这个记录
*/
#define E_MEMCACHE_KEY_NOTFOUND  0x8091003D


/*
* 加载用户名失败
*/
#define E_LOAD_DBSERVER_USER  0x8091003E

/*
*加载密码失败
*/
#define E_LOAD_DBPASSWORD 0x8091003F

/*
*加载数据库名称失败
*/
#define E_LOAD_DBNAME  0x80910040

/*
* 加载配置文件keyconfig.xml失败
*/
#define E_LOAD_KEY_FILE 0x80910041

/*
* 加载keyconfig.xml根节点失败
*/
#define E_LOAD_KEYCONFIG_ROOT 0x80910042

/*
* 加载keyconfig.xml key节点失败
*/
#define E_LOAD_KEYCONFIG_KEY 0x80910043

/*
* 加载命令队列键值失败
*/
#define E_LOAD_KEY_COMMANDQUEUEKEY 0x80910044

/*
* 初始化时创建共享内存失败
*/
#define E_INIT_CREATE_SHAREMEMORY 0x80910045

/*
* 加载memcacheconfig.xml失败
*/
#define E_LOAD_MEMCACHE_FILE 0x80910046

/*
* 加载memcache根节点失败
*/
#define E_LOAD_MEMCACHE_ROOT 0x80910047

/*
* 加载节点memcache失败
*/
#define E_LOAD_MEMCACHECONFIG_MEMCACHE 0x80910048

/*
* 加载memcache的IP失败
*/
#define E_LOAD_MEMCACHE_IP 0x80910049

/*
* 加载memcache的端口失败
*/
#define E_LOAD_MEMCACHE_PORT 0x8091004A

/*
* 购买物品时余额不足
*/
#define E_INSUFF_BALANCE 0x8091004B

/*
* 缓冲区长度不正确
*/
#define E_WRONG_BUFFLEN 0x8091004C

/*
* 查询玩家资产失败
*/
#define E_QUERY_ROLE_ASSET  0x8091004D

/*
* 数据库中没有相应的记录
*/
#define E_NORECORD_IN_DB  0x8091004E

/*
* 无效的money的数量（小于0）
*/
#define E_INVALID_MONEY_AMOUNT   0x8091004F

/*
* 清空memcache中的所有内容失败
*/
#define E_MEMCACHE_FLUSH     0x80910050

/*
* 数据库的个数超出了最大限制
*/
#define E_MORE_THAN_MAX_DBSERVER_COUNT 0x80910051

/*
* 查询玩家充值记录失败
*/
#define E_QUERY_ROLE_RECHARGE  0x80910052

/*
* 查询玩家消费记录失败
*/
#define E_QUERY_ROLE_CONSUME  0x80910053

/*
* 查询玩家获奖记录失败
*/
#define E_QUERY_ROLE_PRIZE   0x80910054

/*
* 查询玩家道具购买记录的购买ID失败
*/
#define E_QUERY_PURCHASEID  0x80910055

/*
* 查询用户基本信息失败
*/
#define E_QUERY_ROLE_BASE_INFO  0x80910056

/*
* 修改的字符串的个数大于总的修改项目数
*/
#define E_STRING_COUNT_LARGERER_THAN_SET_COUNT  0x80910057

/*
* 实际得到的字符串的个数和请求中的个数不一致
*/
#define E_STRING_COUNT_NOT_EQUAL  0x80910058

/*
* 记录在数据库中不存在（查询结果影响行数为0）
*/
#define E_NOT_EXIST_IN_DB  0x80910059

/*
* 向数据库命令队列中push消息失败
*/
#define E_PUSH_DD_COMMANDQUEUE  0x8091005A

/*
* 处理登陆房间获取玩家信息失败
*/
#define E_PRO_ROOM_GET_ROLEINFO 0x8091005B

/*
* 不合法的房间室主roleid
*/
#define E_INVALID_ROOM_HOST_ROLEID 0x8091005C

/*角色在数据库中不存在*/
#define E_ROLE_NOT_EXIST_IN_DB   0x8091005D

/*房间在数据库中不存在*/
#define E_ROOM_NOT_EXIST_IN_DB  0x8091005E

/*
* 发送响应失败
*/
#define E_POST_RESPONSE  0x8091005F

/*
* 批量更新用户资产时，一个人都没有更新成功
*/
#define E_BATCH_UPDATE_ROLE_ASSET  0x80910060

/*
* 更新用户物品信息失败
*/
#define E_UPDATE_USER_ITEMINFO 0x80910061

/*
* 查询用户的道具信息表时，一个都没有查到
*/
#define E_ITEM_GET_USERITEMINFO  0x80910062

/*
* 注册179账号时，从accountserver得到的响应中的roleid或者179id不对（小于或者等于0）
*/
#define E_INVALID_179UIN_OR_179ID  0x80910063

/*
* 向用户账号表中插入记录时，影响的行数不是1行
*/
#define E_INSERT_USER_ACCOUNT_AFFECTED_ROWS 0x80910064

/*
* 查询玩家在某个时间段的总的充值数量失败（一个都没查询成功）
*/
#define E_QUERY_USER_TOTAL_RECHARGE_TIMESPAN 0x80910065

/*
* 批量降级，一个都没有操作成功
*/
#define E_GROUP_DEGRADE_VIPLEVEL 0x80910066

/*
* 查询用户的179uin失败
*/
#define E_QUERY_ROLE_179UIN 0x80910067

/*
*  查询结果记录集为空
*/
#define E_RECORDSET_NULL  0x80910068

/*
*  结果记录集转换成整数之后结果小于0
*/
#define E_RECORDSET_BELOW_ZERO   0x80910069

/*
* 准备降级发现该玩家当月已经降级过了，返回一个错误码大于0的数
*/
#define S_ALREADY_DEGRADE       0x4091006A

/*
* 编码玩家昵称失败：因为出现几次查数据库之后，
* 编码角色基本信息失败，因此，将涉及到汉字的字段的返回值单独列出
* 方便检查错误
*/
#define E_ENCODE_ROLE_NAME      0x8091006B

/*
* 编码玩家家乡所在省份失败
*/
#define E_ENCODE_ROLE_HOME_PROVINCE  0x8091006C

/*
* 编码玩家所在市失败
*/
#define E_ENCODE_ROLE_HOME_CITY      0x8091006D

/*
* 编码玩家家乡所在县失败
*/
#define E_ENCODE_ROLE_HOME_COUNTY    0x8091006F

/*
* 编码玩家目前所在省份失败
*/
#define E_ENCODE_ROLE_CUR_PROVINCE  0x80910070

/*
* 编码玩家目前所在市失败
*/
#define E_ENCODE_CUR_HOME_CITY      0x80910071

/*
* 编码玩家目前所在县失败
*/
#define E_ENCODE_CUR_HOME_COUNTY    0x80910072

/*
* 编码玩家最后一次登录地点失败
*/
#define E_ENCODE_LAST_LOGIN_ADDR    0x80910073

/*
* 编码玩家星座失败
*/
#define E_ENCODE_HOROSCOPE          0x80910074

/*
* 创建memcache连接检测线程失败
*/
#define E_DBPROXY_CREATE_MEMCACHE_CONN_CHECK_THREAD  0x80910075

/*
*  system系统调用clear_memcache.sh脚本失败
*/
#define E_CALL_CLEAR_MEMCACHE_SH   0x80910076

/*
* 执行update语句时，影响的行数为0（也就是没有达到更新的效果，实际上没有修改数据库）
*/
#define E_UPDATE_ZERO_ROW_AFFETCTED  0x80910077

/*
* 给玩家加钱时，结果大于系统规定的玩家的179币的最大数量（40亿）
*/
#define E_OVERRUN_MAX_PLAYER_MONEY 0x80910078

/*
*  给玩家扣钱时，余额不足
*/
#define E_LEFTMONEY_NOT_ENOUGH   0x80910079

/*
* 用户在数据库中不存在
*/
#define E_PLAYER_NOT_EXIST_IN_DB  0x8091007A

/*
* dbconfig.xml中的master节点为空
*/
#define E_TAG_MASTER_NULL  0x8091007B

/*
* slave 节点为空
*/
#define E_TAG_SLAVE_NULL  0x8091007C

/*
*  从库节点为空
*/
#define E_TAG_RDDBSERVER_NULL  0x8091007D

/*
*  数据库读线程的下标不合法
*/
#define E_READ_DBACCESSTHREAD_INIT_INVALID_THREADINDEX  0x8091007E

/*
*  创建数据库读取线程失败
*/
#define E_DBPROXY_CREATE_READ_DBACCESSTHREAD  0x8091007F

/*
* 数据库读取队列key为空
*/
#define E_READ_CMD_QUEUE_KEY_NULL  0x80910080

/*
* 创建数据库读取队列共享内存失败
*/
#define E_CREATE_READ_DBCMD_QUEUE_SHAREMEMORY  0x80910081

/*
* 初始化数据库读取队列失败
*/
#define E_INIT_COMMANDQUEUE_READ  0x80910082

/*
* 恢复数据库读取队列失败
*/
#define E_RESUME_COMMANDQUEUE_READ 0x80910083

/*
* 销毁数据库读取队列失败
*/
#define E_UNINIT_COMMANDQUEUE_READ  0x80910084

/*
* 向数据库读取队列中添加数据包时， 下标不合法
*/
#define E_PUSH_READ_COMMANDQUEUE_INVALIDTHREADINDEX 0x80910085

/*
* 向数据库读取队列中添加数据时，buffer或者bufferlen不合法
*/
#define E_PUSH_READ_COMMANDQUEUE_INVALID_BUFFER_OR_BUFLEN  0x80910086

/*
* 从数据库读取队列中读取数据时， 下标不合法
*/
#define E_POP_READ_COMMANDQUEUE_INVALIDTHREADINDEX 0x80910087

/*
*  数据库配置中没有从库
*/
#define E_NO_SLAVE_DB 0x80910088

/*
* 正确的查询结果为0
*/
#define E_ZERO_COUNT  0x80910089

/*
* key在memcache中不存在
*/
#define E_NOT_EXIST_IN_MEMCACHE  0x8091008A

/*
* 用户的资产超过服务器所能承受的最大值： 40亿
*/
#define E_MAX_MONEY_AMOUNT  0x8091008B

/*
* DB消息在消息处理器MAP中不存在
*/
#define E_DBMSG_HANDLER_NOT_EXIST 0x8091008C

/*
* 获取消息处理器时， 消息处理器为空【没有注册这个消息处理器】
*/
#define E_MSGHANDLER_NULL  0x8091008D

/*
* roombaseinfo里面的channelid的值大于其类型所能表示的最大长度
*/
#define E_MAX_CHANNELID 0x8091008E


/*
* 获取自然周日期失败
*/
#define E_GET_DATEWEEK 0x8091008F

FRAME_DBPROXY_NAMESPACE_END

#endif

