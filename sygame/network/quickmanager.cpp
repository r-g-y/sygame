#include "quickidmanager.h" 
/**
     * 增加一个物品
     * \param object
     */
bool QuickIdManager::addObject(QuickObjectBase *object)
{
    if (!emptyIds.empty())
    {
        DWORD id = *emptyIds.begin();
        emptyIds.erase(emptyIds.begin());
        _objects[id] = object;
        object->setUniqueQuickId(id);
    }
    else
    {
        DWORD id = _objects.size();
        for (unsigned int i = 0; i < _objects.size();i++)
        {
            if (!_objects[i])
                id = i;
        }
        if (id < _objects.size())
        {
            _objects[id] = object;
        }
        else
            _objects.push_back(object);
        object->setUniqueQuickId(id);
    }
	return true;
}
/**
 * 删除一个物品
 * \param object
 **/
bool QuickIdManager::removeObject(QuickObjectBase *object)
{
    if (object && object->getUniqueQuickId() < _objects.size())
    {
        _objects[object->getUniqueQuickId()] = NULL;
        if (emptyIds.size() < maxEmptySize)
            emptyIds.insert(object->getUniqueQuickId());
        return true;
    }
    return false;
}
/**
 * 查找物品
 * \param 快速引索id
 * \return 物品
 **/
QuickObjectBase* QuickIdManager::findObject(DWORD uniqueQuickId)
{
    if (uniqueQuickId < _objects.size())
    {
        return _objects[uniqueQuickId];
    }
    return NULL;
}
/**
 * 快速id管理器
 **/
QuickIdManager::QuickIdManager()
{
    maxEmptySize = 100;
}
/**
 * 设置空闲集合的最大空空间
 **/
void QuickIdManager::setMaxEmptySize(DWORD size)
{
    maxEmptySize = size;
}
/**
 * 获取存储的引用
 */
std::vector<QuickObjectBase * > & QuickIdManager::getObjects()
{
	return _objects;
}
/**
 * 获取节点的数量
 * \return 节点的数量
 */
unsigned int QuickIdManager::getSize()
{
	unsigned int count = 0;
	for (std::vector<QuickObjectBase*>::iterator iter = _objects.begin(); iter != _objects.end();++iter)
	{
		if (*iter) count++;
	}
	return count;
}