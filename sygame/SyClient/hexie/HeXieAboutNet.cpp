#include "HeXieAboutNet.h"
#include "sstream"
#include "netlib.h"
#include "selectpool.h"
#include "AboutRequest.h"

/**
 * 处理消息
 * \param socket 连接
 * \param cmd 消息
 * \param size 消息大小
 */
void DealCmd::doObjectCmd(Socket *socket,void *cmd,int size)
{
	ENTER_FUNCTION
	theAboutNode.handle(&theRequest,cmd,size,socket);
};


/**
 * 处理连接的增加个
 * \param socket 连接对象
 */
void DealClose::handleAdd(Socket *socket)
{
	
}
/**
 * 处理连接的释放
 * \param socket 连接对象
 */
void DealClose::handleClose(Socket *socket)
{
	ENTER_FUNCTION
	_mutex.lock();
	ClientNode * client = theClient.addRemoteClient("request");
	if (client)
	{
		if (socket == client->getSocket())
		{
			printf("xxx");
		}
	}
	_mutex.unlock();
}

/**
 * 处理本地消息
 * \param socket 连接句柄
 * \param object 消息对象
 */
void DealCmd::doLocalCmd(Socket *socket,void *cmd,int size)
{
	// 不处理
};
/**
 * 处理对齐数据
 * 我们用作对客户端消息的处理
 * \param socket 连接句柄
 * \param cmd 消息
 * \param size 消息大小
 */
void DealCmd::doPackCmd(Socket *socket,void *cmd,int size)
{
	// 不处理
}
AboutNet theAboutNode ;
/**
 * 节点消息中心的初始化
 **/
void AboutNet::init()
{
	static bool init = false;
	if (!init) // 确保初始化只走一次
	{
		// 登陆流程
		BIND_FUNCTION(1,AboutRequest,getMapInfo); //注册到登陆中心
		BIND_FUNCTION(2,AboutRequest,retMapInfo); //注册到登陆中心
		init = true;
		ConnectionSelectPool *pool = new ConnectionSelectPool();
		pool->setDealCmdDelegate(&theDeal);
		pool->setSocketCloseDelegate(&theCloseDeal);
		theClient.setConnectionPool(pool);
		pool->start();
		theClient.start();
		WSADATA wsaData;
		int nResult;
		nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		// 错误(一般都不可能出现)
		if (NO_ERROR != nResult)
		{
	//		this->_ShowMessage(_T("初始化WinSock 2.2失败！\n"));
			return; 
		}	

	}
}


bool AboutNet::connect()
{
	ClientNode * client = theClient.addRemoteClient("request");
	std::string ip="localhost";
	WORD port = 4008;
	if (client)
	{
		if (!client->connect(ip.c_str(),port))
		{
			client->getSocket()->setInvalid();
			return false;
		}
		else
		{
			client->reconnect(ip.c_str(),port);
			theRequest.getMapInfo(client->getSocket(),100,100); // 发送请求map的信息
			//client->getSocket()->setInvalid();
		}
	}
	return true;
}