/**
 * 处理MT的结束流程
 */
#pragma once
#include "cocos2d.h"
#include "xmlScript.h"
#include "UIPanel.h"
#include "UIWindow.h"
#include "UILabel.h"
#include "UIButton.h"
NS_CC_BEGIN

class HBEndInfo{
public:
	int time;
	int monsterCount;
	HBEndInfo()
	{
		time  = 0;
		monsterCount = 0;
	}
};
class HBEnd:public UIPanel{
public:
	static HBEnd* createFromNode(script::tixmlCodeNode *node);
	bool initFromNode(script::tixmlCodeNode *node);
	void showInfo(const HBEndInfo &info);  
protected:
	void backToScene(UIBase *base);
	void initEvents();
};

class HBEndScene:public cocos2d::CCLayer,public script::tixmlCode
{
public:
    virtual bool init(const HBEndInfo &info);  
	bool init(){return true;}
    static cocos2d::CCScene* scene(const HBEndInfo &info);
	  // touch callback
    void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    void ccTouchesMoved(cocos2d::CCSet *pTouch, cocos2d::CCEvent *pEvent);
    void ccTouchesEnded(cocos2d::CCSet *pTouch, cocos2d::CCEvent *pEvent);
	void takeNode(script::tixmlCodeNode *node);
	CREATE_FUNC(HBEndScene);
	cocos2d::UIWindow *window;
	HBEndInfo info;
	HBEndScene()
	{
		window = NULL;
	}
};
NS_CC_END