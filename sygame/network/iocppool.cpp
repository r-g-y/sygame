#include "iocppool.h"
#include "usercommand.h"
#ifdef __WINDOWS__
// 每一个处理器上产生多少个线程(为了最大限度的提升服务器性能，详见配套文档)
#define WORKER_THREADS_PER_PROCESSOR 2
// 同时投递的Accept请求的数量(这个要根据实际的情况灵活设置)
#define MAX_POST_ACCEPT              4000
// 传递给Worker线程的退出信号
#define EXIT_CODE                    NULL


// 释放指针和句柄资源的宏

// 释放指针宏
#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}
// 释放句柄宏
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
// 释放Socket宏
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}

class IocpWorkThread:public sys::Thread{
public:
	void run()
	{
		while(isActive())
		{
		//	printf("wait at iocp work thread...\n");
			OVERLAPPED *pOverlapped = NULL;
			SocketContent		 *pSocketContext = NULL;
			DWORD                dwBytesTransfered = 0;
			BOOL bReturn = GetQueuedCompletionStatus(
				pool->m_hIOCompletionPort,
				&dwBytesTransfered,
				(PULONG_PTR)&pSocketContext,
				&pOverlapped,
				INFINITE
			);
			// 如果收到的是退出标志，则直接退出
			if ( EXIT_CODE==(DWORD)pSocketContext )
			{
				break;
			}
			
			// 判断是否出现了错误
			if( !bReturn )  
			{  
				DWORD dwErr = GetLastError();
		//		printf("do error\n");
				// 显示一下提示信息
				if( !pool->HandleError( pSocketContext,dwErr ) )
				{
					continue;
				}

				continue;  
			}  
			else  
			{
				// 读取传入的参数
				IoContent* pIoContext = CONTAINING_RECORD(pOverlapped, IoContent, overlapped);  

				// 判断是否有客户端断开了
				if((0 == dwBytesTransfered) && ( RECV_POSTED==pIoContext->optype || SEND_POSTED==pIoContext->optype))  
				{  
			//		pool->_ShowMessage( _T("客户端 %s:%d 断开连接."),inet_ntoa(pSocketContext->m_ClientAddr.sin_addr), ntohs(pSocketContext->m_ClientAddr.sin_port) );

					// 释放掉对应的资源
					if (pool->closedelegate)
							pool->closedelegate->handleClose(pSocketContext);
					pool->_RemoveContext( pSocketContext );
			//		printf("do error\n");
 					continue;  
				}  
				else
				{
					if (!pSocketContext->checkValid())
					{
						pSocketContext->getHandle().close();
						if (pool->closedelegate)
							pool->closedelegate->handleClose(pSocketContext);
						pool->_RemoveContext( pSocketContext );
						continue;
					}
					switch( pIoContext->optype )  
					{  
						 // Accept  
						case ACCEPT_POSTED:
						{ 
							// 为了增加代码可读性，这里用专门的_DoAccept函数进行处理连入请求
			//				printf("do post\n");
							pool->_DoAccpet( pSocketContext, pIoContext );						
						}
						break;

						// RECV
						case RECV_POSTED:
						{
							// 为了增加代码可读性，这里用专门的_DoRecv函数进行处理接收请求
			//				printf("do recv\n");
							pool->_DoRecv( pSocketContext,pIoContext ,dwBytesTransfered);
						}
						break;

						// SEND
						// 这里略过不写了，要不代码太多了，不容易理解，Send操作相对来讲简单一些
						case SEND_POSTED:
						{
				//			printf("do send\n");
							pool->_DoSend(pSocketContext,pIoContext,dwBytesTransfered);
						}
						break;
						default:
							break;
					} //switch

					
				}
			}
		}
		printf("work thread over\n");
	}
	IocpWorkThread(IocpPool *pool):pool(pool)
	{
	}
private:
	IocpPool *pool;
};

class CheckValidThread:public sys::Thread{
public:
	void run()
	{
		while(isActive())
		{
			sys::sTime::wait(300); // 等待1s
			if (pool) pool->checkValidTick();
		}
	}
	CheckValidThread(IocpPool *pool):pool(pool)
	{
	
	}
private:
	IocpPool *pool;
};

