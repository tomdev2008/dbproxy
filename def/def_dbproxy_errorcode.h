/***********************************************************
* filename:  def_dbproxy_errorcode.h
* date:      2011-12-09  10:58
* author:    xiaohq
* version:   1.0
* purpose:   ����DB����������Ĵ�����
* revision:  no
***********************************************************/

#ifndef  _DEF_DBPROXY_ERRORCODE_H_
#define  _DEF_DBPROXY_ERRORCODE_H_

#include "server_namespace.h"

FRAME_DBPROXY_NAMESPACE_BEGIN

//DBProxy  servertype: 9

/*
* mysql����mysql�����������ã� �ô����������mysql�Ĺٷ��ĵ������ҵ�
*/
#define E_MYSQL_UNAVALIABLE   2006

/*
* mysql���󣺲�ѯ�����ж�ʧ����mysql�����������ӣ� �ô����������mysql�Ĺٷ��ĵ������ҵ�
*/
#define E_MYSQL_LOST_CONNECTION_WITH_MYSQLSERVER  2013


/*
* �������������׽���ʧ��
*/
#define E_OPEN_SOCKET    0x80910001   

/*
* ���������������ݿ��������������ʧ��
*/
#define E_CONNECT_TO_DBSERVER   0x80910002

/*
* ���������� ����������Ϊ��
*/
#define E_NULL_RECV_POINTER     0x80910003

/*
* ��Ҫ���յ����ݳ��Ȳ���
*/
#define E_RECVBUF_LEN           0x80910004

/*
* ��Ҫ���͵����ݳ��Ȳ���
*/
#define E_SENDBUF_LEN           0x80910005

/*
* ��DB�������ݴ���
*/
#define E_RECVDATA_FROM_DB      0x80910006

/*
* ��DB��������ʱ����������ȷ
*/
#define E_INVALID_SENDDATA_ARGUMENT  0x80910007

/*
* ��DDRecvQueue��׷������ʧ��
*/
#define E_PUSH_DDRECVQUEUE           0x80910008

/*
* ��DDRecvQueue��ȡ������ʧ��
*/
#define E_POP_DDRECVQUEUE           0x80910009

/*
* ��DDSendQueue��׷������ʧ��
*/
#define E_PUSH_DDSENDQUEUE      0x8091000A

/*
* ��DDSendQueue��ȡ������ʧ��
*/
#define E_POP_DDSENDQUEUE      0x8091000B

/*
* ����������	δ���յ���������Ϣ
*/
#define S_NOMESSAGETORECEIVE	0x00D1000C

/*
* ʹ��CDDTcpClient::SendOneMessageʱ���������Ϸ�
*/
#define E_SOM_INVALID_ARG      0x8091000D

/*
* ���������� û�����������ݿ��Է���
*/
#define S_NODATATOSEND   0x00D1000E

/*
* ������������ȡ���ݿ�IPʱ����������ȷ
*/
#define E_GTDBIP_INVALID_ARG  0x8091000F

/*
* ���������� ��DDTHREAD��initialize�����е���opensocket��������
*/
#define E_DD_INIT_OPENSOCKET  0x80910010

/*
* ��������������DBProxy��DB֮��������߳�ʧ��
*/
#define E_CREATE_CTDB_NETTHREAD  0x80910011

/*
* �����Ựʧ��
*/
#define E_CREATE_SESSION 0x80910012

/*
* ��DDSendQueue�������������ʱ�����������С����
*/
#define E_INVALID_STREAM_SIZE  0x80910013

/*
* ��DDSENDQUEUE��׷������ʱ��ddsendqueue����
*/
#define E_DD_SENDQUEUE_FULL  0x80910014

/*
* ���䱣��Ự���ݵĿռ�
*/
#define E_ALLOCATE_SESSION_DATA_SPACE  0x80910015

/*
* ������Ϣͷʧ��
*/
#define E_DBPROXY_CREATE_MSGHEAD  0x80910016

/*
* ��Ϣͷû���ҵ�
*/
#define E_MSGHEAD_NOT_FOUND  0x80910017

/*
* ��������ɾ����Ϣͷ����ʱ�� ��Ϣͷ������
*/
#define E_MSGHEAD_NOT_EXIST 0x80910018

/*
* �����ѯ��һ�����Ϣ��Ӧʱ��������ȷ
*/
#define E_PRO_GET_ROLE_BASEINFO_RESP_INVALID_ARG 0x80910019

