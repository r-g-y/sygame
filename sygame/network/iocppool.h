#pragma once
#include "sys.h"
#include "socket.h"
#include "singleton.h"
#include "net.h"
#ifdef __WINDOWS__
// winsock 2 的头文件和库

// 缓冲区长度 (1024*8)
// 之所以为什么设置8K，也是一个江湖上的经验值
// 如果确实客户端发来的每组数据都比较少，那么就设置得小一些，省内存
#define MAX_BUFFER_LEN        8192  
// 默认端口
#define DEFAULT_PORT          12345    
// 默认IP地址
#define DEFAULT_IP            ("127.0.0.1")


//////////////////////////////////////////////////////////////////
// 在完成端口上投递的I/O操作的类型
typedef enum _OPERATION_TYPE  
{  
	ACCEPT_POSTED,                     // 标志投递的Accept操作
	SEND_POSTED,                       // 标志投递的是发送操作
	RECV_POSTED,                       // 标志投递的是接收操作
	NULL_POSTED                        // 用于初始化，无意义

}OPERATION_TYPE;

//====================================================================================
//
//				单IO数据结构体定义(用于每一个重叠操作的参数)
//
//====================================================================================

class IoContent
{
public:
	OVERLAPPED     overlapped;                               // 每一个重叠网络操作的重叠结构(针对每一个Socket的每一个操作，都要有一个)              
	union{
	Socket	       *socket;                               // 这个网络操作所使用的Socket
	SOCKET sockfd; // 连接句柄
	};
	WSABUF         m_wsaBuf;                                   // WSA类型的缓冲区，用于给重叠操作传参数的
	char           m_szBuffer[MAX_BUFFER_LEN];                 // 这个是WSABUF里具体存字符的缓冲区
	OPERATION_TYPE optype;                                   // 标识网络操作的类型(对应上面的枚举)
	DWORD			msgLen;
	// 初始化
	IoContent()
	{
		bzero(&overlapped, sizeof(overlapped));  
		bzero( m_szBuffer,MAX_BUFFER_LEN );
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = MAX_BUFFER_LEN;
		optype     = NULL_POSTED;
		socket = NULL;
		msgLen = 0;
	}
	// 释放掉Socket
	~IoContent()
	{
	//	socket->getHandle().close();
	}
	// 重置缓冲区内容
	void ResetBuffer()
	{
		bzero( m_szBuffer,MAX_BUFFER_LEN );
	}

};


//====================================================================================
//
//				单句柄数据结构体定义(用于每一个完成端口，也就是每一个Socket的参数)
//
//====================================================================================

class SocketContent:public Socket
{  
public:
	//Socket      		*socket;                                  // 每一个客户端连接的Socket
	SOCKADDR_IN		m_ClientAddr;                              // 客户端的地址
	std::vector<IoContent*> ioContents;             // 客户端网络操作的上下文数据，
	                                                       // 也就是说对于每一个客户端Socket，是可以在上面同时投递多个IO请求的
	//IoContent acceptIo;
	IoContent sendIo;
	IoContent recvIo;
	// 初始化
	DYNAMIC_API SocketContent()
	{
		memset(&m_ClientAddr, 0, sizeof(m_ClientAddr)); 
		createTime = sys::sTime::getNowTime();
	//	socket = NULL;
	}

	// 释放资源
	DYNAMIC_API ~SocketContent()
	{
		getHandle().close();
		// 释放掉所有的IO上下文数据
		for(unsigned int i=0;i < ioContents.size();i++ )
		{
			delete ioContents[i];
		}
		ioContents.clear();
	}

	// 获取一个新的IoContext
	DYNAMIC_API IoContent* GetNewIoContext()
	{
		IoContent* p = new IoContent;
		ioContents.push_back(p);
		return p;
	}

	// 从数组中移除一个指定的IoContext
	DYNAMIC_API void RemoveContext( IoContent* pContext )
	{
		for(unsigned int i=0;i< ioContents.size();i++ )
		{
			if( pContext==ioContents[0])
			{
				delete pContext;
				pContext = NULL;
				ioContents[0] = NULL;				
				break;
			}
		}
	}
	DYNAMIC_API void wakeupSend();
};
 
