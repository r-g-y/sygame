/**
 * 实现的轻量级的epollt连接 线程管理
 * 单线程接受 工作 
 * select 只支持多进程的工作方式
 * */
#pragma once
#include "sys.h"
#include "socket.h"
#include "singleton.h"
#include "net.h"
/**
 * 使用epoll 管理连接池
 */
class ListenerThread; // 监听线程
class RecycleThread; // 回收者线程
class WorkThread;//工作者线程
class EpollThreadConnectionPoll:public ConnectionPool,public Singleton<EpollThreadConnectionPoll>{
public:
	/**
	* 初始化监听器
	* \param handle 句柄 
	*/
	DYNAMIC_API virtual void start(WORD port);
	/**
 	 * 增加socket,加入到工作者线程里
 	 * */
	DYNAMIC_API bool addSocket(Socket *socket);
	/**
	 * 删除socket
	 * \param socket 待删除的socket
 	 */
	DYNAMIC_API void delSocket(Socket *socket);
	DYNAMIC_API EpollThreadConnectionPoll()
	{
		listenThread = NULL;
		recycleThread = NULL;
	}	
	DYNAMIC_API void release();
public:
	int handle; // 句柄
	void handleClose(Socket *socket);
	void handleRecv(Socket *socket);
private:
	std::vector<WorkThread*> _workThreads;
	typedef std::vector<WorkThread*>::iterator WORK_THREADS_ITER;
	ListenerThread * listenThread;
	RecycleThread * recycleThread;
	Socket socket;
};
