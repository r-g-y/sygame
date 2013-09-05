/**
 * free bsd 的socket 管理方案
 */
#pragma once
#include "sys.h"
#include "socket.h"
#include "singleton.h"
#include "net.h"
/**
 * 开辟一个线程用来处理socket 的管理
 */
class ActionQueueThread;
class KqueuePool: public ConnectionPool,public Singleton<KqueuePool>{
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
	DYNAMIC_API KqueuePool()
	{
		action = NULL;
	}	
	/**
	 * 处理资源释放
	 */
	DYNAMIC_API void release();
public:
	int handle; // 句柄
	void handleClose(Socket *socket);
	void handleRecv(Socket *socket);
	void handleWrite(Socket *socket);
	/**
	 * 增加n个socket
	 */
	void innerAddSocket(int size);
    Socket socket;
	ActionQueueThread * action;
	int kq;
	friend class ActionQueueThread;
};