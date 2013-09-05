#pragma once
#include "sys.h"
#include "deque"
#include "queue"
/**
 * 测试一个的高效的MQ
 * 无挂起，无死锁
 * */
class MQ{
public:
	/*
	 * 增加一一个消息池
	 **/
	void add(const void *cmd,int len);
	
	/**
	 * 触发取消息
	 */
	void pick();
	
protected:
	virtual void done(void *cmd,int len) = 0;

	std::queue<std::vector<char> > _ts;
	std::queue<std::vector<char> > _temp_ts;
	sys::Mutex _mutex;
};
