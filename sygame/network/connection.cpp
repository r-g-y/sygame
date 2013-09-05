#include "connection.h"

/**
 * 发送对齐的消息
 */
bool Connection::sendPackCmd(void * cmd,unsigned int size)
{
	stPackTag ptag;
	if (tag & ZLIB_TAG)
	{
		ptag.tag |= stPackTag::ZLIB_PACK;
	}
	if (tag & DES_TAG)
	{
		ptag.tag |= stPackTag::DES_PACK;
	}
	if (tag & HASH_TAG)
	{
		ptag.tag |= stPackTag::HASH_PACK;
	}
	if (tag & MAX_TAG)
	{
		ptag.tag |= stPackTag::MAX_PACK;
	}
	stPackContent pack;
	pack.encode(ptag,cmd,size);
	if (pack.hadChildPack())
	{
		sendBuffer.insert(sendBuffer.end(),pack.splitpacks.begin(),pack.splitpacks.end());
	}
	else
	{
		sendBuffer.push_back(pack);
	}
	startWrite(); // 尝试写消息
	return true;
}


/**
 * 设置处理方式
 */
void Connection::setMethod(int method)
{
	tag = method;
}

/**
 * 处理接受
 */
void Connection::doRecv()
{
	// 如果pack 未接受完 则return
	// 果果pack 接受完了 则直到消耗掉传入的数据
	char buffer[8026] = {'\0'};
	int size = socket.recv(buffer,8026);
	if (size == -1)
	{
		setInvalid();
		return ;
	}
	char *temp = (char*) buffer;
	int offset = 0;
	offset = nowPackContent.decode(temp,offset,size);
	while(offset >= size) // 到尾部了
	{
		if (nowPackContent.hadChildPack() && nowPackContent.isOk())
		{
			nowPackContent.reset();
		}
		else if (nowPackContent.isOk())
		{
			// todo 处理nowpack
			
			nowPackContent.reset();
		}
		else if (nowPackContent.isBad())
		{
			nowPackContent.reset();
		}
		offset = nowPackContent.decode(temp,offset,size);
	}
	if (offset > size)
	{
		// 出错了
	}
}

/**
 * 真正意义的向网络层写数据
 */
void Connection::flushSendBuffer()
{
	while (!sendBuffer.empty())
	{
		stPackContent &content = sendBuffer.front();
		void *cmd = content.getContentPointer();
		unsigned int cmdLen = content.getContentSize();
		int res = socket.send(cmd,cmdLen);
		if (res == -1)
		{
			setInvalid();
			return;
		}	
		content.contentStartPos += res;
		if (content.contentStartPos >= content.content.size())
		{
			sendBuffer.pop_front();
		}
	}
}

/**
 * 将自身设置为无效
 */
void Connection::setInvalid()
{
	socket.shutdown();
}