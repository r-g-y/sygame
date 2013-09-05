#include "netlib.h"
#include "usercommand.h"
#if (0)
void NetLib::bindActions()
{
    {
        NetLibAction *action = new NetLibAction();
        action->type = script::__ACTION__RUN__;
        action->action = &NetLib::var;
        actions["var"] = action;
    }
    {
        NetLibAction *action = new NetLibAction();
        action->type = script::__ACTION_CHECK__;
        action->action = &NetLib::calc;
        actions["calc"] = action;
    }
    {
        NetLibAction *action = new NetLibAction();
        action->type = script::__ACTION__RUN__;
        action->action = &NetLib::exec;
        actions["exec"] = action;
    }
    {
        NetLibAction *action = new NetLibAction();
        action->type = script::__ACTION__RUN__;
        action->action = &NetLib::print;
        actions["print"] = action;
    }
    {
        NetLibAction *action = new NetLibAction();
        action->type = script::__ACTION_CHECK__;
        action->action = &NetLib::serverStart;
        actions["serverStart"] = action;
    }
    {
        NetLibAction *action = new NetLibAction();
        action->type = script::__ACTION_CHECK__;
        action->action = &NetLib::clientStart;
        actions["clientStart"] = action;
    }
	{
		NetLibAction *action = new NetLibAction();
		action->type = script::__ACTION_CHECK__;
		action->action = &NetLib::reg;
		actions["reg"] = action;
	}
	{
		NetLibAction *action = new NetLibAction();
		action->type = script::__ACTION_CHECK__;
		action->action = &NetLib::withNo;
		actions["withno"] = action;
	}
	{
		NetLibAction *action = new NetLibAction();
		action->type = script::__ACTION_CHECK__;
		action->action = &NetLib::loadMap;
		actions["loadmap"] = action;
	}
	{
		NetLibAction *action = new NetLibAction();
		action->type = script::__ACTION_CHECK__;
		action->action = &NetLib::withIp;
		actions["withip"] = action;
	}
}
int NetLib::serverStart(ServerNode* stub,script::tixmlCodeNode * node)
{
	if (!stub || !node) return 0;
	WORD port = 0;
	if (node->getAttr("port",port))
	{
		printf("获取port：%u",port);
		if (stub->start(port))
		{
			 return 1;
		}
	}
	return 0;
}

int NetLib::clientStart(ServerNode* stub,script::tixmlCodeNode * node)
{
	if (!stub || !node) return 0;
	int id = 0;
	std::string ip = node->getAttr("ip");
	WORD port = 0;
	node->getAttr("id",id);
	const char *name = node->getAttr("name");
	node->getAttr("port",port);
	std::string type = node->getAttr("type");
	if (id == 0)
	{
		if (type == "local")
		{
			ClientNode * client = theClient.addLocalClient(name);
			if (client)
			{
				client->useLocal();
			}
			return 1;
		}
		ClientNode * client = theClient.addRemoteClient(name);
		if (client)
		{
			if (!client->connect(ip.c_str(),port))
			{
				//client->useLocal();
				client->getSocket()->setInvalid();
				printf("服务器连接失败 %s:%u \n",ip.c_str(),port);
				return 0;
			}
			else
			{
				stTestCmd testCmd;
				cmd::Stream ss = testCmd.toStream();
				client->getSocket()->sendPacket(ss.content(),ss.size());
				printf("服务器连接成功<%s:%u>\n",ip.c_str(),port);
				return 1;
			}
		}
		else
		{
			printf("该类型的连接不存在:%d\n",id);
		}
	}
	return 0;
}

int NetLib::reg(ServerNode *stub, script::tixmlCodeNode *node)
{
	if (!stub || !node) return 0;
	
	std::string myip = node->getAttr("ip");
	WORD port = 0;
	node->getAttr("port",port);
	std::string name= node->getAttr("to");
	std::string nodeType = node->getAttr("meType");
	std::string mapName = node->getAttr("mapName");
	stub->req(name,myip,port,nodeType,mapName);
	return 0;
}

