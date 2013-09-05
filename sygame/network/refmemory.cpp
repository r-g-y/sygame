#include "refmemory.h"

/**
 * 设置快速唯一索引
 * \param uniqueQuickId 设置快速唯一索引
 */
void stRefMemory::setUniqueQuickId(DWORD uniqueQuickId)
{
	this->refQuickId = uniqueQuickId;
}
/**
 * 获取快速唯一索引
 * \return 快速唯一索引
 */
DWORD stRefMemory::getUniqueQuickId()
{
	return this->refQuickId;
}