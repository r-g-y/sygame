#include "mcenter.h"
#include "cmd_serialize.h"
#include "sys.h"
#include "remotefunction.h"
#include "socket.h"
 void CmdObject::parsecmd(void *cmd,int cmdLen)
{
	if (cmdLen < sizeof(int)) return;	
	//TODO __msg__id__ = *(int*) cmd;
	fromStream(((char*)cmd + sizeof(int)),cmdLen - sizeof(int));	
}
void CmdObject::clear()
{
	clearContent();
}
cmd::Stream CmdObject::toStream()
{
	cmd::Stream ss;
	ss.append(&__msg__id__,sizeof(__msg__id__));
	__to_msg__record__(cmd::ADD,ss);
	ss.reset();
	return ss;	
}
MessageCenter::MessageCenter()
{
	functions.resize(1024);
	_mutex = new sys::Mutex();
}
bool MessageCenter::bind(unsigned int id,FuncCaller* func,CmdObject *cmd )
{
	_mutex->lock();
	if (cmd && checkFunction(func,cmd))
	{
		_mutex->unlock();
		return false;
	}
	if (id >= functions.size())
		functions.resize(id+5);
	functions[id].push_back(func);
	if (id >= objects.size())
	{
		objects.resize(id+5);
	} 
	else if (cmd)
	{
		if (!objects[id])
		{
			cmd->__msg__id__ = id;
		}
		else
		{
			_mutex->unlock();
			return false;
		}
	}
	objects[id] = cmd;
	_mutex->unlock();
	return true;
}

bool MessageCenter::handle(CallDelegate *delegate,CmdObject *object,Socket *socket)
{
	if (!object) return false;
	unsigned int id = object->__msg__id__;
	if (id < functions.size())
	{
		std::list<FuncCaller*> &funcs = functions[id];
		for (FUNC_ITER iter = funcs.begin(); iter != funcs.end();++iter)
		{
			FuncCaller* func = *iter;
			
			_mutex->lock();
			func->call(delegate,socket,object);
			_mutex->unlock();
		}
		return true;
	}
	return false;
}

bool MessageCenter::handle(CallDelegate *delegate,CmdObject *object)
{
	if (!object) return false;
	int id = object->__msg__id__;
	if (id < functions.size())
	{
		std::list<FuncCaller*> &funcs = functions[id];
		for (FUNC_ITER iter = funcs.begin(); iter != funcs.end();++iter)
		{
			FuncCaller* func = *iter;
			
			_mutex->lock();
			func->call(delegate,object);
			_mutex->unlock();
		}
		return true;
	}
	return false;
}

bool MessageCenter::checkFunction(FuncCaller* func, CmdObject *object)
{
	if (!object) return  false;
	unsigned int id = object->__msg__id__;
	if (id < functions.size())
	{
		std::list<FuncCaller*> &funcs = functions[id];
		for (FUNC_ITER iter = funcs.begin(); iter != funcs.end();++iter)
		{
			if (func == *iter) return true;
		}
	}
	return false;
}


bool MessageCenter::handle(CallDelegate *delegate,void *cmd,int cmdLen,Socket *socket,eRemoteCallType callType)
{
	_mutex->lock();
	if (!delegate) 
	{
		_mutex->unlock();	
		return false;
	}
	if (cmdLen < sizeof(int))
	{
		_mutex->unlock();	
		return false;
	}
	unsigned int *id_pointer = (unsigned int*) cmd;
	unsigned int id = *id_pointer;
	CmdObject *object = getObject(id);
	if (object && id < functions.size() && object->__msg__id__ == id)
	{
		object->parsecmd(cmd,cmdLen);
		std::list<FuncCaller*> &funcs = functions[id];
	//	int i = 0;
		for (FUNC_ITER iter = funcs.begin(); iter != funcs.end();++iter)
		{
			FuncCaller* func = *iter;
 			if (func)
   			{
				func->call(delegate,socket,object,callType);
			}
    		else
    		{
        			printf("消息号 %d 无函数\n",id);
    		}
		}
		object->clear();
	}
	else if (!object && id < functions.size())
	{
		std::list<FuncCaller*> &funcs = functions[id];
		for (FUNC_ITER iter = funcs.begin(); iter != funcs.end();++iter)
		{
			FuncCaller* func = *iter;
			if (func)
			{
   				func->call(delegate,socket,cmdLen,cmd,callType);
			}
			else
			{
    				printf("消息号 %d 无函数\n",id);
			}
		}
	} 
	else 
	{
		_mutex->unlock();	
		return false;
	}
	_mutex->unlock();
	return true;
}

void MessageCenter::destroy()
{
	for (OBJECT_ITER iter= objects.begin(); iter != objects.end();++iter)
	{
		if (*iter)
			delete *iter;
	}
	for (FUNCS_ITER iter = functions.begin(); iter != functions.end();++iter)
	{
		for (FUNC_ITER pos = iter->begin(); pos != iter->end(); ++pos)
		{
			if (*pos) delete *pos;
		}
	}
	objects.clear();
	functions.clear();
	if (_mutex)
	{
		delete _mutex;
	}
	_mutex = NULL;
}

/**
 * 设置函数的名字
 * \param name 函数名字
 * \param func 函数调用器
 * \param valid 是否有效
 */
void MessageCenter::setFuncName(std::string name,FuncCaller *func,bool valid,std::string validIp)
{
	NAME_FUNCS_ITER iter = namefuncs.find(name);
	if (iter != namefuncs.end())
	{
		if (iter->second && func == NULL)
		{
			iter->second->valid = valid;
			iter->second->validip = validIp;
			return;
		}	
	}
	if (func)
	{
		func->valid = valid;
		func->validip = validIp;
		namefuncs[name] = func;
	}
}
bool FuncCaller::checkInValidIp(Socket *socket)
{
	if (validip != "" && validip != socket->getIp())
	{
		return true;
	}
	return false;
}