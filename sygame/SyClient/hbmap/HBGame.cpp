#include "HBGame.h"
#include "UIWindow.h"
#include "HBMap.h"
#include "TFActionManager.h"
#include "UIScrollView.h"
#include "HBSky.h"
#include "HBMonster.h"
#include "UICenterBag.h"
#include "PngPack.h"
USING_NS_CC;

CCScene* HBGame::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HBGame *layer = HBGame::create();
	
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

/**
 * 一个简单的游戏
 * 进入直接玩 有定时功能 主场景是个BAG
 */

bool HBGame::init()
{
	window = UIWindow::create();
	if (!window) return false;

	cc_timeval psv;
	CCTime::gettimeofdayCocos2d(&psv, NULL);
	unsigned long int seed = psv.tv_sec*1000 + psv.tv_usec/1000;
	srand(seed);

	if ( !CCLayer::init() )
    {
        return false;
    }
	
	/*
	std::string actionpath = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath("action.xml");
	TFActionManager::getMe().initFromFile(actionpath.c_str());
    */
	this->setTouchEnabled(true);
	
	//return true;
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	
	map = HBMap::createFromXml("hbmap.xml");
	this->addChild(map);
	if (map->sky)
	{
		this->addChild(map->sky,-11);
		map->sky->setPosition(map->sky->getPositionX(),visibleSize.height - map->sky->skySize.height);
	}

	if (map->jianTou)
	{
		this->addChild(map->jianTou);
		map->jianTou->setZOrder(10);
	}
	if (map->tickLbl)
		window->addUI(map->tickLbl);
#ifndef OP_MOVE
	map->opmove = CCSprite::create("moveop.png");
	this->addChild(map->opmove,10);
	map->opmove->setPosition(ccp(120,120));
	map->opmove->setScaleX(OP_SIZE / map->opmove->getContentSize().width);
	map->opmove->setScaleY(OP_SIZE / map->opmove->getContentSize().height);
#endif
	this->addChild(window);
	skillPanel.window = window;
	skillPanel.map = map;
	std::string killpath = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath("skillpanel.xml");
	skillPanel.initFromFile(killpath.c_str());
	skillPanel.player = map->_player;
	this->schedule(schedule_selector(HBGame::step), 0.02f); 
	
	return true;
}

void HBGame::step(float dt)
{
	if (map)
	{
		map->tick();
	}
}

void HBGame::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	
	/**
	 * 处理窗口
	 */
	if (window->touchDown(pos.x,pos.y)) return;
	map->onTouch(pos,HBMonster::ON_TOUCH_DOWN);
}
void HBGame::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	/**
	 * 处理窗口
	 */
	if (window->touchMove(pos.x,pos.y)) return;
	map->onTouch(pos,HBMonster::ON_TOUCH_MOVE);
}
void HBGame::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	/**
	 * 处理窗口
	 */
	if (!window->touchEnd(pos.x,pos.y))
		map->onTouch(pos,HBMonster::ON_TOUCH_END);
}