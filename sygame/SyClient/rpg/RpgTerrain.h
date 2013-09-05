/**
 * 加载地图 并且将子类同步做位置更新 有分层的概念 可以加入多层 可以订制子节点的位置更新逻辑
 **/
#pragma once
#include "cocos2d.h"

NS_CC_BEGIN
/**
 * 层
 */
class RpgTerrain:public CCNode{
public:
	/**
	 * 创建天空 
	 * \param pngName 背景图
	 * \param position 位置
	 */
	static RpgTerrain * create(const char *pngName,const CCPoint& position,const CCSize & size);
	bool init(const char *pngName,const CCPoint& position,const CCSize & size);
	/**
	 * 更新位置
	 * \param mapNowPosition 地图当前的位置
	 * \param mapSize 地图的大小
	 */
	void updatePosition(const CCPoint & mapNowPosition,const CCSize &mapSize);
	/**
	 * 系统更新地图位置的回调
	 */
	void setPosition(const CCPoint& pos);
	/**
	 * 当前层的大小
	 */
	CCSize size;
	RpgTerrain()
	{
		
	}
	std::vector<RpgTerrain*> childs;
	typedef std::vector<RpgTerrain*>::iterator CHILDS_ITER;
};

NS_CC_END