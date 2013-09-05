#pragma once
/**
 * RPG游戏的入口
 */
#include "cocos2d.h"
#include "RpgMap.h"
#include "RpgUI.h"
#include "RpgPlayer.h"

NS_CC_BEGIN

class RpgGame : public cocos2d::CCLayer{
public:
    virtual bool init();  
	virtual bool init(const char *sceneName);
    static cocos2d::CCScene* scene();
	static cocos2d::CCScene * scene(const char *sceneName);
    void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    void ccTouchesMoved(cocos2d::CCSet *pTouch, cocos2d::CCEvent *pEvent);
    void ccTouchesEnded(cocos2d::CCSet *pTouch, cocos2d::CCEvent *pEvent);

	CREATE_FUNC(RpgGame);
	// 设定地图
	RpgMap *map;
	// 设定UI
	RpgUI *ui;
	// 设定人物
	RpgPlayer * player;

	RpgGame()
	{
		map = NULL;
		ui = NULL;
		player = NULL;
		canMove = false;
	}
	void step(float dt);
	bool canMove;
};

NS_CC_END