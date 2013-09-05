#include "HBSky.h"

NS_CC_BEGIN
HBSky * HBSky::create(const char *pngName,const CCPoint& position,const CCSize & mapSize,const CCSize & skySize)
{
	HBSky *pRet = new HBSky();
    if (pRet && pRet->init(pngName,position,mapSize,skySize))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}
bool  HBSky::init(const char *pngName,const CCPoint& position,const CCSize & mapSize,const CCSize & skySize)
{
	CCSprite *back = CCSprite::create(pngName);
	if (back)
	{
		this->skySize = skySize;
		this->mapSize = mapSize;
		this->addChild(back);
		back->setScaleX(skySize.width / back->getContentSize().width);
		back->setScaleY(skySize.height / back->getContentSize().height);
		back->setAnchorPoint(ccp(0,0));
	}
	this->setPosition(position);
	return true;
}
/**
* 更新位置
* \param mapNowPosition 地图当前的位置
*/
void  HBSky::updatePoistion(const CCPoint & mapNowPosition)
{
	// 只计算x轴的值
	//float dx = fabs(mapNowPosition.x) / mapSize.width * skySize.width;
	//float dy = fabs(mapNowPosition.y) / mapSize.height * skySize.height;
	//this->setPosition(ccp(mapNowPosition.x / fabs(mapNowPosition.x) * dx,mapNowPosition.y / fabs(mapNowPosition.y) * dy));
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	float dx = ((skySize.width - visibleSize.width ) / (mapSize.width - visibleSize.width)) * mapNowPosition.x;
	this->setPosition(ccp(dx,this->getPositionY()));
}
NS_CC_END