class IocpWorkThread;
class CheckValidThread;
/**
 * Iocp 管理连接和io 输入输出 
 */
class IocpPool:public ConnectionPool,public Singleton<IocpPool>
{
public:
	DYNAMIC_API IocpPool(void);
	DYNAMIC_API ~IocpPool(void);

public:
	/**
	* 初始化监听器
	* \param handle 句柄 
	*/
	DYNAMIC_API virtual void init(Socket *server){}
	/**
 	 * 增加socket,加入到工作者线程里
 	 * */
	DYNAMIC_API bool addSocket(Socket *socket);
	// 启动服务器
	DYNAMIC_API bool Start();

	//	停止服务器
	DYNAMIC_API void Stop();
	DYNAMIC_API virtual void start(WORD port)
	{
		SetPort(port);
		Start();
	}

	// 加载Socket库
	DYNAMIC_API static bool LoadSocketLib();

	// 卸载Socket库，彻底完事
	DYNAMIC_API void UnloadSocketLib() { WSACleanup(); }

	// 获得本机的IP地址
	DYNAMIC_API std::string GetLocalIP();

	// 设置监听端口
	DYNAMIC_API void SetPort( const int& nPort ) { m_nPort=nPort; }
protected:

	// 初始化IOCP
	bool _InitializeIOCP();

	// 初始化Socket
	bool _InitializeListenSocket();

	// 最后释放资源
	void _DeInitialize();

	// 投递Accept请求
	bool _PostAccept( IoContent* pAcceptIoContext ); 

	// 投递接收数据请求
	bool _PostRecv( IoContent* pIoContext );
	
	// 投递发送数据请求
	bool _PostSend( IoContent* pIoContext );

	// 在有客户端连入的时候，进行处理
	bool _DoAccpet( SocketContent* pSocketContext, IoContent* pIoContext );

	// 在有接收的数据到达的时候，进行处理
	bool _DoRecv( SocketContent* pSocketContext, IoContent* pIoContext ,DWORD len);
	
	// 在有发送的数据到达的时候，进行处理
	bool _DoSend( SocketContent* pSocketContext, IoContent* pIoContext ,DWORD len);
	// 将客户端的相关信息存储到数组中
	void _AddToContextList( SocketContent *pSocketContext );

	// 将客户端的信息从数组中移除
	void _RemoveContext( SocketContent *pSocketContext );

	// 清空客户端信息
	void _ClearContextList();

	// 将句柄绑定到完成端口中
	bool _AssociateWithIOCP( SocketContent *pContext);

	// 处理完成端口上的错误
	bool HandleError( SocketContent *pContext,const DWORD& dwErr );

	// 线程函数，为IOCP请求服务的工作者线程
	static DWORD WINAPI _WorkerThread(LPVOID lpParam);

	// 获得本机的处理器数量
	int _GetNoOfProcessors();

	// 判断客户端Socket是否已经断开
	bool _IsSocketAlive(SOCKET s);
	/**
	 * 执行定时检查行为
	 */
	void checkValidTick();
private:
	CheckValidThread *check; // 定时检查线程
	HANDLE                       m_hShutdownEvent;              // 用来通知线程系统退出的事件，为了能够更好的退出线程

	HANDLE                       m_hIOCompletionPort;           // 完成端口的句柄

	std::vector<IocpWorkThread*> _works;             // 工作者线程的句柄指针

	std::string                  m_strIP;                       // 服务器端的IP地址
	int                          m_nPort;                       // 服务器端的监听端口

	CRITICAL_SECTION             m_csContextList;               // 用于Worker线程同步的互斥量

	std::list<SocketContent*>  m_arrayClientContext;          // 客户端Socket的Context信息        
	typedef std::list<SocketContent*>::iterator CLIENT_CONTEXT_ITER;
	SocketContent*          m_pListenContext;              // 用于监听的Socket的Context信息

	LPFN_ACCEPTEX                m_lpfnAcceptEx;                // AcceptEx 和 GetAcceptExSockaddrs 的函数指针，用于调用这两个扩展函数
	LPFN_GETACCEPTEXSOCKADDRS    m_lpfnGetAcceptExSockAddrs; 
	friend class IocpWorkThread;
	friend class CheckValidThread;
};

#endif