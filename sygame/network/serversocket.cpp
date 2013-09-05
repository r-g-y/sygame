#include "serversocket.h"

/**
 * 通过端口初始化server socket
 * \param port 端口
 */
bool ServerSocket::init(unsigned short port)
{
#ifdef __LINUX__
	int reuse = 1;
	setsockopt(socket.getHandle(),SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
	socket.setHandle(::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP));
#endif
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	bzero(&(addr.sin_zero),8);
	socket.bind((struct sockaddr *)&addr, sizeof(addr));
	printf("--server bind port:%u\n---",port);
	socket.setnonblocking();
	socket.listen(4096);
	return 0;
}
/**
 * 释放资源
 */
void ServerSocket::release()
{
	socket.close();
}
/**
 * 获取handle
 */
sys::Socket& ServerSocket::getHandle()
{
	return socket;
}