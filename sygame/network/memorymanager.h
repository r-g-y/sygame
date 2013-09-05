#pragma once
#include "quickidmanager.h"
#include "memorynode.h"
/**
 * 内存管理器
 * 根据当前内存块大小 快速检索出所需的内存,内存大小是2^n次方 最大支持32 次方的检索
 * 快速分配
 */
class MemoryManager{
public:
	/**
	 * 当useLocal 为本地时,内存管理只为一个对象服务
	 * \param useLocal 本地参数
	 */
	DYNAMIC_API MemoryManager(bool useLocal);
	DYNAMIC_API MemoryManager();
	DYNAMIC_API static MemoryManager & getMe()
	{
		static MemoryManager me(false);
		return me;
	}
	/**
	 * 分配出当前内存
	 * \param size 大小
	 * \return 内存引用信息
	 */
	DYNAMIC_API MemoryNode* alloc(unsigned int size);
	
	/**
	 * 释放当前内存引用
	 * \param memory 内存引用
	 */
	DYNAMIC_API void dealloc(stRefMemory& memory);
	/**
	 * 内存指针
	 * \param node 对象的内存块
	 **/
	DYNAMIC_API void dealloc(MemoryNode* node);
	/**
	 * 根据引用查找指定的块
	 * \param memory 引用
	 * \return 内存块
	 */
	DYNAMIC_API MemoryNode *findNode(stRefMemory& memory);

	DYNAMIC_API unsigned int blockSize()
	{
		return quicknodes.getSize();
	}
private:
	QuickIdManager quicknodes;
	std::vector<MemoryNode*> nodes;
	/**
	 * 分配一些内存
	 * \param size 块的大小
	 * \return 块的首个指针
	 */
	MemoryNode *allocNodes(unsigned int size);
	/**
	 * 根据大小获取检索块
	 * \param size 块大小
	 * \return 引索号
	 */
	unsigned int getIndexBySize(unsigned int size);

	bool useLocal; // 是否是由单一的内存分配
	unsigned int nodeIndex;
};

#define theMemoryManager MemoryManager::getMe()