IocpPool::IocpPool(void):
							m_hShutdownEvent(NULL),
							m_hIOCompletionPort(NULL),
							m_strIP(DEFAULT_IP),
							m_nPort(DEFAULT_PORT),
							m_lpfnAcceptEx( NULL ),
							m_pListenContext( NULL )
{
	check = NULL;
}


IocpPool::~IocpPool(void)
{
	// 确保资源彻底释放
	this->Stop();
}
//====================================================================================
//
//				    系统初始化和终止
//
//====================================================================================
////////////////////////////////////////////////////////////////////
// 初始化WinSock 2.2
bool IocpPool::LoadSocketLib()
{    
	WSADATA wsaData;
	int nResult;
	nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	// 错误(一般都不可能出现)
	if (NO_ERROR != nResult)
	{
//		this->_ShowMessage(_T("初始化WinSock 2.2失败！\n"));
		return false; 
	}

	return true;
}

//////////////////////////////////////////////////////////////////
//	启动服务器
bool IocpPool::Start()
{
	// 初始化线程互斥量
	InitializeCriticalSection(&m_csContextList);

	// 建立系统退出的事件通知
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// 初始化IOCP
	if (false == _InitializeIOCP())
	{
		return false;
	}
	else
	{
	}

	// 初始化Socket
	if( false==_InitializeListenSocket() )
	{
		this->_DeInitialize();
		return false;
	}
	else
	{
		
	}

	return true;
}


////////////////////////////////////////////////////////////////////
//	开始发送系统退出消息，退出完成端口和线程资源
void IocpPool::Stop()
{
	if( m_pListenContext!=NULL && m_pListenContext->getHandle().checkValid())
	{
		// 激活关闭消息通知
		SetEvent(m_hShutdownEvent);

//		for (int i = 0; i < m_nThreads; i++)
//		{
//			// 通知所有的完成端口操作退出
//			PostQueuedCompletionStatus(m_hIOCompletionPort, 0, (DWORD)EXIT_CODE, NULL);
//		}

		// 清除客户端列表信息
		this->_ClearContextList();

		// 释放其他资源
		this->_DeInitialize();

	}	
}


////////////////////////////////
// 初始化完成端口
bool IocpPool::_InitializeIOCP()
{
	// 建立第一个完成端口
	m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );

	if ( NULL == m_hIOCompletionPort)
	{
		//this->_ShowMessage(_T("建立完成端口失败！错误代码: %d!\n"), WSAGetLastError());
		return false;
	}

	// 根据本机中的处理器数量，建立对应的线程数
int	m_nThreads = WORKER_THREADS_PER_PROCESSOR * _GetNoOfProcessors();
	
	// 为工作者线程初始化句柄
	//m_phWorkerThreads = new HANDLE[m_nThreads];
	
	// 根据计算出来的数量建立工作者线程
	//DWORD nThreadID;
	for (int i = 0; i < m_nThreads; i++)
	{
		IocpWorkThread * work = new IocpWorkThread(this);
		_works.push_back(work);
		work->start();
	}
	check = new CheckValidThread(this);
	check->start();
//	TRACE(" 建立 _WorkerThread %d 个.\n", m_nThreads );

	return true;
}


