#include "epollpool.h"
#include "sys.h"
#include "singleton.h"
#include "queue"
#include "deque"
typedef std::list<Socket*>::iterator SocketIter;
#ifdef __USE_EPOLL__
typedef std::vector<struct epoll_event> epollfdContainer;
#endif
/**
 * 监听者线程 监听数量 受最大句柄数量影响
 **/
class ListenerThread:public sys::Thread{
public:
	ListenerThread(int handle,EpollThreadConnectionPoll *pool)
	{
		this->handle = handle;
		this->pool = pool;
#ifdef __USE_EPOLL__ 
		this->epfd = epoll_create(1);
		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.ptr = NULL;
		epoll_ctl(epfd,EPOLL_CTL_ADD,handle,&ev);
#endif
	}
	void run();
	void doMakeSocket();
private:
	int handle;
	EpollThreadConnectionPoll *pool;
	int epfd;
};
/**
 * 工作者队列
 **/
class WorkQueue{
public:
	void addSocket(Socket *socket)
	{
		_mutex.lock();
		_works.push(socket);
		workCount++;
		_mutex.unlock();
	}

	int workCount;	
	void check_works()
	{
		_mutex.lock();
		while(!_works.empty())
		{
			_temp_works.push(_works.front());
			_works.pop();
		}
		workCount = 0;
		_mutex.unlock();
		while(!_temp_works.empty())
		{
			_addSocket(_temp_works.front());
			_temp_works.pop();	
		}
	}
	virtual void _addSocket(Socket *socket) = 0;
private:
	std::queue<Socket*,std::deque<Socket*> > _works;
	std::queue<Socket*,std::deque<Socket*> > _temp_works;
	sys::Mutex _mutex;
};
/**
 * 回收者线程
 **/
class RecycleThread:public sys::Thread,public WorkQueue{
public:
	void _addSocket(Socket *socket)
	{
		tasks.push_back(socket);
	}
	void run();
private:
	std::list<Socket*> tasks;	
};

class WorkThread:public sys::Thread,public WorkQueue{
public:
	void _addSocket(Socket *socket)
	{
#ifdef __USE_EPOLL__
		tasks.push_back(socket);
		socket->SETEPOLL(epfd,socket);
		_task_count = tasks.size();
		events.resize(_task_count + 16);
#endif
	}
	WorkThread(EpollThreadConnectionPoll *pool)
	{
		handle = pool->handle;
		this->pool = pool;
#ifdef __USE_EPOLL__ 
		epfd = epoll_create(256);
		events.resize(256);
#endif

	}
	size_t size(){
		return workCount + tasks.size();
	}
	void run();
	void check();
	void action();
private:
	int epfd;
	int handle;
	std::list<Socket*> tasks;
	EpollThreadConnectionPoll *pool;
#ifdef __USE_EPOLL__
	epollfdContainer events;
#endif
	size_t _task_count;
};
void EpollThreadConnectionPoll::start(WORD port)
{
	socket.setHandle(::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP));
	socket.bindAndListen(port);	
	for (int i = 0; i < 1;i++)
	{
		WorkThread * work = new WorkThread(this);
		_workThreads.push_back(work);	
		work->start();
	}	
	listenThread = new ListenerThread(socket.getHandle().getHandle(),this);
	listenThread->start();
	recycleThread = new RecycleThread();
	recycleThread->start();
}

bool EpollThreadConnectionPoll::addSocket(Socket *socket)
{
	WorkThread * min = NULL;
	for (int i = 0; i < _workThreads.size();i++ )	
	{
		WorkThread * work = _workThreads[i];
		if (work)
		{
			if (NULL == min || min->size() > work->size())
			{
				min = work;
			}
		}
	}
	if (min)
	{
		min->addSocket(socket);
		printf("addSocket %x 当前数量:%lu type:%u\n",socket,min->size(),socket->type);
	}
	return true;
}

void WorkThread::run()
{
	sys::stTimeValue startTime = sys::sTime::getNowTime();
	while (isActive())
	{
		check_works();
		sys::stTimeValue nowTime = sys::sTime::getNowTime();
		sys::sTime::wait(500);
		if (startTime + 3 < nowTime)
		{	
			check();
			action();
			startTime = nowTime;
		}
	}
}
void WorkThread::check()
{
	SocketIter iter;
	for ( iter = tasks.begin(); iter != tasks.end();)
	{
		Socket * task = (Socket*)*iter;
		if (!task)
		{
			iter = tasks.erase(iter);
			_task_count --;
			continue;
		}	
		task->setInvalidTimeOut();
		if (!task->checkValid())
		{
			iter = tasks.erase(iter);
			_task_count --;
			task->BeReleased();
#ifdef _USE_EPOLL__
			task->DELEPOLL(epfd);
#endif
			task->getHandle().close();
			if (pool)
				pool->handleClose(task);
			if (task->type == Socket::NODE_CLIENT)
       		{
       		}
    		else
       		{
				pool->delSocket(task);  
        	}
			printf("删除task:%x 创建时间%llu 还剩下:%lu\n",task,task->createTime, size());
			continue;
		}
		++iter;
	}

}

void WorkThread::action()
{
#ifdef __USE_EPOLL__ 
	int nfds = epoll_wait(epfd,&events[0],_task_count,0);
	for (int i = 0; i < nfds;++i)
	{
		struct epoll_event & ev = events[i];
		if (!ev.data.ptr)
		{
			events[i].events = 0;
			continue;
		}
		Socket *task = (Socket*) ev.data.ptr;
		if ((ev.events & EPOLLIN) && task->checkValid())
		{
			if (pool)
				pool->handleRecv(task);
		}
		else if ((ev.events&EPOLLOUT) && task->checkValid())
		{
			task->send(300);
		}
		else if (ev.events & (EPOLLERR|EPOLLPRI))
		{
			task->setInvalid();
		}
		events[i].events = 0;
	}
#endif

}


void ListenerThread::run()
{
	while(isActive())
	{
		sys::sTime::wait(500);
		doMakeSocket();
	}	
}
void ListenerThread::doMakeSocket()
{
#ifdef __USE_EPOLL__ 
	struct epoll_event events[1];
	int nfds = epoll_wait(epfd,events,1,1);
	if(1== nfds)
	{
		struct epoll_event & ev = events[0];
		if (ev.events & EPOLLIN && !ev.data.ptr)
		{
			Socket *vtask= Socket::makeSocket(handle);
			if (vtask)
			{
				pool->addSocket(vtask);
				printf("do Make Socket %x\n", vtask);
			}
		}
	}
#endif
}
/**
 * 删除socket
 * \param socket 
 */
void EpollThreadConnectionPoll::delSocket(Socket *socket)
{
	if (recycleThread)
		recycleThread->addSocket(socket);	
}


void RecycleThread::run()
{
	while(isActive())
	{
		check_works();
		sys::sTime::wait(500);
		for (SocketIter iter = tasks.begin(); iter != tasks.end(); ++iter)
		{
			if (*iter) delete *iter;
			printf("实际删除指针:%x\n",*iter);
		}
		tasks.clear();
	}
}


void EpollThreadConnectionPoll::release()
{
	for (WORK_THREADS_ITER iter = _workThreads.begin(); iter != _workThreads.end();++iter)
	{
		if (*iter) delete *iter;
	}
	_workThreads.clear();
}

void EpollThreadConnectionPoll::handleClose(Socket *socket)
{
	if (closedelegate)
	closedelegate->handleClose(socket);
}

void EpollThreadConnectionPoll::handleRecv(Socket *socket)
{
	if (delegate)
	socket->done(delegate);	
}
