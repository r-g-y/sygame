#include "kqueuepool.h"

class ActionQueueThread:public sys::Thread{
public:
	void run()
	{
		while(isActive())
		{
#ifdef __MAC__
#define MAX_EVENT_COUNT 1023
            struct kevent events[MAX_EVENT_COUNT];
			int nevents = kevent(pool->kq, NULL, 0, events, MAX_EVENT_COUNT, NULL);
			if (nevents == -1)
			{
				printf("kevent failed!\n");
				continue;
			}
			for (int i = 0; i < nevents; i++)
			{
				int sock = events[i].ident;
				int data = events[i].data;
				short filter = events[i].filter;    
				if (sock == pool->handle)
					pool->innerAddSocket(data); // 增加n个socket
                Socket *socket = (Socket*)events[i].udata;
				if (!socket) continue;
				if (filter == EVFILT_WRITE)
				{	
					pool->handleWrite(socket);
				}else if (filter == EVFILT_READ)
				{
					pool->handleRecv(socket);
				}
			}
#endif
		}
	}
	KqueuePool *pool;
	ActionQueueThread(KqueuePool *pool):pool(pool){}
};
/**
* 初始化监听器
* \param handle 句柄 
*/
void KqueuePool::start(WORD port)
{
 // 绑定服务器端口
	socket.setHandle(::socket(PF_INET, SOCK_STREAM, 0));
	socket.bindAndListen(port);
	handle = socket.getHandle().getHandle();
#ifdef __MAC__
	kq = kqueue();
	struct kevent changes[1];
	EV_SET(&changes[0], kq, EVFILT_READ, EV_ADD, 0, 0, NULL);

	int ret = kevent(kq, changes, 1, NULL, 0, NULL); // 注册监听事件
	if (ret == -1)
		return ;
#endif
	action = new ActionQueueThread(this);
	action->start();
}
/**
 * 增加socket,加入到工作者线程里
 * */
bool KqueuePool::addSocket(Socket *socket)
{
	if (!socket) return false;
#ifdef __MAC__
	struct kevent changes[2];
	EV_SET(&changes[0], socket->getHandle().getHandle(), EVFILT_READ, EV_ADD, 0, 0, socket);
	EV_SET(&changes[1], socket->getHandle().getHandle(), EVFILT_WRITE, EV_ADD, 0, 0, socket);
	int ret = kevent(kq, changes, 2, NULL, 0, NULL); // 注册监听事件
	if (ret == -1)
		return false;
#endif
	
	return true;
}
/**
 * 删除socket
 * \param socket 待删除的socket
 */
void KqueuePool::delSocket(Socket *socket)
{
	delete socket;
}

/**
 * 增加n个socket
 */
void KqueuePool::innerAddSocket(int size)
{
	for (int i = 0; i < size; i++)
	{
		int client = accept(handle, NULL, NULL);
		if (client == -1)
		{
			continue;
		}
		Socket *socket = new Socket();
		socket->setHandle(client);
		if (addSocket(socket))
		{
			socket->setInvalid();
			delSocket(socket);
		}
	}
}

void KqueuePool::handleClose(Socket *socket)
{

}
void KqueuePool::handleRecv(Socket *socket)
{
	if (delegate)
		socket->done(delegate);	
}
void KqueuePool::handleWrite(Socket *socket)
{
	socket->send(300);
}