/////////////////////////////////////////////////////////////////
// 初始化Socket
bool IocpPool::_InitializeListenSocket()
{
	// AcceptEx 和 GetAcceptExSockaddrs 的GUID，用于导出函数指针
	GUID GuidAcceptEx = WSAID_ACCEPTEX;  
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 

	// 服务器地址信息，用于绑定Socket
	struct sockaddr_in ServerAddress;

	// 生成用于监听的Socket的信息
	m_pListenContext = new SocketContent;

	// 需要使用重叠IO，必须得使用WSASocket来建立Socket，才可以支持重叠IO操作
//	m_pListenContext->socket = new Socket();
	m_pListenContext->setHandle(WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED));
	if (!m_pListenContext->getHandle().checkValid()) 
	{
	//	this->_ShowMessage("初始化Socket失败，错误代码: %d.\n", WSAGetLastError());
		return false;
	}
	else
	{
	//	TRACE("WSASocket() 完成.\n");
	}

	// 将Listen Socket绑定至完成端口中
	if( NULL== CreateIoCompletionPort( (HANDLE)m_pListenContext->getHandle().getHandle(), m_hIOCompletionPort,(DWORD)m_pListenContext, 0))  
	{  
	//	this->_ShowMessage("绑定 Listen Socket至完成端口失败！错误代码: %d/n", WSAGetLastError());  
		m_pListenContext->getHandle().close();
		return false;
	}
	else
	{
	//	TRACE("Listen Socket绑定完成端口 完成.\n");
	}

	// 填充地址信息
	ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));
	ServerAddress.sin_family = AF_INET;
	// 这里可以绑定任何可用的IP地址，或者绑定一个指定的IP地址 
	//ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);                      
	ServerAddress.sin_addr.s_addr = inet_addr(m_strIP.c_str());         
	ServerAddress.sin_port = htons(m_nPort);                          

	// 绑定地址和端口
	if (SOCKET_ERROR == bind(m_pListenContext->getHandle().getHandle(), (struct sockaddr *) &ServerAddress, sizeof(ServerAddress))) 
	{
//		this->_ShowMessage("bind()函数执行错误.\n");
		return false;
	}
	else
	{
//		TRACE("bind() 完成.\n");
	}

	// 开始进行监听
	if (SOCKET_ERROR == listen(m_pListenContext->getHandle().getHandle(),SOMAXCONN))
	{
//		this->_ShowMessage("Listen()函数执行出现错误.\n");
		return false;
	}
	else
	{
	//	TRACE("Listen() 完成.\n");
	}

	// 使用AcceptEx函数，因为这个是属于WinSock2规范之外的微软另外提供的扩展函数
	// 所以需要额外获取一下函数的指针，
	// 获取AcceptEx函数指针
	DWORD dwBytes = 0;  
	if(SOCKET_ERROR == WSAIoctl(
		m_pListenContext->getHandle().getHandle(), 
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidAcceptEx, 
		sizeof(GuidAcceptEx), 
		&m_lpfnAcceptEx, 
		sizeof(m_lpfnAcceptEx), 
		&dwBytes, 
		NULL, 
		NULL))  
	{  
	//	this->_ShowMessage("WSAIoctl 未能获取AcceptEx函数指针。错误代码: %d\n", WSAGetLastError()); 
		this->_DeInitialize();
		return false;  
	}  

	// 获取GetAcceptExSockAddrs函数指针，也是同理
	if(SOCKET_ERROR == WSAIoctl(
		m_pListenContext->getHandle().getHandle(), 
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidGetAcceptExSockAddrs,
		sizeof(GuidGetAcceptExSockAddrs), 
		&m_lpfnGetAcceptExSockAddrs, 
		sizeof(m_lpfnGetAcceptExSockAddrs),   
		&dwBytes, 
		NULL, 
		NULL))  
	{  
	//	this->_ShowMessage("WSAIoctl 未能获取GuidGetAcceptExSockAddrs函数指针。错误代码: %d\n", WSAGetLastError());  
		this->_DeInitialize();
		return false; 
	}  


	// 为AcceptEx 准备参数，然后投递AcceptEx I/O请求
	for( int i=0;i<MAX_POST_ACCEPT;i++ )
	{
		// 新建一个IO_CONTEXT
		IoContent* pAcceptIoContext = m_pListenContext->GetNewIoContext();
		
		if( false==this->_PostAccept( pAcceptIoContext ) )
		{
			m_pListenContext->RemoveContext(pAcceptIoContext);
			return false;
		}
	}

	//this->_ShowMessage( _T("投递 %d 个AcceptEx请求完毕"),MAX_POST_ACCEPT );

	return true;
}

