#pragma once

/**
 * 提供一些方式来 定义 调用 分布式环境下的 函数
 **/
#include "stdarg.h"
#include "stdio.h"
#include <string>
#include "mcenter.h"
#include "netcommon.h"
#include "singleton.h"
#include "dealcmddelegate.h"
/**
 * 发送实例
 **/
/*
int dfunction(int argc,...)
{
    va_list arg_ptr;
	va_start(arg_ptr,argc);
    cmd::Object object;
    cmd::Stream stream;
    int size = va_arg(arg_ptr,int);
	char* content = va_arg(arg_ptr,char*);
    
    stream.set(content,size);
    int a ;
    std::string name;
    object.getRecord(a,stream);
    object.getRecord(name,stream);
    printf("%d \n",a); // 执行体
    printf("%s \n",name.c_str()); // 打印名字
}

int cfunction(int argc,...)
{
	va_list arg_ptr;
	va_start(arg_ptr,argc);
	int a = va_arg(arg_ptr,int);
	std::string* name = va_arg(arg_ptr,std::string*);
    
    cmd::Object object;
    cmd::Stream stream;
    object.addRecord(a,stream);
    object.addRecord(*name,stream);
    
	printf("%d %s\n",a,name->c_str());
	dfunction(argc,stream.size(),stream.content());
    va_end(arg_ptr);
}*/
#define DEC_REMOTE_FUNCTION(__function__id__,__function__)\
    static int __function__(){ return __function__id__;}\
	void __function__(Socket *socket,int size,void *content)\

