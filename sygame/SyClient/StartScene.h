#pragma once
#include "UIScrollView.h"
#include "StartDialog.h"
#include "UIWindow.h"
#include "UIValue.h"
/**
 * 加载处理
 */
class LoadProcess:public CCNode{
public:
	static LoadProcess* create(const char *backName,const char *backValue);

	bool init(const char *backName,const char *backValue);
	LoadProcess()
	{
		backSprite = NULL;
		valueSprite = NULL;
	}
	void setValue(float value);

	void setColor(const ccColor3B& color);
private:
	CCSprite * backSprite;
	CCSprite * valueSprite;
	int value;
	int maxValue;
};
/**
 * 开始场景
 **/
class StartScene: public cocos2d::CCLayer{
public:
	 // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // touch callback
    void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    void ccTouchesMoved(cocos2d::CCSet *pTouch, cocos2d::CCEvent *pEvent);
    void ccTouchesEnded(cocos2d::CCSet *pTouch, cocos2d::CCEvent *pEvent);

	CREATE_FUNC(StartScene);
	void delayLoadResource();
	static void* threadLoadResource(void* object);
	StartDialog startDialog;
	UIWindow *window;
	UIValue *lineValue;
	LoadProcess *process;
	bool resourceOver;
	void visit();
	void doAddWelcomeUI();
	bool canStart;
private:
	StartScene()
	{
		canStart = false;
		resourceOver = false;
		window = NULL;
		lineValue = NULL;
		process = NULL;
	}
};
