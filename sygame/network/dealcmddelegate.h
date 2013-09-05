#pragma once
class Socket;
class DealCmdDelegate{
public:
	/**
	 * 处理消息的代理
	 */
	DYNAMIC_API virtual void doPackCmd(Socket*socket,void *cmd,int len) = 0;
	DYNAMIC_API virtual void doObjectCmd(Socket*socket,void *cmd,int len) = 0;
};
/**
 * 一个连接强制删除了
 */
class SocketCloseDelegate{
public:
	/**
	 *  处理socket 的关闭
	 * \param socket 连接
	 **/
	DYNAMIC_API virtual void handleClose(Socket *socket) = 0; 
	/**
	 * 处理socket 的增加
	 * \param socket 连接
	 */
	DYNAMIC_API virtual void handleAdd(Socket *socket) = 0;
};