////////////////////////////////////////////////////////////
//	最后释放掉所有资源
void IocpPool::_DeInitialize()
{
	// 删除客户端列表的互斥量
	DeleteCriticalSection(&m_csContextList);

	// 关闭系统退出事件句柄
	RELEASE_HANDLE(m_hShutdownEvent);

	// 释放工作者线程句柄指针
	for(unsigned int i=0;i<_works.size();i++ )
	{
		if (_works[i])
			_works[i]->stop();
	}
	if (check)
	{
		check->stop();
	}
	//RELEASE(m_phWorkerThreads);

	// 关闭IOCP句柄
	RELEASE_HANDLE(m_hIOCompletionPort);

	// 关闭监听Socket
	RELEASE(m_pListenContext);

//	this->_ShowMessage("释放资源完毕.\n");
}


//====================================================================================
//
//				    投递完成端口请求
//
//====================================================================================


//////////////////////////////////////////////////////////////////
// 投递Accept请求
bool IocpPool::_PostAccept( IoContent* pAcceptIoContext )
{
	//ASSERT( INVALID_SOCKET!=m_pListenContext->m_Socket );

	// 准备参数
	DWORD dwBytes = 0;  
	pAcceptIoContext->optype = ACCEPT_POSTED;  
	WSABUF *p_wbuf   = &pAcceptIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pAcceptIoContext->overlapped;
	
	// 为以后新连入的客户端先准备好Socket( 这个是与传统accept最大的区别 ) 
	SOCKET socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);  
	if( INVALID_SOCKET ==  socket)  
	{  
	//	_ShowMessage("创建用于Accept的Socket失败！错误代码: %d", WSAGetLastError()); 
		return false;  
	} 
	pAcceptIoContext->sockfd = socket;
	// 投递AcceptEx
	if(FALSE == m_lpfnAcceptEx( m_pListenContext->getHandle().getHandle(),
						socket,
						p_wbuf->buf,0,// p_wbuf->len - ((sizeof(SOCKADDR_IN)+16)*2),   
								sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, &dwBytes, p_ol)) 
	{  
		char buffer[1024]={'\0'};
		sprintf(buffer,"投递 AcceptEx 请求失败，错误代码: %d\n", WSAGetLastError());
		if(WSA_IO_PENDING != WSAGetLastError())  
		{  
	//		_ShowMessage("投递 AcceptEx 请求失败，错误代码: %d", WSAGetLastError());  
			return false;  
		}  
	} 

	return true;
}

////////////////////////////////////////////////////////////
// 在有客户端连入的时候，进行处理
// 流程有点复杂，你要是看不懂的话，就看配套的文档吧....
// 如果能理解这里的话，完成端口的机制你就消化了一大半了

