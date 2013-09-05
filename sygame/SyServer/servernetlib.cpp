#include "servernetlib.h"
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