/*
* �����ļ�ʧ��
*/
#define E_LOAD_DBCONFIG_FILE 0x8091001A

/*
* ����DBCONFIG�ļ����ڵ�ʧ��
*/
#define E_LOAD_DBCONFIG_ROOT  0x8091001B

/*
* ����dbconfig��dbserver�ڵ�ʧ��
*/
#define E_LOAD_DBCONFIG_DBSERVER 0x8091001C

/*
* ����dbserver�ڵ��ipʧ��
*/
#define E_LOAD_DBSERVER_IP  0x8091001D

/*
* ����dbserver�ڵ�Ķ˿�ʧ��
*/
#define E_LOAD_DBSERVER_PORT   0x8091001E

/*
* �Ƿ������ݿ��±�
*/
#define E_INVALID_DBSERVER_INDEX  0x8091001F

/*
* �ַ���Ӧ��Ϣʱ����������ȷ
*/
#define E_DISPATCH_CSMSG_INVALID_ARG 0x80910020

/*
* ��DDRecvQueue��push����ʱ����������ȷ
*/
#define E_PUSH_DDRECVQUEUE_INVALID_ARG  0x80910021

/*
* ������Ϣͷ������ʧ��
*/
#define E_DBPROXY_CREATE_MSGHEAD_MGT 0x80910022

/*
* ��Ϣͷ��������ʼ��ʧ��
*/
#define E_MSGHEAD_MGT_INIT 0x80910023

/*
* �ָ���������ʱ��������Ϣͷ������ʧ��
*/
#define E_RESUME_DATACENTER_NEW_MSGHEAD_MGT 0x80910024

/*
*��ȡ��Ϣͷ����ʱ����������
*/
#define E_INVALID_ARG_IN_GET_MSGHEAD_IDX 0x80910025

/*
* memcache�з���memcached_stʧ��
*/
#define E_DBPROXY_MALLOC_MEMC  0x80910026

/*
* memcache��ӻ��߸��»������ʧ��
*/
#define E_MEMCACHED_SET  0x80910027

/*
* memcache ��ȡ����ʧ��
*/
#define E_MEMCACHED_GET  0x80910028

/*
* memcache ɾ������ʧ��
*/
#define E_MEMCACHED_DELETE  0x80910029

/*
* ����memcache������ʧ��
*/
#define E_DBPROXY_MALLOC_MEMCACHE_SERVERS   0x8091002A

/*
* �����ݿ��������ѹ������ʱ��db�����߳��±겻�Ϸ�
*/
#define E_PUSHCOMMANDQUEUE_INVALIDTHREADINDEX  0x8091002B

/*
* �����ݿ��������ѹ������ʱ��������ָ����߻��������Ȳ��Ϸ�
*/
#define E_PUSHCOMMANDQUEUE_INVALID_BUFFER_OR_BUFLEN  0x8091002C

/*
* �����ݿ����������ȡ������ʱ��dbaccess�߳��±겻�Ϸ�
*/
#define E_POPCOMMANDQUEUE_INVALIDTHREADINDEX  0x8091002D

/*
* ����������׷������ʱ�����ݿ�����߳��±겻�Ϸ�
*/
#define E_PUSHRETURNQUEUE_INVALIDTHREADINDEX 0x8091002E

/*
* �ӽ��������ȡ������ʱ�����ݿ�����߳��±겻�Ϸ�
*/
#define E_POPRETURNQUEUE_INVALIDTHREADINDEX 0x8091002F

/*
* ��ʼ���������ʧ��
*/
#define E_INIT_COMMANDQUEUE  0x80910030

/*
* ��ʼ���������ʧ��
*/
#define E_INIT_RETURNQUEUE  0x80910031

/*
* �ָ��������ʧ��
*/
#define E_RESUME_COMMANDQUEUE  0x80910032

/*
* �ָ��������ʧ�� 
*/
#define E_RESUME_RETURNQUEUE  0x80910033

/*
* ע���������ʧ��
*/
#define E_UNINIT_COMMANDQUEUE  0x80910034

/*
* ע���������ʧ��
*/
#define E_UNINIT_RETURNQUEUE  0x80910035

/*
* ����������׷������ʱ��ָ����߻��������Ȳ��Ϸ�
*/
#define E_PUSH_RETURNQUEUE_INVALID_BUF_OR_BUFLEN 0x80910036

