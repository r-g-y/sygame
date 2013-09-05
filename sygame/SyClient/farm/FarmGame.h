#pragma once
/**
 * 农场游戏的入口
 */
#include "cocos2d.h"
#include "UIWindow.h"
#include "FarmUI.h"
#include "FarmMap.h"
#include "UICenterBag.h"
NS_CC_BEGIN

class FarmGame : public cocos2d::CCLayer{
public:
    virtual bool init();  
    static cocos2d::CCScene* scene();
   
    void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    void ccTouchesMoved(cocos2d::CCSet *pTouch, cocos2d::CCEvent *pEvent);
    void ccTouchesEnded(cocos2d::CCSet *pTouch, cocos2d::CCEvent *pEvent);

	CREATE_FUNC(FarmGame);

	UIWindow *window;
	FarmMap * map;
	UISkillBag *bag;
	FarmUI ui;
	FarmGame()
	{
		bag = NULL;
		window = NULL;
		map = NULL;
	}
};

NS_CC_END