// 总之你要知道，传入的是ListenSocket的Context，我们需要复制一份出来给新连入的Socket用
// 原来的Context还是要在上面继续投递下一个Accept请求
//
bool IocpPool::_DoAccpet( SocketContent* pSocketContext, IoContent* pIoContext )
{
	//if (m_arrayClientContext.size() >= 4000) return false;
	SOCKADDR_IN* ClientAddr = NULL;
	SOCKADDR_IN* LocalAddr = NULL;  
	int remoteLen = sizeof(SOCKADDR_IN), localLen = sizeof(SOCKADDR_IN);  

	///////////////////////////////////////////////////////////////////////////
	// 1. 首先取得连入客户端的地址信息
	// 这个 m_lpfnGetAcceptExSockAddrs 不得了啊~~~~~~
	// 不但可以取得客户端和本地端的地址信息，还能顺便取出客户端发来的第一组数据，老强大了...
	this->m_lpfnGetAcceptExSockAddrs(pIoContext->m_wsaBuf.buf,0,// pIoContext->m_wsaBuf.len - ((sizeof(SOCKADDR_IN)+16)*2), 
		sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, (LPSOCKADDR*)&LocalAddr, &localLen, (LPSOCKADDR*)&ClientAddr, &remoteLen);  

	printf(("客户端 %s:%d 连入.\n"), inet_ntoa(ClientAddr->sin_addr), ntohs(ClientAddr->sin_port) );
//	this->_ShowMessage( _T("客户额 %s:%d 信息：%s."),inet_ntoa(ClientAddr->sin_addr), ntohs(ClientAddr->sin_port),pIoContext->m_wsaBuf.buf );
	
	pSocketContext->ip =  inet_ntoa(ClientAddr->sin_addr);
	pSocketContext->port =  ntohs(ClientAddr->sin_port);
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. 这里需要注意，这里传入的这个是ListenSocket上的Context，这个Context我们还需要用于监听下一个连接
	// 所以我还得要将ListenSocket上的Context复制出来一份为新连入的Socket新建一个SocketContext
#if (0)
	SocketContent* pNewSocketContext = new SocketContent;
#else
	Ref<SocketContent> ref = Ref<SocketContent>::get();
	SocketContent* pNewSocketContext = ref.pointer();
	pNewSocketContext->ref = ref.getRef();
#endif
	pNewSocketContext->setHandle(pIoContext->sockfd);
	memcpy(&(pNewSocketContext->m_ClientAddr), ClientAddr, sizeof(SOCKADDR_IN));

	// 参数设置完毕，将这个Socket和完成端口绑定(这也是一个关键步骤)
	if( false==this->_AssociateWithIOCP( pNewSocketContext ) )
	{
		RELEASE( pNewSocketContext );
		return false;
	}  


	///////////////////////////////////////////////////////////////////////////////////////////////////
	// 3. 继续，建立其下的IoContext，用于在这个Socket上投递第一个Recv数据请求
	{
		IoContent* pNewIoContext = &(pNewSocketContext->recvIo);//GetNewIoContext();
		pNewIoContext->optype       = RECV_POSTED;
		pNewIoContext->socket = pNewSocketContext;
	//	pNewIoContext->socket->getHandle().setHandle(pNewSocketContext->socket->getHandle().getHandle());
		// 如果Buffer需要保留，就自己拷贝一份出来
		//memcpy( pNewIoContext->m_szBuffer,pIoContext->m_szBuffer,MAX_BUFFER_LEN );

		// 绑定完毕之后，就可以开始在这个Socket上投递完成请求了
		if( false==this->_PostRecv( pNewIoContext) )
		{
			pNewSocketContext->RemoveContext( pNewIoContext );
		//	RELEASE( pNewSocketContext );
			return false;
		}
	}
	{
		// 3.1 投递一个SEND 数据请求ssssssssssssssssssss
		IoContent* pNewIoContext = &(pNewSocketContext->sendIo);// pNewSocketContext->GetNewIoContext();
		pNewIoContext->optype       = SEND_POSTED;
		pNewIoContext->socket = pNewSocketContext;
		//pNewIoContext->socket->getHandle().setHandle(pNewSocketContext->socket->getHandle().getHandle());
		// 如果Buffer需要保留，就自己拷贝一份出来
		//memcpy( pNewIoContext->m_szBuffer,pIoContext->m_szBuffer,MAX_BUFFER_LEN );

		// 绑定完毕之后，就可以开始在这个Socket上投递完成请求了
		if( false==this->_PostSend( pNewIoContext) )
		{
			pNewSocketContext->RemoveContext( pNewIoContext );
//			RELEASE( pNewSocketContext );
			//return false;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// 4. 如果投递成功，那么就把这个有效的客户端信息，加入到ContextList中去(需要统一管理，方便释放资源)
	this->_AddToContextList( pNewSocketContext );
	if (this->closedelegate)
		this->closedelegate->handleAdd(pNewSocketContext);
	////////////////////////////////////////////////////////////////////////////////////////////////
	// 5. 使用完毕之后，把Listen Socket的那个IoContext重置，然后准备投递新的AcceptEx
	pIoContext->ResetBuffer();
	return this->_PostAccept( pIoContext ); 	
}

////////////////////////////////////////////////////////////////////
// 投递接收数据请求
bool IocpPool::_PostRecv( IoContent* pIoContext )
{
	// 初始化变量
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	WSABUF *p_wbuf   = &pIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pIoContext->overlapped;

	pIoContext->ResetBuffer();
	pIoContext->optype = RECV_POSTED;
	if (!pIoContext->socket) return false;
	// 初始化完成后，，投递WSARecv请求
	int nBytesRecv = WSARecv( pIoContext->socket->getHandle().getHandle(), p_wbuf, 1, &pIoContext->msgLen, &dwFlags, p_ol, NULL );

	// 如果返回值错误，并且错误的代码并非是Pending的话，那就说明这个重叠请求失败了
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		//this->_ShowMessage("投递第一个WSARecv失败！");
		return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////
// 投递发送数据请求
bool IocpPool::_PostSend( IoContent* pIoContext )
{
	// 初始化变量
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	WSABUF *p_wbuf   = &pIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pIoContext->overlapped;

	pIoContext->ResetBuffer();
	pIoContext->optype = SEND_POSTED;
	p_wbuf->buf = pIoContext->m_szBuffer;
	if (!pIoContext->socket) return false;
	int len = pIoContext->socket->copySendBuffer(pIoContext->m_szBuffer,MAX_BUFFER_LEN);
	p_wbuf->len = len; 
	if (!len) return true;
	// 初始化完成后，，投递WSARecv请求
	int nBytesRecv = WSASend( pIoContext->socket->getHandle().getHandle(), p_wbuf, 1, &dwBytes, dwFlags, p_ol, NULL );

	// 如果返回值错误，并且错误的代码并非是Pending的话，那就说明这个重叠请求失败了
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
	//	this->_ShowMessage("投递第一个WSARecv失败！");
		return false;
	}
	return true;
}

void SocketContent::wakeupSend()
{
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	WSABUF *p_wbuf   = &sendIo.m_wsaBuf;
	OVERLAPPED *p_ol = &sendIo.overlapped;

	sendIo.ResetBuffer();
	sendIo.optype = SEND_POSTED;
	p_wbuf->buf = sendIo.m_szBuffer;
	if (!sendIo.socket) return;
	stTestCmd testCmd;
	sendIo.socket->sendObject(&testCmd);
	p_wbuf->len = sendIo.socket->copySendBuffer(sendIo.m_szBuffer,MAX_BUFFER_LEN);
	// 初始化完成后，，投递WSARecv请求
	if (!p_wbuf->len) return;
	int nBytesRecv = WSASend( sendIo.socket->getHandle().getHandle(), p_wbuf, 1, &dwBytes, dwFlags, p_ol, NULL );

	// 如果返回值错误，并且错误的代码并非是Pending的话，那就说明这个重叠请求失败了
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
	//	this->_ShowMessage("投递第一个WSARecv失败！");
		return ;
	}
	return ;
}
/////////////////////////////////////////////////////////////////
// 在有接收的数据到达的时候，进行处理
bool IocpPool::_DoRecv( SocketContent* pSocketContext, IoContent* pIoContext ,DWORD len)
{
	// 先把上一次的数据显示出现，然后就重置状态，发出下一个Recv请求
	SOCKADDR_IN* ClientAddr = &pSocketContext->m_ClientAddr;
	//printf("收到  %s:%d 信息：%s\n",inet_ntoa(ClientAddr->sin_addr), ntohs(ClientAddr->sin_port),pIoContext->m_wsaBuf.buf );
	pIoContext->socket->done(delegate,pIoContext->m_wsaBuf.buf,len);	
	// 然后开始投递下一个WSARecv请求
	return _PostRecv( pIoContext );
}
/////////////////////////////////////////////////////////////////
// 在有发送的数据到达的时候，进行处理
bool IocpPool::_DoSend( SocketContent* pSocketContext, IoContent* pIoContext ,DWORD len)
{
	// 先把上一次的数据显示出现，然后就重置状态，发出下一个Recv请求
	SOCKADDR_IN* ClientAddr = &pSocketContext->m_ClientAddr;
	//this->_ShowMessage( _T("收到  %s:%d 信息：%s"),inet_ntoa(ClientAddr->sin_addr), ntohs(ClientAddr->sin_port),pIoContext->m_wsaBuf.buf );

	// 然后开始投递下一个WSARecv请求
	return _PostSend( pIoContext );
}



/////////////////////////////////////////////////////
// 将句柄(Socket)绑定到完成端口中
bool IocpPool::_AssociateWithIOCP( SocketContent *pContext )
{
	// 将用于和客户端通信的SOCKET绑定到完成端口中
	HANDLE hTemp = CreateIoCompletionPort((HANDLE)pContext->getHandle().getHandle(), m_hIOCompletionPort, (DWORD)pContext, 0);

	if (NULL == hTemp)
	{
		//this->_ShowMessage(("执行CreateIoCompletionPort()出现错误.错误代码：%d"),GetLastError());
		return false;
	}

	return true;
}




//====================================================================================
//
//				    ContextList 相关操作
//
//====================================================================================


//////////////////////////////////////////////////////////////
// 将客户端的相关信息存储到数组中
void IocpPool::_AddToContextList( SocketContent *pHandleData )
{
	EnterCriticalSection(&m_csContextList);

	m_arrayClientContext.push_back(pHandleData);	
	
	LeaveCriticalSection(&m_csContextList);
}

/////////////////////////////////////////////////////////Z///////
//	移除某个特定的Context
void IocpPool::_RemoveContext( SocketContent *pSocketContext )
{
	EnterCriticalSection(&m_csContextList);

	for( CLIENT_CONTEXT_ITER iter = m_arrayClientContext.begin();
			iter !=  m_arrayClientContext.end();++iter )
	{
		if( pSocketContext== *iter )
		{
			printf(".....................................当前删除的连接:%s %u\n",pSocketContext->getIp().c_str(),pSocketContext->port);
			pSocketContext->getHandle().close(); // 再次设置无效
			RELEASE( pSocketContext );			
			m_arrayClientContext.erase(iter);
			break;
		}
	}
	printf(".....................................当前连接剩余数:%lu\n",m_arrayClientContext.size());
	LeaveCriticalSection(&m_csContextList);
}

////////////////////////////////////////////////////////////////
// 清空客户端信息
void IocpPool::_ClearContextList()
{
	EnterCriticalSection(&m_csContextList);

	for( CLIENT_CONTEXT_ITER iter = m_arrayClientContext.begin();
			iter !=  m_arrayClientContext.end();++iter )
	{
		if(*iter )
		{
			delete *iter;		
		}
	}

	m_arrayClientContext.clear();

	LeaveCriticalSection(&m_csContextList);
}

// 执行定时检查行为

void IocpPool::checkValidTick()
{
	if (!m_hIOCompletionPort) return;
	EnterCriticalSection(&m_csContextList);
	int  count = 0;
	for( CLIENT_CONTEXT_ITER iter = m_arrayClientContext.begin();
			iter !=  m_arrayClientContext.end();++iter )
	{
		if(*iter )
		{
			(*iter)->setInvalidTimeOut();
			if (!(*iter)->checkValid())
				count ++;
		}
	}
//	if (count)
//		printf("-------------------------------当前设置连接无效的个数为:%u\n",count);
	LeaveCriticalSection(&m_csContextList);
}
//====================================================================================
//
//				       其他辅助函数定义
//
//====================================================================================



////////////////////////////////////////////////////////////////////
// 获得本机的IP地址
std::string IocpPool::GetLocalIP()
{
	// 获得本机主机名
	char hostname[MAX_PATH] = {0};
	gethostname(hostname,MAX_PATH);                
	struct hostent FAR* lpHostEnt = gethostbyname(hostname);
	if(lpHostEnt == NULL)
	{
		return DEFAULT_IP;
	}

	// 取得IP地址列表中的第一个为返回的IP(因为一台主机可能会绑定多个IP)
	LPSTR lpAddr = lpHostEnt->h_addr_list[0];      

	// 将IP地址转化成字符串形式
	struct in_addr inAddr;
	memmove(&inAddr,lpAddr,4);
	m_strIP = std::string( inet_ntoa(inAddr) );        

	return m_strIP;
}

///////////////////////////////////////////////////////////////////
// 获得本机中处理器的数量
int IocpPool::_GetNoOfProcessors()
{
	SYSTEM_INFO si;

	GetSystemInfo(&si);

	return si.dwNumberOfProcessors;
}

/////////////////////////////////////////////////////////////////////
// 判断客户端Socket是否已经断开，否则在一个无效的Socket上投递WSARecv操作会出现异常
// 使用的方法是尝试向这个socket发送数据，判断这个socket调用的返回值
// 因为如果客户端网络异常断开(例如客户端崩溃或者拔掉网线等)的时候，服务器端是无法收到客户端断开的通知的

bool IocpPool::_IsSocketAlive(SOCKET s)
{
	int nByteSent=send(s,"",0,0);
	if (-1 == nByteSent) return false;
	return true;
}

///////////////////////////////////////////////////////////////////
// 显示并处理完成端口上的错误
bool IocpPool::HandleError( SocketContent *pContext,const DWORD& dwErr )
{
	// 如果是超时了，就再继续等吧  
	if(WAIT_TIMEOUT == dwErr)  
	{  	
		// 确认客户端是否还活着...
		if( !( pContext->getHandle().checkValid()) )
		{
			if (closedelegate)
				closedelegate->handleClose(pContext);
	//		this->_ShowMessage( _T("检测到客户端异常退出！") );
			this->_RemoveContext( pContext );
			return true;
		}
		else
		{
	//		this->_ShowMessage( _T("网络操作超时！重试中...") );
			return true;
		}
	}  

	// 可能是客户端异常退出了
	else if( ERROR_NETNAME_DELETED==dwErr )
	{
		if (closedelegate)
				closedelegate->handleClose(pContext);
	//	this->_ShowMessage( _T("检测到客户端异常退出！") );
		this->_RemoveContext( pContext );
		return true;
	}

	else
	{
	//	this->_ShowMessage( _T("完成端口操作出现错误，线程退出。错误代码：%d"),dwErr );
		return false;
	}
}

bool IocpPool::addSocket(Socket *socket)
{
	SocketContent* pNewSocketContext = new SocketContent();// socket;
	pNewSocketContext->setHandle(socket->getHandle().getHandle());

	// 参数设置完毕，将这个Socket和完成端口绑定(这也是一个关键步骤)
	if( false==this->_AssociateWithIOCP( pNewSocketContext ) )
	{
		RELEASE( pNewSocketContext );
		return false;
	}  


	///////////////////////////////////////////////////////////////////////////////////////////////////
	// 3. 继续，建立其下的IoContext，用于在这个Socket上投递第一个Recv数据请求
	{
		IoContent* pNewIoContext = &pNewSocketContext->recvIo;//GetNewIoContext();
		pNewIoContext->optype       = RECV_POSTED;
		pNewIoContext->socket = pNewSocketContext;
		// 如果Buffer需要保留，就自己拷贝一份出来
		//memcpy( pNewIoContext->m_szBuffer,pIoContext->m_szBuffer,MAX_BUFFER_LEN );

		// 绑定完毕之后，就可以开始在这个Socket上投递完成请求了
		if( false==this->_PostRecv( pNewIoContext) )
		{
			pNewSocketContext->RemoveContext( pNewIoContext );
			return false;
		}
	}
	{
		// 3.1 投递一个SEND 数据请求
		IoContent* pNewIoContext = &pNewSocketContext->sendIo;//GetNewIoContext();
		pNewIoContext->optype       = SEND_POSTED;
		pNewIoContext->socket = pNewSocketContext;
		// 如果Buffer需要保留，就自己拷贝一份出来
		//memcpy( pNewIoContext->m_szBuffer,pIoContext->m_szBuffer,MAX_BUFFER_LEN );

		// 绑定完毕之后，就可以开始在这个Socket上投递完成请求了
		if( false==this->_PostSend( pNewIoContext) )
		{
			pNewSocketContext->RemoveContext( pNewIoContext );
			return false;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// 4. 如果投递成功，那么就把这个有效的客户端信息，加入到ContextList中去(需要统一管理，方便释放资源)
	this->_AddToContextList( pNewSocketContext );
	
	return true;
}


#endif