/*
* ������ϢIDʧ��
*/
#define E_DECODE_COMMANDID  0x80910037

/*
* ����Ự����ʧ��
*/
#define E_DECODE_DIALOGINDEX  0x80910038

/*
* ���������ȡ������Ϣ�������н������idʧ��
*/
#define E_DECODE_ROLEID  0x80910039

/*
* �����ݿ���ɾ����ɫʧ��
*/
#define E_DELETEROLE_FROM_DB 0x8091003A

/*
* �������ݿ�����߳�ʧ��
*/
#define E_DBPROXY_CREATEDBACCESSTHREAD 0x8091003B

/*
* ���ݿ�����̳߳�ʼ��ʱ �±겻�Ϸ�
*/
#define E_DBACCESSTHREAD_INIT_INVALID_THREADINDEX 0x8091003C

/*
* ����memcache��û���ҵ���Ҳ������memcache�в����������¼
*/
#define E_MEMCACHE_KEY_NOTFOUND  0x8091003D


/*
* �����û���ʧ��
*/
#define E_LOAD_DBSERVER_USER  0x8091003E

/*
*��������ʧ��
*/
#define E_LOAD_DBPASSWORD 0x8091003F

/*
*�������ݿ�����ʧ��
*/
#define E_LOAD_DBNAME  0x80910040

/*
* ���������ļ�keyconfig.xmlʧ��
*/
#define E_LOAD_KEY_FILE 0x80910041

/*
* ����keyconfig.xml���ڵ�ʧ��
*/
#define E_LOAD_KEYCONFIG_ROOT 0x80910042

/*
* ����keyconfig.xml key�ڵ�ʧ��
*/
#define E_LOAD_KEYCONFIG_KEY 0x80910043

/*
* ����������м�ֵʧ��
*/
#define E_LOAD_KEY_COMMANDQUEUEKEY 0x80910044

/*
* ��ʼ��ʱ���������ڴ�ʧ��
*/
#define E_INIT_CREATE_SHAREMEMORY 0x80910045

/*
* ����memcacheconfig.xmlʧ��
*/
#define E_LOAD_MEMCACHE_FILE 0x80910046

/*
* ����memcache���ڵ�ʧ��
*/
#define E_LOAD_MEMCACHE_ROOT 0x80910047

/*
* ���ؽڵ�memcacheʧ��
*/
#define E_LOAD_MEMCACHECONFIG_MEMCACHE 0x80910048

/*
* ����memcache��IPʧ��
*/
#define E_LOAD_MEMCACHE_IP 0x80910049

/*
* ����memcache�Ķ˿�ʧ��
*/
#define E_LOAD_MEMCACHE_PORT 0x8091004A

/*
* ������Ʒʱ����
*/
#define E_INSUFF_BALANCE 0x8091004B

/*
* ���������Ȳ���ȷ
*/
#define E_WRONG_BUFFLEN 0x8091004C

/*
* ��ѯ����ʲ�ʧ��
*/
#define E_QUERY_ROLE_ASSET  0x8091004D

/*
* ���ݿ���û����Ӧ�ļ�¼
*/
#define E_NORECORD_IN_DB  0x8091004E

/*
* ��Ч��money��������С��0��
*/
#define E_INVALID_MONEY_AMOUNT   0x8091004F

/*
* ���memcache�е���������ʧ��
*/
#define E_MEMCACHE_FLUSH     0x80910050

/*
* ���ݿ�ĸ����������������
*/
#define E_MORE_THAN_MAX_DBSERVER_COUNT 0x80910051

/*
* ��ѯ��ҳ�ֵ��¼ʧ��
*/
#define E_QUERY_ROLE_RECHARGE  0x80910052

/*
* ��ѯ������Ѽ�¼ʧ��
*/
#define E_QUERY_ROLE_CONSUME  0x80910053

/*
* ��ѯ��һ񽱼�¼ʧ��
*/
#define E_QUERY_ROLE_PRIZE   0x80910054

/*
* ��ѯ��ҵ��߹����¼�Ĺ���IDʧ��
*/
#define E_QUERY_PURCHASEID  0x80910055

/*
* ��ѯ�û�������Ϣʧ��
*/
#define E_QUERY_ROLE_BASE_INFO  0x80910056

/*
* �޸ĵ��ַ����ĸ��������ܵ��޸���Ŀ��
*/
#define E_STRING_COUNT_LARGERER_THAN_SET_COUNT  0x80910057

