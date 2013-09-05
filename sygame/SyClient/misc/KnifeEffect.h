/**
 * 实现刀光效果的例子
 */
#pragma once
#include "cocos2d.h"
#include <list>
NS_CC_BEGIN
class KnifeEffect:public CCLayer{
public:
	CREATE_FUNC(KnifeEffect);
	bool init();
	/** 
	 * 检查是否在区域里
	 */
	bool touchDown(CCTouch* touch);
	/**
	 * 更新位置
	 */
	bool touchMove(CCTouch* touch);
	/**
	 * 停止拖动
	 */
	bool touchEnd(CCTouch* touch);

	void draw();

	void tick();
private:
	std::list<CCPoint> _points; 
	CCParticleSystem*  m_pParticle ;
	KnifeEffect()
	{
		m_pParticle = NULL;
		isTouchDown = false;
	}
	bool isTouchDown;
};

NS_CC_END