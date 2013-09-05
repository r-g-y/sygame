#pragma once
#include "sys.h"
#include "package.h"
#include "deque"
/**
 * 当前的tcp 连接 完成msg的解析 和基本的消息接受发送 
 * conn->sendObject(&object); // object 可以很大 内部处理为分块 等待发送
 */
class Connection{
public:
	DYNAMIC_API Connection()
	{
		tag = NULL_TAG;
	}
	/**
	 * 发送序列化的对象
	 */
	DYNAMIC_API bool sendObject();

	/**
	 * 发送对齐的消息
	 */
	DYNAMIC_API bool sendPackCmd(void * cmd,unsigned int size);
	
	int tag; // 发送报文的处理方式
	enum{
		NULL_TAG = 0, // 原始报文
		ZLIB_TAG = 1 << 0, // 压缩
		DES_TAG = 1 << 1, // DES加密
		HASH_TAG = 1 << 2, // hash 校验
		MAX_TAG = 1 << 3, // 允许不间断大包文发送 不设定该标识的话 当数据大于MAX_PACK_SIZE时 会自动分割,效率会变慢
	};
	/**
	 * 设置处理方式
	 */
	DYNAMIC_API void setMethod(int method);
	/**
	 * 处理接受
	 */
	DYNAMIC_API void doRecv();

	/**
	 * 尝试写入数据 主要是增加
	 */
	DYNAMIC_API void startWrite(){}

	/**
	 * 尝试停止写入
	 */
	DYNAMIC_API void stopWrite();

	stPackContent nowPackContent; // 当前pack 的内容
	sys::Socket socket;// 系统socket	

	std::deque<stPackContent> sendBuffer;
	/**
	 * 真正意义的向网络层写数据
	 */
	DYNAMIC_API void flushSendBuffer();
	/**
	 * 将自身设置为无效
	 */
	DYNAMIC_API void setInvalid();
	/**
	 * 设置操作句柄
	 */
	DYNAMIC_API void setHandle(const sys::Socket& socket){}

};