/**
 * email:jjl_2009_hi@163.com
 * Author jijinlong
 * Data 2011.10.14~~~
 * All Rights Resvered
 ***/
#pragma once
#include "vector"
#include "deque"
#include "sys.h"
//#include "mcenter.h"
#include "singleton.h"
#include "dealcmddelegate.h"
#include "refpoint.h"
#ifdef __USE_SELECT__
#endif
class CmdObject;

template<class _T1>
inline void constuctInPlace(_T1 *ptr)
{
	new (static_cast<void*>(ptr)) _T1();
}

/**
 * 构建变长消息体
 **/
#define BUFFER_CMD(cmd,name,len) BYTE buffer##name[len]={0};\
	cmd * name = (cmd*) &buffer##name[0];constuctInPlace(name);
/**
 *构建消息 填充进buf
 **/
#define BULID_OBJECT_CMD(buf,cmd,cmdSize)\
{int __len__ = sizeof(stHeader) + (cmdSize); \
buf.resize(__len__);\
stHeader header;\
header.msgType = stHeader::OBJECT_MSG;\
header.bodySize = (cmdSize);\
bcopy(&header,&buf[0],sizeof(stHeader));\
bcopy((cmd),&buf[sizeof(stHeader)],(cmdSize));}

#define BULID_PACK_CMD(buf,cmd,cmdSize)\
{int __len__ = sizeof(stPackHeader) + (cmdSize); \
buf.resize(__len__);\
stPackHeader header;\
header.bodySize = (cmdSize);\
bcopy(&header,&buf[0],sizeof(stPackHeader));\
	bcopy((cmd),&buf[sizeof(stPackHeader)],(cmdSize));}
#pragma pack(1)
/**
 * 报文头
 */
struct stHeader{
	unsigned short bodySize;
	unsigned char msgType;
	unsigned char packType;
	stHeader()
	{
		bodySize = 0;
		msgType = PACK_MSG;
	}	
	enum{
		PACK_MSG = 0, // 对齐指令
		OBJECT_MSG = 1, // 对象序列化指令
	};
};
struct stPackHeader{
	unsigned int bodySize;
	stPackHeader()
	{
		bodySize = 0;
	}
};
#pragma pack()
/**
 * 接受指定长度的消息
 * 没达到长度 继续接受
 **/
class Socket ;
class RecvMessage{
public:
	std::vector<char>content; // 已经接受的内容
	int preSize; // 当前位移
	int messageSize; // 消息大小
public:
	RecvMessage();
	/**
	*	设置待接受消息的长度
	* \param size 消息长度
	**/
	DYNAMIC_API void setSize(DWORD size);
	bool ok; // 当前是否接受到messageSize 的消息

	/**
	* 从缓存大小为size 缓存指针为ptr 中尝试读取messageSize 的大小
	* \param ptr 当前指针 out 偏移后的指针
	* \param size 当前缓存大小 out 剩余大小
	**/
	DYNAMIC_API void append(char **ptr, int& size);
	/**
	*	清除当前缓存
	**/
	DYNAMIC_API void clear();
	/** 
	*	解码
	**/
	DYNAMIC_API void decrypt();
	/**
	* 设置密钥
	* \param akeys 密钥
	*/
	DYNAMIC_API void setKey(std::vector<char>& akeys);

	std::vector<char > keys; // 当前密钥  
	unsigned int keyPos; // 当前密钥位移
};

