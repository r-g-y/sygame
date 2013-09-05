#pragma once
#include "cocos2d.h"
#include "xmlScript.h"
#include "UIWindow.h"
#include "HBSkillPanel.h"
NS_CC_BEGIN
class HBMap;
/**
 * 先将怪物加载到场景中
 */

class HBGame: public cocos2d::CCLayer
{
public:
	virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
	 // touch callback
    void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    void ccTouchesMoved(cocos2d::CCSet *pTouch, cocos2d::CCEvent *pEvent);
    void ccTouchesEnded(cocos2d::CCSet *pTouch, cocos2d::CCEvent *pEvent);

	CREATE_FUNC(HBGame);
	cocos2d::UIWindow *window;
	cocos2d::HBMap *map;
	cocos2d::HBSkillPanel skillPanel;
private:
	HBGame()
	{
		window = NULL;
		map = NULL;
	}
	void step(float dt);
};

NS_CC_END