#pragma once
//#include "scriptLib.h"
#include "singleton.h"
#include "socket.h"
#include "net.h"
#include "sys.h"
#include "selectpool.h"
/**
 * 服务器节点
 */
class ServerNode{
public:
	/**
	 * 启动服务器节点
	 * \param port 服务器端口
	 * \return 是否成功
	 */
	DYNAMIC_API virtual bool start(WORD port);
	/**
	 * 注册行为
	 * \param name 远端节点名字
	 * \param ip 当前节点ip
	 * \param port 当前节点端口
	 * \param nodeType 节点类型
	 * \param mapName 节点中的地图的名字
	 */
	DYNAMIC_API virtual void req(std::string name,std::string ip,WORD port,std::string nodeType,std::string mapName){}

	/**
	 * 卸载掉相关功能
	 * \param name 功能函数
	 */
	DYNAMIC_API virtual void downFunction(std::string name){}
	/**
	 * 设置功能在仅在ip 的连接下使用
	 */
	DYNAMIC_API virtual void setFunctionInUseIp(std::string ip,std::string name){}
	
	/**
	 * 加载地图
	 * \param name 地图名字
	 */
	DYNAMIC_API virtual void loadMap(std::string name){}
};
/**
 * 客户端节点
 */
class ClientNode{
public:
	/**
	 * 构造是 会初始化socket类型
	 */
	DYNAMIC_API ClientNode();
	/**
	 * 节点名字
	 */
	std::string name;
	/**
	 * 服务器url ip:port
	 */
	std::string serverUrl;
	/**
	 * 启动客户端节点
	 */
	DYNAMIC_API void start();
	/**
	 * 使用本地服务
	 */
	DYNAMIC_API void useLocal();
	/**
	 * 连接服务器
	 **/
	DYNAMIC_API bool connect(const  char *name,WORD port);
	/**
	 * 连接服务器
	 **/
	DYNAMIC_API bool reconnect(const  char *name,WORD port);
	/**
	 * 设置为非阻塞
	 */
	DYNAMIC_API void setnonblocking();
	/**
	 * 获取socket
	 * \return 当前socket 的指针
	 */
	DYNAMIC_API Socket * getSocket();

	/**
	 * 检查客户端是否有效
	 * \return true 有效 false无效
	 */
	DYNAMIC_API bool checkValid();
private:
	Socket _socket;
};
/**
 *  负责脚本客户端连接
 **/
class ClientManager:public Singleton<ClientManager>{
public:
	DYNAMIC_API ClientManager()
	{
		pool = NULL;
	}
	/**
	 * 设置连接池
	 * \param pool 当前连接池
	 */
	DYNAMIC_API void setConnectionPool(ConnectionPool * pool);
	/**
	 * 开启
	 */
	DYNAMIC_API void start();
	/**
	 * 资源释放
	 */
	DYNAMIC_API void release();
	/**
	 * 找到客户端节点
	 * \param clientType 客户端类型
	 */
	DYNAMIC_API ClientNode * find(int clientType);
	/**
	 * 查找客户端节点
	 * \param name 名字
	 **/
	DYNAMIC_API ClientNode *find(const char *name);

	/**
	 * 增加客户端
	 * \param node 客户端节点
	 */
	DYNAMIC_API ClientNode* addLocalClient(const char *name);
	/**
	 * 增加远程客户端
	 * \param name 远程客户端
	 * \param clientnode
	 */
	DYNAMIC_API ClientNode* addRemoteClient(const char *name);
private:
	friend class ClientNode;
	std::vector<ClientNode*> _clients; // 客户端节点集合
	typedef std::vector<ClientNode*>::iterator CLIENTS_ITER;
	ConnectionPool * pool; // 连接池
};
DYNAMIC_API extern ClientManager theClient;

#if (0)
class NetLib;
typedef int (NetLib::*NodeAction)(ServerNode*,script::tixmlCodeNode*);

/**
 * 节点执行
 * */
struct NetLibAction{
	int type; // 执行点类型
	NodeAction action;  // 行为
};
/**
 * 节点执行库
 * */
class NetLib:public script::Lib<NetLibAction,ServerNode,NetLib>,
	public Singleton<NetLib>
{
public:
    /**
     * 只有在scope="stub" 的时候 type 才有效
     */
    int var(ServerNode* stub,script::tixmlCodeNode * node)
    {
        return script::Lib<NetLibAction,ServerNode,NetLib>::var(stub,node);
    }
    /**
     * 输出
     */
    int print(ServerNode* stub,script::tixmlCodeNode * node)
    {
    	return script::Lib<NetLibAction,ServerNode,NetLib>::print(stub,node);
    }
    /**
     * 计算变量
     **/
    int calc(ServerNode* stub,script::tixmlCodeNode * node)
    {
    	return script::Lib<NetLibAction,ServerNode,NetLib>::calc(stub,node);
    }
    /**
     * 执行其他节点
     **/
    int exec(ServerNode* stub,script::tixmlCodeNode * node)
    {
        execCode(stub,node->getAttr("name"));
        return 0;
    }
    /**
     *  启动服务器
     * */
    int serverStart(ServerNode* stub,script::tixmlCodeNode * node);
    /**
     * 启动客户端
     * */
    int clientStart(ServerNode* stub,script::tixmlCodeNode * node); 

	/**
	 * 注册行为
	 */
	int reg(ServerNode* stub,script::tixmlCodeNode * node);
    /**
	 * 加载地图
	 */
	int loadMap(ServerNode* stub,script::tixmlCodeNode * node);
	/**
	 * 卸载相关功能
	 */
	int withNo(ServerNode* stub,script::tixmlCodeNode * node);
	/**
	 * 在指定连接上 处理功能
	 */
	int withIp(ServerNode *stub, script::tixmlCodeNode *node);
	/**
     * 绑定行为
     * */
     void bindActions();
};

#define theNetLib NetLib::getMe()
#endif