struct ByteContent{
	std::vector<char> content; // 待发送的内容
    DWORD offset; // 当前偏移
public:
	/** 
	 * 使用消息构造发送内容
	 * \param cmd 消息
	 * \param len 消息长度
	 * 加上头信息 构造成记录
	 **/
	DYNAMIC_API ByteContent(void *cmd,unsigned short len,char msgType)
    {
		 keyPos = 0;
		 encrypt((char*) cmd,len); // 加密内容
		 if (msgType == stHeader::OBJECT_MSG)
			BULID_OBJECT_CMD(content,cmd,len);
		 if (msgType == stHeader::PACK_MSG)
			BULID_PACK_CMD(content,cmd,len);
		 offset = 0;
    }
	DYNAMIC_API ByteContent& operator=(const ByteContent & contents)
    {
		content = contents.content;
		offset = contents.offset;
        return *this;
    }
	/**
	 * 发送当前缓存池中内容
	 * \param sock socket 
 	* \return -1 失败 1成功
	 **/
	DYNAMIC_API int send(Socket *sock);
	/**
	 * 当前缓存发送状态
	 *	\return 成功与否
	 **/
    DYNAMIC_API	bool isOk();
	/**
	 * 清除当前状态
	 **/
	DYNAMIC_API void remove(){}
	/**
	 * 加密当前内容
	 * \param content 内容
	 * \param messageSize 内容长度
	 **/
	DYNAMIC_API void encrypt(char *content,unsigned int messageSize);
	/**
	 * 设置密钥
	 * \param akeys 密钥
	 **/
	DYNAMIC_API void setKey(std::vector<char>& akeys);
	std::vector<char > keys;  // 密钥
	unsigned int keyPos; // 当前密钥偏移
};
/**
 *	网络记录
 **/
class nRecord{
public:
	RecvMessage header; // 消息头
	RecvMessage body; // 消息体
};

/**
 * 同时拥有 client 和 server 的功能 和简单的消息解析能力
 */
class ConnectionPool;
class nRecord;
class Socket{
public:
	/**
 	 * 连接管理池
 	 * */
	ConnectionPool *pool;
	/**
 	 * 是否使用本地函数
 	 * */
	bool useLocalServer;
	/**
 	 * delay 时间后 设置无效
 	 * */
	DYNAMIC_API void setInvalidDelay(DWORD delayTime);	
	DYNAMIC_API void clearValidDelay();	
	/**
 	 * 超时后设置为无效
 	 * */
	DYNAMIC_API void setInvalidTimeOut();
	/**
 	 * 创建时间
 	 * */	
	sys::stTimeValue createTime;
	/**
	 * 使用端口初始化 绑定端口 并监听
	 * \param port 端口
	 * \return 是否成功
	 **/
	DYNAMIC_API int bindAndListen(unsigned short port);
	/**
	 * 连接服务器 ip ,port 初始化
	 * \param ip 32位地址
	 * \param port 端口
	 * **/
	DYNAMIC_API int connect(const char *ip,unsigned short port);
	
	/**
	 * 	接受二进制串
	 * \param cmd 消息
	 * \param len 消息长度
	 * \return 消息的长度
	 **/
	DYNAMIC_API int recv(void *cmd,unsigned int len);
	
