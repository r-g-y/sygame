#pragma once
/**
 * 快速引索管理器
 */
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
	virtual void setUniqueQuickId(int uniqueQuickId) = 0;
	/**
	 * 获取快速唯一索引
	 * \return 快速唯一索引
	 */
	virtual int getUniqueQuickId() = 0;
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
    bool addObject(QuickObjectBase *object);
    
    /**
     * 删除一个物品
     * \param object
     **/
    bool removeObject(QuickObjectBase *object);
  
    /**
     * 查找物品
     * \param 快速引索id
     * \return 物品
     **/
    QuickObjectBase* findObject(int uniqueQuickId);
 
    /**
     * 快速id管理器
     **/
    QuickIdManager();
   
    /**
     * 设置空闲集合的最大空空间
     **/
    void setMaxEmptySize(int size);
   
	/**
	 * 获取存储的引用
	 */
	std::vector<QuickObjectBase * > & getObjects();
	
	/**
	 * 获取节点的数量
	 * \return 节点的数量
	 */
	unsigned int getSize();
	
private:
    std::vector<QuickObjectBase * > _objects; // 物品集合
    std::set<int> emptyIds; // 空闲的id列表
    int maxEmptySize; // 最大空闲大小
};