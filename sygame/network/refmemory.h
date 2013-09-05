/**
 * 一种特殊的内存管理方案
 * 在实际使用中 屏蔽了 真正意思上的指针
 * 系统中不会出现使用不存在指针的现象 在指针非法的时候.返回空
 */
#pragma once
#include "sys.h"
#include "quickidmanager.h"
struct stRefMemory:public QuickObjectBase{
	void *pointer;
	int refQuickId;
	DYNAMIC_API stRefMemory()
	{
		refQuickId = -1;
	}
	/**
	 * 设置快速唯一索引
	 * \param uniqueQuickId 设置快速唯一索引
	 */
	DYNAMIC_API virtual void setUniqueQuickId(DWORD uniqueQuickId);
	/**
	 * 获取快速唯一索引
	 * \return 快速唯一索引
	 */
	DYNAMIC_API virtual DWORD getUniqueQuickId() ;
};
