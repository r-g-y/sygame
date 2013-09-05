#pragma once
#include "libtag.h"
#include "cmd_serialize.h"
/**
 * 消息体,携带了消息唯一标示
 */
class CmdObject:public cmd::Object{
public:
	unsigned int __msg__id__; // 消息号
	/**
	 * 处理内容
	 * \param cmd 内容
	 * \param cmdLen 内容长度
	 */
	DYNAMIC_API virtual void parsecmd(void *cmd,int cmdLen);
	/**
	 * 清除消息结构体内容
	 * 消息结构体 中的内容是暂存的
	 */
	DYNAMIC_API virtual void clear();
	/**
	 * 将消息转化为字符串
	 */
	DYNAMIC_API cmd::Stream toStream();
	/**
	 * 使用唯一id 标示消息号
	 * \param id 唯一
	 */
	DYNAMIC_API CmdObject(unsigned int id):__msg__id__(id){}
};