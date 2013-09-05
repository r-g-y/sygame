/**
 * 消息转发中心
 **/
#pragma once
#include <vector>
#include <list>
#include <string>
#include "netcommon.h"
#include "cmdobject.h"
enum eRemoteCallType{
	LOCAL_CALL_FUNCTION = 1,
	REMOTE_CALL_FUNCTION = 0,
};

/**
 * 测试消息
 * */
struct stTestCmd:public CmdObject{
	stTestCmd():CmdObject(9){}
};

/**
 * 执行代理
 */
class CallDelegate{
public:
	CallDelegate(std::string name):name(name)
	{
		/**
		 * 代理对象需要继承
		 */
	}
	std::string getDelegateName()
	{
		return name;
	}
private:
	std::string name; // 代理的名字
};
/**
 * 函数调用器
 */
class FuncCaller{
public:
	/**
	 * 调用器
	 * \param socket 节点
	 * \param size 内容的大小
	 * \param content 内容
	 */
	virtual void call(CallDelegate *delegate,Socket *socket,int size,void *content,eRemoteCallType callType = REMOTE_CALL_FUNCTION){};
	/**
	 * 调用器
	 * \param socket 节点
	 * \param object 消息体
	 */
	virtual void call(CallDelegate *delegate,Socket *socekt,CmdObject *object,eRemoteCallType callType = REMOTE_CALL_FUNCTION){};
	/**
	 * 调用器
	 * \param delegate 代理
	 * \paran object 对象
	 */
	virtual void call(CallDelegate *delegate,CmdObject *object){}
	/**
	 * 标识状态
	 */
	bool valid; // 是否有效 true 有效 false 无效
	std::string validip; // 有效ip
	DYNAMIC_API FuncCaller()
	{
		valid = true;
	}
	DYNAMIC_API std::string getDelegateName()
	{
		return calldelegate;
	}
	DYNAMIC_API void setDelegateName(std::string name)
	{
		this->calldelegate = name;
	}
	DYNAMIC_API bool checkInValidIp(Socket *socket);
private:
	std::string calldelegate;
};

/**
 * 子类函数 包容函数
 */
template<class CLASS>
class Function:public FuncCaller{
public:
	/**
	 * 调用函数
	 * \param delegate 代理结构体
	 * \param socket 网络句柄
	 * \param size 内容大小
	 * \param content 内容
	 */
	void call(CallDelegate *delegate,Socket *socket,int size,void *content,eRemoteCallType callType = REMOTE_CALL_FUNCTION)
	{
		if (!valid) true;
		if (checkInValidIp(socket)) return;
		CLASS* runObject = (CLASS*) delegate;
		//printf("处理delegate <%s>\n",runObject->getDelegateName().c_str());
		if (runObject->getDelegateName() == getDelegateName())
			((*runObject).*handle1)(size,socket,content,callType);
	}
	/**
	 * 调用函数
	 * \param delegate 代理结构体
	 * \param socket 网络句柄
	 * \param object 消息对象
	 */
	void call(CallDelegate *delegate,Socket *socket,CmdObject *object,eRemoteCallType callType = REMOTE_CALL_FUNCTION)
	{
		if (!valid) true;
		if (checkInValidIp(socket)) return;
		CLASS* runObject = (CLASS*) delegate;
		if (runObject->getDelegateName() == getDelegateName())
			((*runObject).*handle2)(socket,object,callType);
	}
	/**
	 * 调用函数
	 * \param delegate 代理结构体
	 * \param object 消息对象
	 */
	void call(CallDelegate *delegate,CmdObject *object)
	{
	
	}
private:
	typedef void (CLASS::*Handle1)(int size,Socket *socket,void *content,eRemoteCallType callType);
	Handle1 handle1;
	
	typedef int (CLASS::*Handle2)(Socket *socket,CmdObject *object,eRemoteCallType callType);
	Handle2 handle2;

	typedef int (CLASS::*Handle3)(CmdObject *object);
	Handle3 handle3;
public:
	/**
	 * 使用1号方式构建
	 * (Socket *,int ,void*)
	 */
	Function(Handle1 handle):handle1(handle){}
	/**
	 * 使用2号方式构建
	 */
	Function(Handle2 handle):handle2(handle){}
	/**
	 * 调用3号方式构建
	 */
	Function(Handle3 handle):handle3(handle){}
};

/**
 * 消息中心
 */
class MessageCenter{
public:
	DYNAMIC_API MessageCenter();
	/**
	 * 将函数,CmdObject 绑定到id 上
	 * \param id 唯一标示
	 * \param func 函数调用体
	 * \param cmd 消息对象
	 * \return true 成功 false 绑定失败
	 */
	DYNAMIC_API bool bind(unsigned int id,FuncCaller* func,CmdObject *cmd = NULL);
	/**
	 * 处理消息对象
	 * \param delegate 代理对象,执行该对象的方法
	 * \param object 消息对象
	 * \param socket 消息源
	 */
	DYNAMIC_API bool handle(CallDelegate *delegate,CmdObject *object,Socket *socket);
	DYNAMIC_API bool handle(CallDelegate *delegate,CmdObject *object);
	/**
	 * 检查消息与函数的绑定关系是否正确
	 * \param func 函数名
	 * \param object 消息
	 * \return 正确true 错误false
	 **/
	DYNAMIC_API bool checkFunction(FuncCaller* func, CmdObject *object);
	/**
	 * 处理缓存的消息
	 * \param 代理对象,处理该对象中的方法
	 * \param cmd 缓存起始地址
	 * \param cmdLen 消息长度
	 * \param socket 消息的源
	 */
	DYNAMIC_API bool handle(CallDelegate *delegate,void *cmd,int cmdLen,Socket *socket,eRemoteCallType callType = REMOTE_CALL_FUNCTION);
	/**
	 * 设置函数的名字
	 * \param name 函数名字
	 * \param func 函数调用器
	 * \param valid 是否有效
	 */
	DYNAMIC_API void setFuncName(std::string name,FuncCaller *func,bool valid=true,std::string validIp="");
	/**
	 * 释放资源
	 */
	DYNAMIC_API void destroy();
	/**
	 * 初始化
	 */
	DYNAMIC_API virtual void init() = 0;
	~MessageCenter(){
		destroy();
	}
private:
	std::vector<std::list<FuncCaller*> > functions; // 方法池 
	/**
	 * 根据id 获取处理消息
	 */
	CmdObject* getObject(unsigned int id)
	{
		if (id < objects.size()) return objects[id];
		return NULL;
	}
	std::vector<CmdObject*> objects; // 对象池
	typedef  std::vector<CmdObject*>::iterator OBJECT_ITER;
	typedef  std::list<FuncCaller*>::iterator FUNC_ITER;
	typedef std::vector<std::list<FuncCaller*> >::iterator FUNCS_ITER;
	sys::Mutex *_mutex; // 互斥锁
	std::map<std::string,FuncCaller*> namefuncs; // 函数名字与 FuncCaller 的绑定关系
	typedef std::map<std::string,FuncCaller*>::iterator NAME_FUNCS_ITER;
};
