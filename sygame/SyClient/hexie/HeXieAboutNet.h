#pragma once
#include "mcenter.h"
#include "singleton.h"
#include "dealcmddelegate.h"
#include "remotefunction.h"
#include "nullcommand.h"
#include "netlib.h"
/**
 * 消息中心池
 */
class AboutNet:public MessageCenter,public Singleton<AboutNet>{
public:
    AboutNet(){}
    ~AboutNet(){destroy();}
    void init();
	bool connect();
};
extern AboutNet theAboutNode ;
/**
 * 处理消息
 **/
class DealCmd:public DealCmdDelegate,public Singleton<DealCmd>,public LocalCallDelegate{
public:
	/**
	 * 处理消息
	 * \param socket 连接
	 * \param cmd 消息
	 * \param size 消息大小
	 */
	void doObjectCmd(Socket *socket,void *cmd,int size);
	/**
	 * 处理本地消息
	 * \param socket 连接句柄
	 * \param object 消息对象
	 */
	void doLocalCmd(Socket *socekt,void *cmd,int size);

	/**
	 * 处理对齐数据
	 * \param socket 连接句柄
	 * \param cmd 消息
	 * \param size 消息大小
	 */
	void doPackCmd(Socket *socekt,void *cmd,int size);
};
/**
 * 处理连接的关闭
 */
class DealClose:public SocketCloseDelegate,public Singleton<DealClose>{
public:
	/**
	 * 处理连接的释放
	 * \param socket 连接对象
	 */
	void handleClose(Socket *socket);
	/**
	 * 处理连接的增加
	 * \param socket 连接对象
	 */
	void handleAdd(Socket *socket);
	/**
	 *  互斥鎖
	 **/
	sys::Mutex _mutex;
};
#define theDeal DealCmd::getMe()
#define theCloseDeal DealClose::getMe()