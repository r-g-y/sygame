/**
* email:ji.jin.dragon@gmail.com
* Author jijinlong
* Data 2011.10.14~~~
* All Rights Resvered
***/
#define TEMP_FAILURE_RETRY 
#include "socket.h"
#include "mcenter.h"
#include "net.h"
#include <sstream>
#define REV_BUFFER_MAX_SIZE 1024
/**
 * 接受指定长度的消息
 * 没达到长度 继续接受
 **/
RecvMessage::RecvMessage()
{
	ok = false;
	keyPos = 0;
	preSize = 0;
	messageSize = 0;
	content.resize(256) ;
}
/**
 *设置待接受消息的长度
 * \param size 消息长度
 **/
void RecvMessage::setSize(DWORD size)
{
        if (content.size() < size)
        	content.resize(size);
        messageSize = size;
}
	
/**
 * 从缓存大小为size 缓存指针为ptr 中尝试读取messageSize 的大小
 * \param ptr 当前指针 out 偏移后的指针
 * \param size 当前缓存大小 out 剩余大小
 **/
void RecvMessage::append(char **ptr, int& size)
{ 
        char *tmp = *ptr;
	/*
	 * 如果缓存区中大小 大于 待接受大小 拷贝待接受大小
	 * 否则 拷贝缓存区中的大小
	 **/
        int copySize = (size >= messageSize - preSize)?messageSize - preSize : size;
        bcopy(tmp,&content[preSize],copySize);
        preSize += copySize;
        if (preSize == messageSize)
            ok = true;
        (*ptr) += copySize;
        size -= copySize;
}
/**
 *清除当前缓存
 **/
void RecvMessage::clear()
{
        ok = false;
	preSize = 0;
	messageSize = 0;
	keyPos = 0;    
}	
/** 
 *解码
 **/
void RecvMessage::decrypt()
{
        if (!keys.size()) return;
	for (int i = 0; i < messageSize; i++)
	{
		keyPos = i % keys.size();
		content[i] ^= keys[keyPos];	
	}
}
/**
 * 设置密钥
 * \param akeys 密钥
 */
void RecvMessage::setKey(std::vector<char>& akeys)
{
	keys = akeys;
}  


/**
* 发送当前缓存池中内容
* \param sock socket 
* \return -1 失败 1成功
**/
int ByteContent::send(Socket *sock)
{
	if (isOk()) return 1;
	int res = sock->send(&content[offset],((unsigned int)content.size())-offset);
	if (res == -1) return -1;
	offset += res;  	
	return 1;
}
/**
 * 当前缓存发送状态
 *	\return 成功与否
 **/
bool ByteContent::isOk()
{
	return offset >= content.size();
}
/**
 * 加密当前内容
 * \param content 内容
 * \param messageSize 内容长度
 **/
void ByteContent::encrypt(char *content,unsigned int messageSize)
{
	if (!keys.size()) return;
	for (unsigned int i = 0; i < messageSize; i++)
	{
		keyPos = i % keys.size();
		content[i] ^= keys[keyPos];	
	}
}
/**
 * 设置密钥
 * \param akeys 密钥
 **/
void ByteContent::setKey(std::vector<char>& akeys)
{
	this->keys = akeys;
}
/**
 * 使用端口初始化 绑定端口 并监听
 * \param port 端口
 * \return 是否成功
 **/
