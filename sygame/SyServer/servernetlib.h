#pragma once
#include "netlib.h"
#include "scriptLib.h"
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