#include "memorymanager.h"

/**
 * 分配一些内存
 * \param size 块的大小
 * \return 块的首个指针
 */
MemoryNode *MemoryManager::allocNodes(unsigned int size)
{
	MemoryNode * header = new MemoryNode;
	for (int i = 0; i < 150; i++)
	{
		MemoryNode * node = new MemoryNode;
		node->content.resize(size);
		node->next = header;
		header = node;
	}
	return header;
}
/**
 * 分配出当前内存
 * \param size 大小
 * \return 内存引用信息
 */
MemoryNode* MemoryManager::alloc(unsigned int size)
{
	MemoryNode * node = NULL;
	if (size == 0) return node;
	unsigned int index = 1;
	if (false == useLocal)
	{
		index = getIndexBySize(size);
	}
	// 当前索引为index
	if (index >= 1 && index <= 32)
	{
		index --;
		if (index >= nodes.size())
		{
			nodes.resize(index+1);
		}
		if (NULL == nodes[index])
		{	
			MemoryNode * header = allocNodes(size);
			nodes[index] = header;
		}
		MemoryNode * node = nodes[index];
		if (node)
		{
			nodes[index] = node->next;
			if (size != node->content.size())
				node->content.resize(size);
			node->pointer = node;
			quicknodes.addObject(node);
			return node;
		}
	}
	return node;
}

/**
 * 释放当前内存引用
 */
void MemoryManager::dealloc(stRefMemory& memory)
{
	
	MemoryNode *node = (MemoryNode*)quicknodes.findObject(memory.refQuickId);
	if (!node) return;
	if (node && node->pointer == memory.pointer)
	{
		if (useLocal && nodes.size())
		{
			node->next = nodes[0];
			nodes[0] = node; // 回收块
			node->pointer = 0;
			node->refQuickId = -1;
		}
		else
		{
			unsigned int size = node->content.size();
			unsigned int index = getIndexBySize(size);
			// 当前索引为index
			if (index >= 1 && index <= 32)
			{
				index --;
				if (index >= nodes.size()) return;
				node->next = nodes[index];
				nodes[index] = node; // 回收块
				quicknodes.removeObject(node);
				node->pointer = 0;
				node->refQuickId = -1;
			}
		}
	}
}
/**
 * 内存指针
 * \param node 对象的内存块
 **/
 void MemoryManager::dealloc(MemoryNode* node)
 {
	if (!node) return;
	MemoryNode *newnode = (MemoryNode*)quicknodes.findObject(node->refQuickId);
	if (node && node == newnode)
	{
		if (useLocal)
		{
			node->next = nodes[0];
			nodes[0] = node; // 回收块
			node->pointer = 0;
			node->refQuickId = -1;
		}
		else
		{
			unsigned int size = node->content.size();
			unsigned int index = getIndexBySize(size);
			// 当前索引为index
			if (index >= 1 && index <= 32)
			{
				index --;
				if (index >= nodes.size()) return;
				node->next = nodes[index];
				nodes[index] = node; // 回收块
				quicknodes.removeObject(node);
				node->pointer = 0;
				node->refQuickId = -1;
			}
		}
	}
 }
/**
 * 根据引用查找指定的块
 * \param memory 引用
 * \return 内存块
 */
MemoryNode *MemoryManager::findNode(stRefMemory& memory)
{
	MemoryNode *node = (MemoryNode*)quicknodes.findObject(memory.refQuickId);
	if (node && node->pointer == memory.pointer && node->refQuickId != -1)
	{
		return node;
	}
	return NULL;
}
MemoryManager::MemoryManager(bool useLocal)
{
	if (useLocal)
	{
		nodes.resize(1);
	}
	else
		nodes.resize(32); // 初始32个引索
	this->useLocal = useLocal;
	nodeIndex = 0;
}
MemoryManager::MemoryManager()
{
	useLocal = true;
	nodes.resize(1);
	nodeIndex = 0;
}
/**
 * 根据大小获取检索块
 * \param size 块大小
 * \return 引索号
 */
unsigned int MemoryManager::getIndexBySize(unsigned int size)
{
	unsigned int index = 0;
	for (int i = 31;i >= 0;i--)
	{
		if (size & ((unsigned int) 1 << i))
		{
			index = i;
			break;
		}
	}
	if (size > ((unsigned int) 1 << index) && size < ((unsigned int) 1<< (index+1)))
		index++;
	return index;
}