/*
* ʵ�ʵõ����ַ����ĸ����������еĸ�����һ��
*/
#define E_STRING_COUNT_NOT_EQUAL  0x80910058

/*
* ��¼�����ݿ��в����ڣ���ѯ���Ӱ������Ϊ0��
*/
#define E_NOT_EXIST_IN_DB  0x80910059

/*
* �����ݿ����������push��Ϣʧ��
*/
#define E_PUSH_DD_COMMANDQUEUE  0x8091005A

/*
* �����½�����ȡ�����Ϣʧ��
*/
#define E_PRO_ROOM_GET_ROLEINFO 0x8091005B

/*
* ���Ϸ��ķ�������roleid
*/
#define E_INVALID_ROOM_HOST_ROLEID 0x8091005C

/*��ɫ�����ݿ��в�����*/
#define E_ROLE_NOT_EXIST_IN_DB   0x8091005D

/*���������ݿ��в�����*/
#define E_ROOM_NOT_EXIST_IN_DB  0x8091005E

/*
* ������Ӧʧ��
*/
#define E_POST_RESPONSE  0x8091005F

/*
* ���������û��ʲ�ʱ��һ���˶�û�и��³ɹ�
*/
#define E_BATCH_UPDATE_ROLE_ASSET  0x80910060

/*
* �����û���Ʒ��Ϣʧ��
*/
#define E_UPDATE_USER_ITEMINFO 0x80910061

/*
* ��ѯ�û��ĵ�����Ϣ��ʱ��һ����û�в鵽
*/
#define E_ITEM_GET_USERITEMINFO  0x80910062

/*
* ע��179�˺�ʱ����accountserver�õ�����Ӧ�е�roleid����179id���ԣ�С�ڻ��ߵ���0��
*/
#define E_INVALID_179UIN_OR_179ID  0x80910063

/*
* ���û��˺ű��в����¼ʱ��Ӱ�����������1��
*/
#define E_INSERT_USER_ACCOUNT_AFFECTED_ROWS 0x80910064

/*
* ��ѯ�����ĳ��ʱ��ε��ܵĳ�ֵ����ʧ�ܣ�һ����û��ѯ�ɹ���
*/
#define E_QUERY_USER_TOTAL_RECHARGE_TIMESPAN 0x80910065

/*
* ����������һ����û�в����ɹ�
*/
#define E_GROUP_DEGRADE_VIPLEVEL 0x80910066

/*
* ��ѯ�û���179uinʧ��
*/
#define E_QUERY_ROLE_179UIN 0x80910067

/*
*  ��ѯ�����¼��Ϊ��
*/
#define E_RECORDSET_NULL  0x80910068

/*
*  �����¼��ת��������֮����С��0
*/
#define E_RECORDSET_BELOW_ZERO   0x80910069

/*
* ׼���������ָ���ҵ����Ѿ��������ˣ�����һ�����������0����
*/
#define S_ALREADY_DEGRADE       0x4091006A

/*
* ��������ǳ�ʧ�ܣ���Ϊ���ּ��β����ݿ�֮��
* �����ɫ������Ϣʧ�ܣ���ˣ����漰�����ֵ��ֶεķ���ֵ�����г�
* ���������
*/
#define E_ENCODE_ROLE_NAME      0x8091006B

/*
* ������Ҽ�������ʡ��ʧ��
*/
#define E_ENCODE_ROLE_HOME_PROVINCE  0x8091006C

/*
* �������������ʧ��
*/
#define E_ENCODE_ROLE_HOME_CITY      0x8091006D

/*
* ������Ҽ���������ʧ��
*/
#define E_ENCODE_ROLE_HOME_COUNTY    0x8091006F

/*
* �������Ŀǰ����ʡ��ʧ��
*/
#define E_ENCODE_ROLE_CUR_PROVINCE  0x80910070

/*
* �������Ŀǰ������ʧ��
*/
#define E_ENCODE_CUR_HOME_CITY      0x80910071

/*
* �������Ŀǰ������ʧ��
*/
#define E_ENCODE_CUR_HOME_COUNTY    0x80910072

/*
* ����������һ�ε�¼�ص�ʧ��
*/
#define E_ENCODE_LAST_LOGIN_ADDR    0x80910073

/*
* �����������ʧ��
*/
#define E_ENCODE_HOROSCOPE          0x80910074

/*
* ����memcache���Ӽ���߳�ʧ��
*/
#define E_DBPROXY_CREATE_MEMCACHE_CONN_CHECK_THREAD  0x80910075

