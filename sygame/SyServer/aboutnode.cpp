#include "aboutnode.h"
#include "sstream"
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
	if (theAboutNode.handle(&theRequest,cmd,size,socket))
	{}
};


/**
 * 处理连接的增加个
 * \param socket 连接对象
 */
void DealClose::handleAdd(Socket *socket)
{
//	if (socket)
//		socket->setInvalidDelay(3000); // 3000 ms后自动断开
}
/**
 * 处理连接的释放
 * \param socket 连接对象
 */
void DealClose::handleClose(Socket *socket)
{
	ENTER_FUNCTION
	_mutex.lock();
	_mutex.unlock();
}

/**
 * 处理本地消息
 * \param socket 连接句柄
 * \param object 消息对象
 */
void DealCmd::doLocalCmd(Socket *socket,void *cmd,int size)
{
	ENTER_FUNCTION
	if (theAboutNode.handle(&theRequest,cmd,size,socket,LOCAL_CALL_FUNCTION))
	{}
	
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
	ENTER_FUNCTION
}
/**
 * 节点消息中心的初始化
 **/
void AboutNode::init()
{
	static bool init = false;
	if (!init) // 确保初始化只走一次
	{
		theRequest.init(this);
		init = true;
	}
}
