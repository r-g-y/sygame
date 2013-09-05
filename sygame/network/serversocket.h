#pragma once
#include "sys.h"
class ServerSocket{
public:
	/**
	 * 通过端口初始化server socket
	 * \param port 端口
	 */
	DYNAMIC_API bool init(unsigned short port);
	/**
	 * 释放资源
	 */
	DYNAMIC_API void release();
	/**
	 * 获取handle
	 */
	DYNAMIC_API sys::Socket& getHandle();
private:
	sys::Socket socket;
};