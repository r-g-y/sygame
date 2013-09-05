/**
 * 此文件只处理消息部分，逻辑部分交给各个控制器
 **/
#include "node.h"
/**
 * 远程过程调用实现体
 * Q:为啥不弄成c++ 的函数形式 
 * A: 该宏的实现就是封装的c++ 的实现,但是直接c++实际使用 不如宏来的明了
 * */
IMP_REMOTE_FUNCTION(Node::sayHello)
	PARAM(std::string,name); // 声明参数 注意次序与 定义时一致
	PARAM(int,id);
	BODY
	{	
		printf("%s %d\n",name.c_str(),id);
	}
REMOTE_FUNCTION_END

