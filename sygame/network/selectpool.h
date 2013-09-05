#pragma once
#include "net.h"
/**
 * 使用select 管理连接池
 */
class SelectWorkThread;
class ConnectionSelectPool:public ConnectionPool,public Singleton<ConnectionSelectPool>
{
public:
	DYNAMIC_API ConnectionSelectPool()
	{
		selectWorkThread = NULL;
	}
	/**
	* 开启select 管理
	*/
	DYNAMIC_API virtual void start();
	/**
	 * 开启start 管理
	 * \param 从port 口 开启工作者线程
	 */
	DYNAMIC_API virtual void start(WORD port);
	/**
	 * 增加socket 进入连接管理器
 	 */
	DYNAMIC_API virtual bool addSocket(Socket *socket);
private:
	bool checkAddSocket();
	bool innerAddSocket(Socket *socket);
	friend class SelectWorkThread;
	std::list<Socket* > tasks;
	std::list<Socket* > exttasks; // 线程外部加入的socket
	sys::Mutex _mutex;
	fd_set rfds,wfds,errfds;
	int handle; // 监听句柄
	SelectWorkThread * selectWorkThread; // 工作线程
	Socket socket; // 服务器socket

	/**
	 * 执行io 
 	 */
	virtual void action();
	
	/**
	 * 增加处理接受的机制
	 * 传入当前句柄 handle
	 */
	virtual void doMakeSocket();
	/**
	 * 检查无效连接
	 */
	virtual void check();
	/**
	 * 清除连接
	 */
	virtual void clear();
};