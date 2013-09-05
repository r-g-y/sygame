#pragma once
/**
 * 快速引索管理器
 */
#include "sys.h"
#include "vector"
#include "set"
/**
 * 具备快速引索的物品基类
 */
struct QuickObjectBase{
    /**
	 * 设置快速唯一索引
	 * \param uniqueQuickId 设置快速唯一索引
	 */
	DYNAMIC_API virtual void setUniqueQuickId(DWORD uniqueQuickId) = 0;
	/**
	 * 获取快速唯一索引
	 * \return 快速唯一索引
	 */
	DYNAMIC_API virtual DWORD getUniqueQuickId() = 0;
	QuickObjectBase()
	{
	}
};
/**
 * 检索管理器，在生成后quickId 不变动 ，可能会在回收时造成一定程度的空间浪费
 */
class QuickIdManager{
public:
    /**
     * 增加一个物品
     * \param object
     */
    DYNAMIC_API bool addObject(QuickObjectBase *object);
    
    /**
     * 删除一个物品
     * \param object
     **/
    DYNAMIC_API bool removeObject(QuickObjectBase *object);
  
    /**
     * 查找物品
     * \param 快速引索id
     * \return 物品
     **/
    DYNAMIC_API QuickObjectBase* findObject(DWORD uniqueQuickId);
 
    /**
     * 快速id管理器
     **/
    DYNAMIC_API QuickIdManager();
   
    /**
     * 设置空闲集合的最大空空间
     **/
    DYNAMIC_API void setMaxEmptySize(DWORD size);
   
	/**
	 * 获取存储的引用
	 */
	DYNAMIC_API std::vector<QuickObjectBase * > & getObjects();
	
	/**
	 * 获取节点的数量
	 * \return 节点的数量
	 */
	DYNAMIC_API unsigned int getSize();
	
private:
    std::vector<QuickObjectBase * > _objects; // 物品集合
    std::set<DWORD> emptyIds; // 空闲的id列表
    DWORD maxEmptySize; // 最大空闲大小
};