#include "servernetlib.h"
#include "aboutnode.h"
#include "iocppool.h"
#include "selectpool.h"
#include "epollpool.h"
#include "kqueuepool.h"
#include <sstream>
/**
 * 服务器逻辑
 */
class Server:public ServerNode{
public:
#if __LINUX__
	std::vector<EpollThreadConnectionPoll*> pools;	
#elif __WINDOWS__
	std::vector<IocpPool*> pools;
#else
    std::vector<KqueuePool*> pools;
#endif
	
	void preLoad()
	{
#ifdef __WINDOWS__
		/**
		 * 预加载
		 * 主要是初始化windows 下的socket 环境
		 */
		IocpPool::LoadSocketLib();
#endif
	}
	void clear()
	{
#ifdef __WINDOWS__
		for (std::vector<IocpPool*>::iterator iter = pools.begin(); iter != pools.end();++iter)
		{
			(*iter)->Stop();
		}
		//pool.Stop();
#endif
	}
	/**
	 * 开启服务器节点
	 */
	bool start(WORD port)
	{
#if __WINDOWS__
		IocpPool *pool = new IocpPool;
#elif __LINUX__
		EpollThreadConnectionPoll * pool = new EpollThreadConnectionPoll;
#else 
        KqueuePool * pool = new KqueuePool;
#endif
		pool->setDealCmdDelegate(&theDeal); // 接受消息处理的代理
		pool->setSocketCloseDelegate(&theCloseDeal);
		pool->start(port);
		pools.push_back(pool);
		return true;
	}
	/**
	 * 注册行为
	 * \param 节点名字
	 * \param ip 当前节点的ip
	 * \param port 当前节点port
	 * \param nodeType 节点类型
	 * \param mapName 节点中的地图的名字
	 **/
	void req(std::string name,std::string ip,WORD port,std::string nodeType,std::string mapName)
	{
		printf("注册节点到:%s 当前节点类型:%s ip:%s port:%u\n",name.c_str(),nodeType.c_str(),ip.c_str(),port);
	}
	/**
	 * 加载地图
	 * \param name 地图名字
	 */
	void loadMap(std::string name)
	{
	}
	/**
	 * 卸载功能函数
	 * \param name 函数名字
	 */
	void downFunction(std::string name)
	{
		theAboutNode.setFuncName(name,NULL,false);
	}
	/**
	 * 设置功能在仅在ip 的连接下使用
	 * \param ip 端点地址
	 * \param name 功能名字
	 */
	virtual void setFunctionInUseIp(std::string ip,std::string name)
	{
		theAboutNode.setFuncName(name,NULL,true,ip);
	}
};
/**
 *
 * 启动器
 * 初始化 nodelib 并启动
 * //TODO 测试30000 人登录 并维持其连接 
 *    先启动loginnodemanager root 
 *    再启动login login可以先启动 在root,loginnodemanager启动后 才能进入逻辑处理流程
 *
 *	1个客户端 1s中发起1次连接 去loginnodemanager 查询有效节点
 *	获取login 节点 后 发起连接
 */
 int main(int argc,char*argv[])
 {
	// 加载后netlib 后
	// 启动客户端连接
	// 启动服务节点服务 
#ifdef __LINUX__
	 daemon(1,1);
#endif
	theAboutNode.init();
	Server server;
	server.preLoad();
	NetLib netlib;
	ConnectionSelectPool pool;
	pool.setDealCmdDelegate(&theDeal);
	pool.setSocketCloseDelegate(&theCloseDeal);
	theClient.setConnectionPool(&pool);
	if (argc == 2)
	{
		netlib.init(argv[1]);
		netlib.execScript(&server,"start"); // 根据start 逻辑执行
	}
	else
	{
		printf("请输入启动文件*.bw\n");
		system("pause");
		return 1;
	}
	pool.start();
	theClient.start();
	printf("\n~~~服务器启动成功~~~\n");
	sys::theThreadManager.waitAll();
	return 0;
 }
