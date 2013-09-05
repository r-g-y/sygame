#pragma once
#include "socket.h"
/**
 * 连接池
 */
class ConnectionPool{
public:
	DYNAMIC_API ConnectionPool()
	{
		delegate = NULL;
		closedelegate = NULL;
	}
	/**
	 * 开启服务
	 */
	DYNAMIC_API virtual void start(){
	
	}
	/**
	 * 开启端口服务
	 */
	DYNAMIC_API virtual void start(WORD port)
	{
	
	}
	/**
	 * 增加socket
	 * \param 传入构建后的socket 对象
	 */
	DYNAMIC_API virtual bool addSocket(Socket *socket){
		return false;
	};
	/**
	 * 设置消息处理器
	 */
	DYNAMIC_API virtual void setDealCmdDelegate(DealCmdDelegate *delegate)
	{
		this->delegate = delegate;
	}
	/**
	 * 设置socket 的关闭
	 * \param closedelegate 
	 */
	DYNAMIC_API virtual void setSocketCloseDelegate(SocketCloseDelegate *closedelegate)
	{
		this->closedelegate = closedelegate;
	}
protected:
	DealCmdDelegate *delegate;
	SocketCloseDelegate *closedelegate;
};