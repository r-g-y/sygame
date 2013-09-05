#pragma once
#include "sys.h"
#include "vector"
#include "refmemory.h"
/**
 * 一个内存节点
 */
class MemoryNode:public stRefMemory{
public:
	std::vector<char> content;// 内容
	MemoryNode *next; // 下一个空闲块
	DYNAMIC_API MemoryNode()
	{
		next = NULL;
	}
};