#define IMP_REMOTE_FUNCTION(__function__)\
    void __function__(int size,Socket *socket,void *msg,eRemoteCallType callType)\
    {\
		int __function__id__ = __function__();\
        CmdObject object(__function__id__);\
        cmd::Stream stream;\
		if (size >= sizeof(int))\
        {\
            char* content = (char*) msg;\
            stream.set(content + sizeof(int),size-sizeof(int));\
        }\

#define PARAM(__type__,__name__)\
    __type__ __name__;\
    {\
        object.getRecord(__name__,stream);\
    }

#define BODY

#define REMOTE_FUNCTION_END }

#define REMOTE_CALL(__function__)\
    __function__(socket,0,
	                 
#define END_CALL );

#define MAKE_REMOTE_CLASS(className)\
	className():CallDelegate(#className){}\
	void init(MessageCenter *center);
#define BEGIN_REMOTE_CLASS(className) \
	void className::init(MessageCenter *center)\
	{\
		int id = 0;\

#define END_REMOTE_CLASS }
#define DEC_REMOTE_CLASS_BEGIN(className)\
class className:public RemoteFunction:public Singleton<className>,public CallDelegate{\
public:className():CallDelegate(#className){}

#define DEC_REMOTE_CLASS_END }; 

#define BIND_FUNCTION(id,CLASS,FUNCTION)\
	do{\
	Function<CLASS>* func = new Function<CLASS>(&CLASS::FUNCTION);\
	if (bind(id,func))\
	{\
		CLASS::FUNCTION(id);\
	}\
	else\
	{\
		delete func;\
	}\
	std::stringstream ss;\
	func->setDelegateName(#CLASS);\
	ss<<#CLASS<<"::"<<#FUNCTION;\
	setFuncName(ss.str(),func);\
	}while(false)
#define REQ_FUNCTION(CLASS,FUNCTION)\
	do{\
	id++;\
	Function<CLASS>* func = new Function<CLASS>(&CLASS::FUNCTION);\
	if (center->bind(id,func))\
	{\
		CLASS::FUNCTION(id);\
	}\
	else\
	{\
		delete func;\
	}\
	std::stringstream ss;\
	func->setDelegateName(#CLASS);\
	ss<<#CLASS<<"::"<<#FUNCTION;\
	center->setFuncName(ss.str(),func);\
	}while(false)
/**
 * 定义一个远程过程调用体
 **/
/*
IMP_REMOTE_FUNCTION(rfunction,1)
    PARAM(int,a); // 远程过程的相关函数的参数
    PARAM_POINTER(std::string,name); // c++ 对象只处理指针
    BODY{
        printf("%d \n",a); // 执行体
        printf("%s \n",name.c_str()); // 打印名字
    }
REMOTE_FUNCTION_END
*/

/**
 * 一堆模版实现 用来支撑很多参数的调用
 * 目前最多支撑10个参数,很方便扩展
 * */
/**
 * 对不起 又来宏了 哥真心写累了
 * */
#define BEGIN_REMOTE_FUNCTION_CALL\
	if (!socket) return -1;\
	CmdObject object(functionid);\
       	cmd::Stream stream;\
       	stream.append(&object.__msg__id__,sizeof(object.__msg__id__));
#define END_REMOTE_FUNCTION_CALL\
	doSendPack(socket,stream.content(),stream.size(),tag);\
	return 0;
#define REMOTE_ARG(t)\
	object.addRecord(t,stream);

class LocalCallDelegate{
public:
	virtual void doLocalCmd(Socket *socekt,void *cmd,int size) = 0;
};

class RemoteFunction{
public:
	RemoteFunction()
	{
		localDelegate = NULL;
	}
	
	/**
	 * 设置本地处理代理
	 * \param call 本地处理代理
	 */
	void setLocalCallDelegate(LocalCallDelegate *call)
	{
		this->localDelegate = call;
	}
	
	template<class TCMD>
	int send(Socket *socket,TCMD cmd)
	{
		doSendObject(socket,&cmd);
		return 0;
	}
	DYNAMIC_API void doSendObject(Socket *socket,CmdObject *object);
	DYNAMIC_API void doSendPack(Socket *socket,void* cmd,unsigned len,eRemoteCallType tag);
	int call(Socket *socket,int functionid,eRemoteCallType tag = REMOTE_CALL_FUNCTION)
	{
		BEGIN_REMOTE_FUNCTION_CALL
		END_REMOTE_FUNCTION_CALL
	}
	template<class T1>
	int call(Socket *socket,int functionid,T1 t1,eRemoteCallType tag = REMOTE_CALL_FUNCTION)
	{
		BEGIN_REMOTE_FUNCTION_CALL
			REMOTE_ARG(t1);	
		END_REMOTE_FUNCTION_CALL
	}	
	template<class T1,class T2>
	int call(Socket *socket,int functionid,T1 t1,T2 t2,eRemoteCallType tag = REMOTE_CALL_FUNCTION)
	{
		BEGIN_REMOTE_FUNCTION_CALL
			REMOTE_ARG(t1);
			REMOTE_ARG(t2);	
		END_REMOTE_FUNCTION_CALL
	}	
	template<class T1,class T2,class T3>
	int call(Socket *socket,int functionid,T1 t1,T2 t2,T3 t3,eRemoteCallType tag = REMOTE_CALL_FUNCTION)
	{
		BEGIN_REMOTE_FUNCTION_CALL
			REMOTE_ARG(t1);
			REMOTE_ARG(t2);
			REMOTE_ARG(t3);
		END_REMOTE_FUNCTION_CALL
	}	
	template<class T1,class T2,class T3,class T4>
	int call(Socket *socket,int functionid,T1 t1,T2 t2,T3 t3,T4 t4,eRemoteCallType tag = REMOTE_CALL_FUNCTION)
	{
		BEGIN_REMOTE_FUNCTION_CALL
			REMOTE_ARG(t1);
			REMOTE_ARG(t2);
			REMOTE_ARG(t3);
			REMOTE_ARG(t4);	
		END_REMOTE_FUNCTION_CALL
	}	
	template<class T1,class T2,class T3,class T4,class T5>
	int call(Socket *socket,int functionid,T1 t1,T2 t2,T3 t3,T4 t4,T5 t5,eRemoteCallType tag = REMOTE_CALL_FUNCTION)
	{
		BEGIN_REMOTE_FUNCTION_CALL
			REMOTE_ARG(t1);
			REMOTE_ARG(t2);
			REMOTE_ARG(t3);
			REMOTE_ARG(t4);
			REMOTE_ARG(t5);	
		END_REMOTE_FUNCTION_CALL
	}
	template<class T1,class T2,class T3,class T4,class T5,class T6>
	int call(Socket *socket,int functionid,T1 t1,T2 t2,T3 t3,T4 t4,T5 t5,T6 t6,eRemoteCallType tag = REMOTE_CALL_FUNCTION)
	{
		BEGIN_REMOTE_FUNCTION_CALL
			REMOTE_ARG(t1);
			REMOTE_ARG(t2);
			REMOTE_ARG(t3);
			REMOTE_ARG(t4);
			REMOTE_ARG(t5);
			REMOTE_ARG(t6);
		END_REMOTE_FUNCTION_CALL
	}
	template<class T1,class T2,class T3,class T4,class T5,class T6,class T7>
	int call(Socket *socket,int functionid,T1 t1,T2 t2,T3 t3,T4 t4,T5 t5,T6 t6,T7 t7,eRemoteCallType tag = REMOTE_CALL_FUNCTION)
	{
		BEGIN_REMOTE_FUNCTION_CALL
			REMOTE_ARG(t1);
			REMOTE_ARG(t2);
			REMOTE_ARG(t3);
			REMOTE_ARG(t4);
			REMOTE_ARG(t5);
			REMOTE_ARG(t6);
			REMOTE_ARG(t7);
		END_REMOTE_FUNCTION_CALL
	}
	template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
	int call(Socket *socket,int functionid,T1 t1,T2 t2,T3 t3,T4 t4,T5 t5,T6 t6,T7 t7,T8 t8,eRemoteCallType tag = REMOTE_CALL_FUNCTION)
	{
		BEGIN_REMOTE_FUNCTION_CALL
			REMOTE_ARG(t1);
			REMOTE_ARG(t2);
			REMOTE_ARG(t3);
			REMOTE_ARG(t4);
			REMOTE_ARG(t5);
			REMOTE_ARG(t6);
			REMOTE_ARG(t7);
			REMOTE_ARG(t8);
		END_REMOTE_FUNCTION_CALL
	}
	template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
	int call(Socket *socket,int functionid,T1 t1,T2 t2,T3 t3,T4 t4,T5 t5,T6 t6,T7 t7,T8 t8,T9 t9,eRemoteCallType tag = REMOTE_CALL_FUNCTION)
	{
		BEGIN_REMOTE_FUNCTION_CALL
			REMOTE_ARG(t1);
			REMOTE_ARG(t2);
			REMOTE_ARG(t3);
			REMOTE_ARG(t4);
			REMOTE_ARG(t5);
			REMOTE_ARG(t6);
			REMOTE_ARG(t7);
			REMOTE_ARG(t8);
			REMOTE_ARG(t9);
		END_REMOTE_FUNCTION_CALL
	}
	template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10>
	int call(Socket *socket,int functionid,T1 t1,T2 t2,T3 t3,T4 t4,T5 t5,T6 t6,T7 t7,T8 t8,T9 t9,T10 t10,eRemoteCallType tag = REMOTE_CALL_FUNCTION)
	{
		BEGIN_REMOTE_FUNCTION_CALL
			REMOTE_ARG(t1);
			REMOTE_ARG(t2);
			REMOTE_ARG(t3);
			REMOTE_ARG(t4);
			REMOTE_ARG(t5);
			REMOTE_ARG(t6);
			REMOTE_ARG(t7);
			REMOTE_ARG(t8);
			REMOTE_ARG(t9);
			REMOTE_ARG(t10);
		END_REMOTE_FUNCTION_CALL
	}
private:
	LocalCallDelegate * localDelegate; // 本地处理代理
};
#define DEC_REMOTE_FUNCTION_0(functionname)\
	static int functionname(int id = 0){\
	static int _functionid = 0; \
	if (_functionid == 0) _functionid = id;return _functionid;}\
	void functionname(int size,Socket *socket,void *msg,eRemoteCallType callType=REMOTE_CALL_FUNCTION);\
	void functionname(Socket *socket,eRemoteCallType tag = REMOTE_CALL_FUNCTION)

#define DEC_REMOTE_FUNCTION_1(functionname,a1)\
	static int functionname(int id=0){\
	static int _functionid = 0; \
	if (_functionid == 0) _functionid = id;return _functionid;}\
	void functionname(int size,Socket *socket,void *msg,eRemoteCallType callType=REMOTE_CALL_FUNCTION);\
	void functionname(Socket *socket,a1,eRemoteCallType tag = REMOTE_CALL_FUNCTION)

#define DEC_REMOTE_FUNCTION_2(functionname,a1,a2)\
	static int functionname(int id=0){\
	static int _functionid = 0; \
	if (_functionid == 0) _functionid = id;return _functionid;}\
	void functionname(int size,Socket *socket,void *msg,eRemoteCallType callType=REMOTE_CALL_FUNCTION);\
	void functionname(Socket *socket,a1,a2,eRemoteCallType tag = REMOTE_CALL_FUNCTION)

#define DEC_REMOTE_FUNCTION_3(functionname,a1,a2,a3)\
	static int functionname(int id=0){\
	static int _functionid = 0; \
	if (_functionid == 0) _functionid = id;return _functionid;}\
	void functionname(int size,Socket *socket,void *msg,eRemoteCallType callType=REMOTE_CALL_FUNCTION);\
	void functionname(Socket *socket,a1,a2,a3,eRemoteCallType tag = REMOTE_CALL_FUNCTION)

#define DEC_REMOTE_FUNCTION_4(functionname,a1,a2,a3,a4)\
	static int functionname(int id=0){\
	static int _functionid = 0; \
	if (_functionid == 0) _functionid = id;return _functionid;}\
	void functionname(int size,Socket *socket,void *msg,eRemoteCallType callType=REMOTE_CALL_FUNCTION);\
	void functionname(Socket *socket,a1,a2,a3,a4,eRemoteCallType tag = REMOTE_CALL_FUNCTION)

#define DEC_REMOTE_FUNCTION_5(functionname,a1,a2,a3,a4,a5)\
	static int functionname(int id=0){\
	static int _functionid = 0; \
	if (_functionid == 0) _functionid = id;return _functionid;}\
	void functionname(int size,Socket *socket,void *msg,eRemoteCallType callType=REMOTE_CALL_FUNCTION);\
	void functionname(Socket *socket,a1,a2,a3,a4,a5,eRemoteCallType tag = REMOTE_CALL_FUNCTION)

#define DEC_REMOTE_FUNCTION_6(functionname,a1,a2,a3,a4,a5,a6)\
	static int functionname(int id=0){\
	static int _functionid = 0; \
	if (_functionid == 0) _functionid = id;return _functionid;}\
	void functionname(Socket *socket,int size,void *msg,eRemoteCallType callType=REMOTE_CALL_FUNCTION);\
	void functionname(Socket *socket,a1,a2,a3,a4,a5,a6,eRemoteCallType tag = REMOTE_CALL_FUNCTION)

#define DEC_REMOTE_FUNCTION_7(functionname,a1,a2,a3,a4,a5,a6,a7)\
	static int functionname(int id=0){\
	static int _functionid = 0; \
	if (_functionid == 0) _functionid = id;return _functionid;}\
	void functionname(int size,Socket *socket,void *msg,eRemoteCallType callType=REMOTE_CALL_FUNCTION);\
	void functionname(Socket *socket,a1,a2,a3,a4,a5,a6,a7,eRemoteCallType tag = REMOTE_CALL_FUNCTION)

#define DEC_REMOTE_FUNCTION_8(functionname,a1,a2,a3,a4,a5,a6,a7,a8)\
	static int functionname(int id=0){\
	static int _functionid = 0; \
	if (_functionid == 0) _functionid = id;return _functionid;}\
	void functionname(int size,Socket *socket,void *msg,eRemoteCallType callType=REMOTE_CALL_FUNCTION);\
	void functionname(Socket *socket,a1,a2,a3,a4,a5,a6,a7,a8,eRemoteCallType tag = REMOTE_CALL_FUNCTION)

#define DEC_REMOTE_FUNCTION_9(functionname,a1,a2,a3,a4,a5,a6,a7,a8,a9)\
	static int functionname(int id=0){\
	static int _functionid = 0; \
	if (_functionid == 0) _functionid = id;return _functionid;}\
	void functionname(int size,Socket *socket,void *msg,eRemoteCallType callType=REMOTE_CALL_FUNCTION);\
	void functionname(Socket *socket,a1,a2,a3,a4,a5,a6,a7,a8,a9,eRemoteCallType tag = REMOTE_CALL_FUNCTION)

#define DEC_REMOTE_FUNCTION_10(functionname,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10)\
	static int functionname(int id=0){\
	static int _functionid = 0; \
	if (_functionid == 0) _functionid = id;return _functionid;}\
	void functionname(int size,Socket *socket,void *msg,eRemoteCallType callType=REMOTE_CALL_FUNCTION);\
	void functionname(Socket *socket,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,eRemoteCallType tag = REMOTE_CALL_FUNCTION)

//一堆宏用来使我们的形式更加简洁
#define PRE_CALL_0(function)\
	call(socket,function(),tag)
#define PRE_CALL_1(function,a1)\
	call(socket,function(),a1,tag)
#define PRE_CALL_2(function,a1,a2)\
	call(socket,function(),a1,a2,tag)
#define PRE_CALL_3(function,a1,a2,a3)\
	call(socket,function(),a1,a2,a3,tag)
#define PRE_CALL_4(function,a1,a2,a3,a4)\
	call(socket,function(),a1,a2,a3,a4,tag)
#define PRE_CALL_5(function,a1,a2,a3,a4,a5)\
	call(socket,function(),a1,a2,a3,a4,a5,tag)
#define PRE_CALL_6(function,a1,a2,a3,a4,a5,a6)\
	call(socket,function(),a1,a2,a3,a4,a5,a6,tag)
#define PRE_CALL_7(function,a1,a2,a3,a4,a5,a6,a7)\
	call(socket,function(),a1,a2,a3,a4,a5,a6,a7,tag)
#define PRE_CALL_8(function,a1,a2,a3,a4,a5,a6,a7,a8)\
	call(socket,function(),a1,a2,a3,a5,a5,a6,a7,a8,tag)
#define PRE_CALL_9(function,a1,a2,a3,a4,a5,a6,a7,a8,a9)\
	call(socket,function(),a1,a2,a3,a4,a5,a6,a7,a8,a9,tag)
#define PRE_CALL_10(function,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10)\
	call(socket,function(),a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,tag)

#define PRE_SEND_0(TCMD)\
	TCMD cmd;\
	send(socket,&cmd);

#define PRE_SEND_1(TCMD,t1)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	send(socket,&cmd);

#define PRE_SEND_2(TCMD,t1,t2)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	send(socket,&cmd);

#define PRE_SEND_3(TCMD,t1,t2,t3)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	send(socket,&cmd);

#define PRE_SEND_4(TCMD,t1,t2,t3,t4)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	cmd.t4 = t4;\
	send(socket,&cmd);

#define PRE_SEND_5(TCMD,t1,t2,t3,t4,t5)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	cmd.t4 = t4;\
	cmd.t5 = t5;\
	send(socket,&cmd);

#define PRE_SEND_6(TCMD,t1,t2,t3,t4,t5,t6)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	cmd.t4 = t4;\
	cmd.t5 = t5;\
	cmd.t6 = t6;\
	send(socket,&cmd);

#define PRE_SEND_7(TCMD,t1,t2,t3,t4,t5,t6,t7)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	cmd.t4 = t4;\
	cmd.t5 = t5;\
	cmd.t6 = t6;\
	cmd.t7 = t7;\
	send(socket,&cmd);

#define PRE_SEND_8(TCMD,t1,t2,t3,t4,t5,t6,t7,t8)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	cmd.t4 = t4;\
	cmd.t5 = t5;\
	cmd.t6 = t6;\
	cmd.t7 = t7;\
	cmd.t8 = t8;\
	send(socket,&cmd);
#define PRE_SEND_9(TCMD,t1,t2,t3,t4,t5,t6,t7,t8,t9)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	cmd.t4 = t4;\
	cmd.t5 = t5;\
	cmd.t6 = t6;\
	cmd.t7 = t7;\
	cmd.t8 = t8;\
	cmd.t9 = t9;\
	send(socket,&cmd);
#define PRE_SEND_10(TCMD,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	cmd.t4 = t4;\
	cmd.t5 = t5;\
	cmd.t6 = t6;\
	cmd.t7 = t7;\
	cmd.t8 = t8;\
	cmd.t9 = t9;\
	cmd.t10 = t10;\
	send(socket,&cmd);

#define PRE_CMD_0(TCMD)\
	TCMD cmd;\
	send(socket,&cmd,sizeof(TCMD)); 

#define PRE_CMD_1(TCMD,t1)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	send(socket,&cmd,sizeof(TCMD));

#define PRE_CMD_2(TCMD,t1,t2)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	send(socket,&cmd,sizeof(TCMD));

#define PRE_CMD_3(TCMD,t1,t2,t3)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	send(socket,&cmd,sizeof(TCMD));

#define PRE_CMD_4(TCMD,t1,t2,t3,t4)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	cmd.t4 = t4;\
	send(socket,&cmd,sizeof(TCMD));

#define PRE_CMD_5(TCMD,t1,t2,t3,t4,t5)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	cmd.t4 = t4;\
	cmd.t5 = t5;\
	send(socket,&cmd,sizeof(TCMD));

#define PRE_CMD_6(TCMD,t1,t2,t3,t4,t5,t6)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	cmd.t4 = t4;\
	cmd.t5 = t5;\
	cmd.t6 = t6;\
	send(socket,&cmd,sizeof(TCMD));

#define PRE_CMD_7(TCMD,t1,t2,t3,t4,t5,t6,t7)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	cmd.t4 = t4;\
	cmd.t5 = t5;\
	cmd.t6 = t6;\
	cmd.t7 = t7;\
	send(socket,&cmd,sizeof(TCMD));

#define PRE_CMD_8(TCMD,t1,t2,t3,t4,t5,t6,t7,t8)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	cmd.t4 = t4;\
	cmd.t5 = t5;\
	cmd.t6 = t6;\
	cmd.t7 = t7;\
	cmd.t8 = t8;\
	send(socket,&cmd，sizeof(TCMD));
#define PRE_CMD_9(TCMD,t1,t2,t3,t4,t5,t6,t7,t8,t9)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	cmd.t4 = t4;\
	cmd.t5 = t5;\
	cmd.t6 = t6;\
	cmd.t7 = t7;\
	cmd.t8 = t8;\
	cmd.t9 = t9;\
	send(socket,&cmd,sizeof(TCMD));
#define PRE_CMD_10(TCMD,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10)\
	TCMD  cmd;\
	cmd.t1 = t1;\
	cmd.t2 = t2;\
	cmd.t3 = t3;\
	cmd.t4 = t4;\
	cmd.t5 = t5;\
	cmd.t6 = t6;\
	cmd.t7 = t7;\
	cmd.t8 = t8;\
	cmd.t9 = t9;\
	cmd.t10 = t10;\
	send(socket,&cmd,sizeof(TCMD));
