#pragma once
#include "mcenter.h"
#include "singleton.h"
#include "dealcmddelegate.h"
#include "remotefunction.h"
#include "sys.h"
/**
 * 连接管理
 */
enum {
	LOGIN_NODE_CONNECT, // 连接管理
	LOGIN_USER_CONNECT, // 玩家连接
	LOGIN_USER_IN_MANAGER_CONNECT, // 玩家在管理中心的连接
	MAP_NODE_CONNECT, // 地圖連接
	NODE_ROOT_CONNECT, // 跟节点连接
};
/**
 * 消息中心池
 */
class AboutNode:public MessageCenter,public Singleton<AboutNode>{
public:
    AboutNode(){init();}
    ~AboutNode(){destroy();}
    void init();
};

#define theAboutNode AboutNode::getMe()

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