#pragma once
#include "socket.h"
#include "remotefunction.h"
/**
 * 新式远程调用,请使用 新的函数定义规则
 * 内部还是c++
 *  过程中隐含了对 socket 的声明,在调用时 麻烦加上
 **/
class Node:public RemoteFunction{
public:
    /**
     * 内部使用模板实现无c++对象必须是指针的限制
     * 参数总的数量不大于10,你可以再增加 形式很简单 在RemoveFunction 中扩展
     * 需要技能RemoteFunction class
     * 调用方式:
     * 		node->sayHello(socket,"jijinlong",1); // 调用远程函数 {我想大笑 终于搞定了,perfect}
     * 	这里当参数中函数指针时 麻烦手动释放,不建议使用指针来传递
     * 	 //TODO 需要弄个auto_ptr
     * 	返回值 自动约定了 void   
     * 	\param number 函数号 请唯一
     * 	\param sayHello 函数名
     * 	\param params 参数开始定义
     * */
    DEC_REMOTE_FUNCTION_2(sayHello,std::string name,int id)
	{
		PRE_CALL_2(sayHello,name,id);
	}
};
/**
 * 关于无缝地图
 * 地图节点先注册到world 然后由world 分配地图的块
 */
class AboutSeamlessMap:public RemoteFunction{
public:
	/**
	 * 注册地图到世界
	 * \param url节点的ip:port
	 */
	DEC_REMOTE_FUNCTION_1(registerMapNodeToWorld,
		std::string url)
	{
		PRE_CALL_1(registerMapNodeToWorld,url);
	}
	/**
	 * 分发数据到地图节点
	 * \param mapName 地图名字
	 */
	DEC_REMOTE_FUNCTION_1(distributeMapData,std::string mapName)
	{
		PRE_CALL_1(distributeMapData,mapName);
	}
};



