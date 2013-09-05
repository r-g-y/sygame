#pragma once
#include "cocos2d.h"

NS_CC_BEGIN
/**
 * 横版的天空
 */
class HBSky:public CCNode{
public:
	/**
	 * 创建天空 
	 * \param pngName 背景图
	 * \param position 位置
	 * \param width 宽
	 * \param height 高
	 */
	static HBSky * create(const char *pngName,const CCPoint& position,const CCSize & mapSize,const CCSize & skySize);
	bool init(const char *pngName,const CCPoint& position,const CCSize & mapSize,const CCSize & skySize);
	/**
	 * 更新位置
	 * \param mapNowPosition 地图当前的位置
	 */
	void updatePoistion(const CCPoint & mapNowPosition);

	CCSize mapSize;
	CCSize skySize;
	HBSky()
	{
		
	}
};

NS_CC_END