int NetLib::withNo(ServerNode *stub, script::tixmlCodeNode *node)
{
	if (!stub || !node) return 0;
	std::string name = node->getAttr("function");
	/**
	 * 卸载掉某个功能函数
	 */
	stub->downFunction(name);
	return 0;
}

int NetLib::withIp(ServerNode *stub, script::tixmlCodeNode *node)
{
	if (!stub || !node) return 0;
	std::string ip = node->getAttr("ip");
	std::string funcName = node->getAttr("function");
	stub->setFunctionInUseIp(ip,funcName);
	return 0;
}

int NetLib::loadMap(ServerNode *stub, script::tixmlCodeNode *node)
{
	if (!stub || !node) return 0;
	std::string name = node->getAttr("name");
	stub->loadMap(name);
	return 0;
}
#endif
/**
 * 构造是 会初始化socket类型
 */
ClientNode::ClientNode()
{
	_socket.type = Socket::NODE_CLIENT;
}
ClientManager theClient;
void ClientManager::start()
{
	for (CLIENTS_ITER iter = _clients.begin(); iter != _clients.end();++iter)
	{
		ClientNode *client = *iter;
		if (client && pool)
		{
//			
//			pool->addSocket(client->getSocket());
		}
	}
}

void ClientManager::release()
{
	for (CLIENTS_ITER iter = _clients.begin(); iter != _clients.end();++iter)
	{
		ClientNode *client = *iter;
		if (client)
		{
			delete client;
		}
	}
}
/**
 * 增加客户端
 * \param node 客户端节点
 */
ClientNode* ClientManager::addLocalClient(const char *name)
{
	for (CLIENTS_ITER iter = _clients.begin(); iter != _clients.end();++iter)
	{
		ClientNode *client = *iter;
		if (client && client->name == name)
		{
			return client;
		}
	}
	ClientNode *client = new ClientNode();
	client->name = name;
	_clients.push_back(client);
	return client;
}
/**
 * 设置连接池
 * \param pool 当前连接池
 */
void ClientManager::setConnectionPool(ConnectionPool * pool)
{
	this->pool = pool;
}
ClientNode * ClientManager::addRemoteClient(const char *name)
{
	for (CLIENTS_ITER iter = _clients.begin(); iter != _clients.end();++iter)
	{
		ClientNode *client = *iter;
		if (client && client->name == name)
		{
			return client;
		}
	}
	ClientNode *client = new ClientNode();
	client->name = name;
	client->getSocket()->setHandle(::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP));
	_clients.push_back(client);
	if (this->pool)
		this->pool->addSocket(client->getSocket());
	return client;
}
ClientNode * ClientManager::find(const char *name)
{
	for (CLIENTS_ITER iter = _clients.begin(); iter != _clients.end();++iter)
	{
		ClientNode *client = *iter;
		if (client && client->name == name)
		{
			return client;
		}
	}
	return NULL;
}
ClientNode *ClientManager::find(int clientType)
{
	return NULL;
}

/**
 * 启动客户端节点
 */
void ClientNode::start()
{

}
/**
 * 使用本地服务
 */
void ClientNode::useLocal()
{
	_socket.useLocalServer = true;
}
/**
 * 连接服务器
 **/
bool ClientNode::connect(const  char *name,WORD port)
{
	return !_socket.connect(name,port);
}
/**
 * 连接服务器
 **/
bool ClientNode::reconnect(const  char *name,WORD port)
{
	getSocket()->setHandle(::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP));
	if (theClient.pool)
		theClient.pool->addSocket(getSocket());
	return !_socket.connect(name,port);
}
/**
 * 设置为非阻塞
 */
void ClientNode::setnonblocking()
{

}
/**
 * 获取socket
 * \return 当前socket 的指针
 */
Socket * ClientNode::getSocket()
{
	return &_socket;
}

/**
 * 检查客户端是否有效
 * \return true 有效 false无效
 */
bool ClientNode::checkValid()
{
	return _socket.checkValid();
}

bool ServerNode::start(WORD port)
{
	return false;
}

