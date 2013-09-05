#pragma once
#include "connection.h"
/**
 * 客户端socket 完成连接
 * 获取connection
 */
class ClientSocket{
public:
	ClientSocket()
	{
		connection = NULL;
	}
	/**
	 * 连接到 ip.port 的服务器
	 */
	DYNAMIC_API bool connect(const char *ip,unsigned short port);
	
	/**
	 * 获取连接
	 * \reeturn 连接
	 */
	DYNAMIC_API Connection* getConnection();
	
	/**
	 * 断开连接
	 */
	DYNAMIC_API void disconnect();
private:
	Connection* connection;
};