/*
*  systemϵͳ����clear_memcache.sh�ű�ʧ��
*/
#define E_CALL_CLEAR_MEMCACHE_SH   0x80910076

/*
* ִ��update���ʱ��Ӱ�������Ϊ0��Ҳ����û�дﵽ���µ�Ч����ʵ����û���޸����ݿ⣩
*/
#define E_UPDATE_ZERO_ROW_AFFETCTED  0x80910077

/*
* ����Ҽ�Ǯʱ���������ϵͳ�涨����ҵ�179�ҵ����������40�ڣ�
*/
#define E_OVERRUN_MAX_PLAYER_MONEY 0x80910078

/*
*  ����ҿ�Ǯʱ������
*/
#define E_LEFTMONEY_NOT_ENOUGH   0x80910079

/*
* �û������ݿ��в�����
*/
#define E_PLAYER_NOT_EXIST_IN_DB  0x8091007A

/*
* dbconfig.xml�е�master�ڵ�Ϊ��
*/
#define E_TAG_MASTER_NULL  0x8091007B

/*
* slave �ڵ�Ϊ��
*/
#define E_TAG_SLAVE_NULL  0x8091007C

/*
*  �ӿ�ڵ�Ϊ��
*/
#define E_TAG_RDDBSERVER_NULL  0x8091007D

/*
*  ���ݿ���̵߳��±겻�Ϸ�
*/
#define E_READ_DBACCESSTHREAD_INIT_INVALID_THREADINDEX  0x8091007E

/*
*  �������ݿ��ȡ�߳�ʧ��
*/
#define E_DBPROXY_CREATE_READ_DBACCESSTHREAD  0x8091007F

/*
* ���ݿ��ȡ����keyΪ��
*/
#define E_READ_CMD_QUEUE_KEY_NULL  0x80910080

/*
* �������ݿ��ȡ���й����ڴ�ʧ��
*/
#define E_CREATE_READ_DBCMD_QUEUE_SHAREMEMORY  0x80910081

/*
* ��ʼ�����ݿ��ȡ����ʧ��
*/
#define E_INIT_COMMANDQUEUE_READ  0x80910082

/*
* �ָ����ݿ��ȡ����ʧ��
*/
#define E_RESUME_COMMANDQUEUE_READ 0x80910083

/*
* �������ݿ��ȡ����ʧ��
*/
#define E_UNINIT_COMMANDQUEUE_READ  0x80910084

/*
* �����ݿ��ȡ������������ݰ�ʱ�� �±겻�Ϸ�
*/
#define E_PUSH_READ_COMMANDQUEUE_INVALIDTHREADINDEX 0x80910085

/*
* �����ݿ��ȡ�������������ʱ��buffer����bufferlen���Ϸ�
*/
#define E_PUSH_READ_COMMANDQUEUE_INVALID_BUFFER_OR_BUFLEN  0x80910086

/*
* �����ݿ��ȡ�����ж�ȡ����ʱ�� �±겻�Ϸ�
*/
#define E_POP_READ_COMMANDQUEUE_INVALIDTHREADINDEX 0x80910087

/*
*  ���ݿ�������û�дӿ�
*/
#define E_NO_SLAVE_DB 0x80910088

/*
* ��ȷ�Ĳ�ѯ���Ϊ0
*/
#define E_ZERO_COUNT  0x80910089

/*
* key��memcache�в�����
*/
#define E_NOT_EXIST_IN_MEMCACHE  0x8091008A

/*
* �û����ʲ��������������ܳ��ܵ����ֵ�� 40��
*/
#define E_MAX_MONEY_AMOUNT  0x8091008B

/*
* DB��Ϣ����Ϣ������MAP�в�����
*/
#define E_DBMSG_HANDLER_NOT_EXIST 0x8091008C

/*
* ��ȡ��Ϣ������ʱ�� ��Ϣ������Ϊ�ա�û��ע�������Ϣ��������
*/
#define E_MSGHANDLER_NULL  0x8091008D

/*
* roombaseinfo�����channelid��ֵ�������������ܱ�ʾ����󳤶�
*/
#define E_MAX_CHANNELID 0x8091008E


/*
* ��ȡ��Ȼ������ʧ��
*/
#define E_GET_DATEWEEK 0x8091008F

FRAME_DBPROXY_NAMESPACE_END

#endif