	/**
	 * 	发送二进制串
	 * \param cmd 消息
	 * \param len 消息长度
	 * \return 发送消息的长度
	 **/
	DYNAMIC_API int send(void *cmd,unsigned int len);
	/**
	 * 处理消息
	 * code>实现vTask::doCmd </code>
	 * \param cmd 消息
	 * \param len 消息长度
	 * \return 实际发送的长度
	 * **/
	DYNAMIC_API virtual int doCmd(DealCmdDelegate *delegate,void *cmd,unsigned int len);
	/**
	 *
	 * 发送消息 加上header
	 * \param cmd 二进制数据
     * \param len 长度
     * \return 发送的后剩余的长度	 
	 */
	DYNAMIC_API int sendPacket(void *cmd,unsigned int len);
	/**
	 *
	 * 发送消息 加上header
	 * \param 发送对齐数据
	 * \param cmd 二进制数据
     * \param len 长度
     * \return 发送的后剩余的长度	 
	 */
	DYNAMIC_API int sendCmd(void *cmd,unsigned int len);
	/**
	 * 获取socket 的句柄
	 * \return _socket 
	 */
	DYNAMIC_API sys::Socket& getHandle();
	DYNAMIC_API void setHandle(int h){_socket.setHandle(h);}
	/**
	 * 接受消息 形成记录 并转发处理
	 * \param father socket
	 * \param buf 消息缓存
	 * \param leftsize 消息缓存大小
	 * */
	DYNAMIC_API int done(DealCmdDelegate *delegate,char *buf,int leftsize);
	/**
	 *read 有效
	 * <code>实现vTask::done </code>
	 * 处理read 操作
	 * \return  0 失败 1成功
	 * **/
	DYNAMIC_API virtual int done(DealCmdDelegate *delegate);
	/**
	 * send 操作有效 处理send
	 * <code>实现vTask::send </code>
	 * \param tap 允许处理的时间
	 * \return  处理的数量
	 * */
	DYNAMIC_API virtual int send(unsigned int tap);
    	/**
	 * send 操作将数据copy进buffer 里
	 * \param buffer 缓存
	 * \param size 缓存空间
	 * */
	DYNAMIC_API int copySendBuffer(void *buffer,unsigned int size);
	/**
	 * 在需要唤醒send的时候使用
	 */
	DYNAMIC_API virtual void wakeupSend(){}
	/**
 	* 发送cmdObject
 	*/
	DYNAMIC_API virtual int sendObject(CmdObject *object);
	DYNAMIC_API Socket()
	{
		delay = 0;
//		startDelayTime = 0;
		pool = NULL;
		epfd = -1;
		valid = true;
		useLocalServer = false;
		extData = NULL;
	}
	DYNAMIC_API ~Socket()
	{
		_socket.close();
		valid = false;
	}
	/**
	* 初始成功
	*/
	DYNAMIC_API virtual void BeInit(){};
	/**
	 * 处理资源释放
	 */
	DYNAMIC_API virtual void BeReleased() {};
	/**
	 * 请求释放资源
	 */
	DYNAMIC_API void release();
#ifdef __USE_EPOLL__ 
	/**
	 * 设置epoll 监听
	 * \param epfd epoll句柄
	 * \param ptr 第3方数据
	 **/
	virtual void SETEPOLL(int epfd,void *ptr);
	/**
	 * 删除epoll 监听
	 * \param epfd epoll句柄
	 **/
	virtual void DELEPOLL(int epfd);
#endif	
    /**
     * 将自身设置无效状态
     */
	DYNAMIC_API void setInvalid(){
		if (valid)
		{
			send(10);
		}
		printf("窝巢 删除了 %s %u\n",ip.c_str(),port);
		valid = false;
		_socket.shutdown();
	}
    /**
     * 获取唯一连接符号
     * 当为  client 时 时本端的ip : port
     * 当为 server 时 为对端的 ip port
     */
	DYNAMIC_API std::string getUniqueName();
    
    /**
     * 检查当前socket 是否无效
     */
	DYNAMIC_API bool checkValid(){return valid;}
	
    /**
     * 关闭socket
     * 不可更改 socket 的值
     *
     */
    DYNAMIC_API virtual void handleClose(Socket * const socket){};
public:
	enum{
		NODE_CLIENT = 1,
		NODE_NULL = 2,
		NODE_SERVER = 3,
	};
	BYTE type; // 节点类型
	std::string ip; // ip地址
	unsigned short port; // ip 端口
	stRefMemory ref; // 如果使用内存管理 ref 指代当前内存值
	/**
	 * 制造socket
     * \param handle 当前服务器handle
 	 */
	DYNAMIC_API static Socket* makeSocket(int handle);
    unsigned char extDataType; // 第3方数据类型
    void * extData; // 第3方数据
	/**
 	 * 设置为非阻塞的
 	 * */
	DYNAMIC_API void setnonblocking();
	/**
	 * 获取连接的ip
	 */
	DYNAMIC_API std::string getIp(){return ip;}
protected:
	sys::Socket _socket;
	int epfd; //epoll 句柄
	bool valid; // 标示是否有效 
	nRecord Msg; // 消息解析器
	sys::stTimeValue startDelayTime; // 开始延时的时间
	unsigned int delay; //  延时到时 清除socket
    
	std::deque<ByteContent> contents; // 消息缓存池	
	
};