int Socket::bindAndListen(unsigned short port)
{
	type = NODE_SERVER;
#ifdef __LINUX__
	int reuse = 1;
	setsockopt(_socket.getHandle(),SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
#endif
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	bzero(&(addr.sin_zero),8);
	_socket.bind((struct sockaddr *)&addr, sizeof(addr));
	printf("--server bind port:%u\n---",port);
	setnonblocking();
	_socket.listen(4096);
	return 0;
}
/**
 * 连接服务器 ip ,port 初始化
 * \param ip 32位地址
 * \param port 端口
 * \param 0 表示成功 
 * **/
int Socket::connect(const char *ip,WORD port)
{
	type = NODE_CLIENT;
	struct hostent *host;
	if((host=gethostbyname(ip))==NULL){
#if __WINDOWS__
		int err = WSAGetLastError();
#endif
		_socket.close();
		return -1;
	}
	struct sockaddr_in serv_addr;
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(port);
	serv_addr.sin_addr=*((struct in_addr *)host->h_addr);
	//serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(serv_addr.sin_zero),8);
	if (_socket.connect((struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) == -1) 	
	{
		_socket.close();
		return -1;		
	}
	valid = true;
	return 0;
}
#ifdef __USE_EPOLL__
void Socket::SETEPOLL(int epfd,void *ptr)
{
	 struct epoll_event ev;
	 ev.events = EPOLLOUT|EPOLLIN|EPOLLERR|EPOLLPRI;
	 ev.data.ptr = ptr;
	 epoll_ctl(epfd,EPOLL_CTL_ADD,_socket.getHandle(),&ev);
}

void Socket::DELEPOLL(int epfd)
{
	struct epoll_event ev;
	ev.events = EPOLLOUT|EPOLLIN|EPOLLERR|EPOLLPRI;
	ev.data.ptr = NULL;
	epoll_ctl(epfd,EPOLL_CTL_DEL,_socket.getHandle(),&ev);
}

#endif
/**
 * 	接受二进制串
 * \param cmd 消息
 * \param len 消息长度
 * \return 消息的长度
 **/
int Socket::recv(void *cmd,unsigned int len)
{
    if (!checkValid()) return -1;
	return _socket.recv(cmd,len);
}

/**
 * 	发送二进制串
 * \param cmd 消息
 * \param len 消息长度
 * \return 发送消息的长度
 **/
int Socket::send(void *cmd,unsigned int len)
{
    if (!checkValid()) return -1;
	return _socket.send(cmd,len);
}
/**
 * 函数使用
 */
int Socket::doCmd(DealCmdDelegate  *socket,void *cmd,unsigned int len)
{
	// 这里解析出CmdObject 然后调用parseCmdObject
	return 1;
}
/**
 * 发送消息 加上header
 * \param cmd 二进制数据
 * \param len 长度	 
 * \return 发送的后剩余的长度	 
 */
int Socket::sendPacket(void *cmd,unsigned int len)
{
    if (!checkValid()) return -1;
	std::vector<char>buf;
	BULID_OBJECT_CMD(buf,cmd,len);
	ByteContent bc(&buf[0],buf.size(),stHeader::OBJECT_MSG);
	return send(&buf[0],buf.size());
	contents.push_back(bc);
	wakeupSend(); // 尝试唤起send
	return 0;
}
/**
 *
 * 发送消息 加上header
 * \param 发送对齐数据
 * \param cmd 二进制数据
 * \param len 长度
 * \return 发送的后剩余的长度	 
 */
int Socket::sendCmd(void *cmd,unsigned int len)
{
	if (!checkValid()) return -1;
	std::vector<char>temp;
	temp.resize(len);
	bcopy(cmd,&temp[0],len);
	std::vector<char>buf;
	BULID_PACK_CMD(buf,cmd,len);
	int res = send(&buf[0],buf.size());
	return res;
	ByteContent bc(&buf[0],buf.size(),stHeader::PACK_MSG);
	contents.push_back(bc);
	wakeupSend(); // 尝试唤起send
	return 0;
}
/**
 * 发送object
 */
int Socket::sendObject(CmdObject *object)
{
    if (!checkValid()) return -1;
    cmd::Stream ss = object->toStream();
	//return sendPacket(ss.content(),ss.size());
	ByteContent bc(ss.content(),ss.size(),stHeader::OBJECT_MSG);
    contents.push_back(bc);
	wakeupSend(); // 尝试唤起send
    return 0;
}
/**
 * 获取socket 的句柄
 * \return _socket 
 */
sys::Socket& Socket::getHandle(){return _socket;};
/**
 * 接受消息 形成记录 并转发处理
 * \param father socket
 * \param buf 消息缓存
 * \param leftsize 消息缓存大小
 * */
int Socket::done(DealCmdDelegate *delegate,char *buf,int leftSize)
{
	if (!checkValid()) return -1;
	if (leftSize == -1 || leftSize == 0) {
		setInvalid();
		return 0;
	}
	char *nowPtr = buf; 
	while (leftSize)
	{
		if (!Msg.header.ok)
		{
			Msg.header.setSize(sizeof(stHeader));
			Msg.header.append(&nowPtr,leftSize);
		}
		if (Msg.header.ok)
		{
			stHeader header = *(stHeader*) &(Msg.header.content[0]);
			stPackHeader *tag = (stPackHeader*)&(Msg.header.content[0]);
			Msg.body.setSize(header.bodySize);
			Msg.body.append(&nowPtr,leftSize);
			if (Msg.body.ok && header.bodySize)
			{
				void *nullCmd = &Msg.body.content[0];
				if (delegate && header.bodySize && header.msgType == stHeader::OBJECT_MSG)
					delegate->doObjectCmd(this,nullCmd,Msg.body.messageSize);
				if (delegate && header.bodySize && header.msgType == stHeader::PACK_MSG)
					delegate->doPackCmd(this,nullCmd,Msg.body.messageSize);
				Msg.header.clear();
				Msg.body.clear();
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	return 1;
}
/**
 *read 有效
 * <code>实现vTask::done </code>
 * 处理read 操作
 * \return  void
 * **/
int Socket::done(DealCmdDelegate *delegate)
{
    	if (!checkValid()) return -1;
	char buf[REV_BUFFER_MAX_SIZE]={0};
	int leftSize = recv(buf,REV_BUFFER_MAX_SIZE);
	if (leftSize == -1) 
	{
		setInvalid();
		return -1;
	}
	return done(delegate,buf,leftSize);
}
/**
 * send 操作有效 处理send
 * <code>实现vTask::send </code>
 * \param tap 允许处理的时间
 * \return  void
 * */
int Socket::send(unsigned int tap)
{
    if (!checkValid()) return 0;
	sys::stTimeValue curTime = sys::sTime::getNowTime();	
	if (!contents.empty())
		sys::sTime::wait(tap);
	while (!contents.empty() /*
							 && curTime + tap >= sys::sTime::getNowTime()*/)
	{
		ByteContent &content = contents.front();
		if (-1 == content.send(this))
		{
			setInvalid();
			break;
		}
		if (content.isOk())
		{
			content.remove();
			contents.pop_front();
		}
	}
	return (int)contents.size();
}
/**
 * send 操作将数据copy进buffer 里
 * \param buffer 缓存
 * \param size 缓存空间
 * */
int Socket::copySendBuffer(void *buffer,unsigned int size)
{
	if (!checkValid()) return 0;
	unsigned int leftSize = size;
	unsigned int offset = 0;
	sys::stTimeValue nowTime = sys::sTime::getNowTime(),
		oldTime = sys::sTime::getNowTime();	
	unsigned int tap = 100;
	while (!contents.empty() && oldTime + tap >= nowTime)
	{
		ByteContent &content = contents.front();
		if (leftSize >= content.content.size())
		{
			bcopy(&content.content[0],((char*)buffer) + offset,content.content.size());
			leftSize = leftSize - content.content.size();
			offset += content.content.size();
		}	
		content.remove();
		contents.pop_front();
		nowTime = sys::sTime::getNowTime();
	}
	return offset;
}
void Socket::setnonblocking()
{
#if __LINUX__ || __MAC__
	int opts;
	opts=fcntl(_socket.getHandle(),F_GETFL);
	if(opts<0)
	{
		return;
	}
	opts = opts|O_NONBLOCK;
	if(fcntl(_socket.getHandle(),F_SETFL,opts)<0)
	{
		return;
	} 
#else
	unsigned long ul = 1;
    ioctlsocket(_socket.getHandle(), FIONBIO, (unsigned long*)&ul);
#endif
}


Socket * Socket::makeSocket(int _socket)
{
#if __LINUX__ || __MAC__
	socklen_t clilen = 1024;
#else
	int clilen = 1024;
#endif
	struct sockaddr_in addr;
	if (-1 == _socket) return NULL;
	Socket * socket = new Socket;
	int sockfd = ::accept(_socket,(struct sockaddr*)(&addr),&clilen);
	if (-1 != sockfd)
	{
		socket->setHandle(sockfd);
		socket->setnonblocking();
		socket->type = NODE_NULL;
		socket->createTime = sys::sTime::getNowTime();
		printf("获取一个连接 %x %llu\n",socket,socket->createTime);
	}
	else
	{
		delete socket;
		socket = NULL;
	}
	return socket;
}



void Socket::setInvalidDelay(DWORD delayTime)
{
	//if (delay) return;
	this->delay = delayTime;
	this->startDelayTime = sys::sTime::getNowTime();
}

void Socket::clearValidDelay()
{
	this->delay = 0;
	this->startDelayTime.second = 0;
	this->startDelayTime.msecond = 0;
}

void Socket::setInvalidTimeOut()
{
	sys::stTimeValue nowTime = sys::sTime::getNowTime();
	sys::stTimeValue delayTime = startDelayTime + delay;
	if (delay &&  delayTime < nowTime)
	{
		setInvalid();
	//	printf("...................................................因为超时 结束当前socket:%u\n",delay);
		this->wakeupSend();
		_socket.close();
	}
}

std::string Socket::getUniqueName()
{
	std::stringstream ss;
	ss<<ip << ":" << port;
	return ss.str();
}