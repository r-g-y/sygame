#include "RpgResourceManager.h"

NS_CC_BEGIN
void RpgResourceManager::addResource(int resourceType,const char* packName,stLoadPackCallback *callback)
{
	packs[resourceType] = PngPack();
	PngPack *pack = getResource(resourceType);
	if (pack)
	{
		pack->load(packName,callback);
	}
}
PngPack * RpgResourceManager::getResource(int resourceType)
{
	PACKS_ITER iter = packs.find(resourceType);
	if (iter != packs.end())
	{
		return &iter->second;
	}
	return NULL;
}
NS_CC_END