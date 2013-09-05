#include "cocos2d.h"
#include "RpgGame.h"
#include "RpgMonster.h"
#include "RpgGuide.h"
#include "SoudManager.h"
NS_CC_BEGIN

bool RpgGame::init()
{
	return init("mainscene.xml");
}
bool RpgGame::init(const char *sceneName)
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	if ( !CCLayer::init() )
    {
        return false;
    }
	theSound.init();
	theSound.play("backgroud");
	theItemData->init();
	MonsterManager::getMe().loadConfig();// 加载配置
    this->setTouchEnabled(true);
	map = RpgMap::create(sceneName,this);
	if (map)
	{
		this->addChild(map);
	}
    
	player = RpgPlayer::create(map);
	ui = RpgUI::create(map,player,this);
	if (ui)
	{
		this->addChild(ui);
	}
	
	this->schedule(schedule_selector(RpgGame::step), 0.25f); 
	map->showMapBlocks();
	RpgFollow *follow = RpgFollow::create(map->mapSize,player);
	map->runAction(follow);
	CCMoveBy * moveTo = CCMoveBy::create(0.5f,ccp(visibleSize.width - map->mapSize.width ,0));
	CCAction * action = CCSequence::create(moveTo,moveTo->reverse(),NULL);
//	map->runAction(action);
	CCSprite * back = CCSprite::create("up.png");

	if (back)
	{
		this->addChild(back);
		back->setScaleX(visibleSize.width / back->getContentSize().width);
		back->setScaleY(visibleSize.height / back->getContentSize().height);
		back->setPosition(ccp(visibleSize.width/2,visibleSize.height/2));
	}
	theGuide.init(player,map,ui);
	theGuide.start("welcome"); // 开启欢迎引导
	return true;
}
static cocos2d::CCScene * scene(const char *sceneName)
{
	CCScene *scene = CCScene::create();
    RpgGame *layer = new RpgGame();
	layer->init(sceneName);
	layer->autorelease();
    scene->addChild(layer);
    return scene;
}
cocos2d::CCScene*  RpgGame::scene()
{
    CCScene *scene = CCScene::create();
    RpgGame *layer = RpgGame::create();
    scene->addChild(layer);
    return scene;
}
   
void RpgGame::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	bool  tag = false;
	for (CCSetIterator iter = pTouches->begin(); iter != pTouches->end(); ++iter)
	{
		CCPoint pos = ((CCTouch*)(*iter))->getLocation();
		if (ui && ui->doTouch(RpgUI::TOUCH_DOWN,((CCTouch*)(*iter))))
		{
			tag = true;
		}
	}
	if (tag) return;
	if (map && map->doTouch(RpgUI::TOUCH_DOWN,pos))return;
	canMove = true;
}
void  RpgGame::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	
	bool  tag = false;
	for(CCSetIterator iter = pTouches->begin(); iter != pTouches->end(); ++iter)
	{
		CCPoint pos = ((CCTouch*)(*iter))->getLocation();
		if (ui && ui->doTouch(RpgUI::TOUCH_MOVE,((CCTouch*)(*iter))))
		{
			tag = true;
		}
	}
	if (tag) return;
	if (map && map->doTouch(RpgUI::TOUCH_MOVE,pos))return;
}
void  RpgGame::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	
	for (CCSetIterator iter = pTouches->begin(); iter != pTouches->end(); ++iter)
	{
		CCPoint pos = ((CCTouch*)(*iter))->getLocation();
		if (ui && ui->doTouch(RpgUI::TOUCH_END,((CCTouch*)(*iter))));
	}
	if (map)
		map->doTouch(RpgUI::TOUCH_END,pos);
	if (canMove)
	{
		player->nowDestination = map->getGridPointByPixelPoint(map->convertToNodeSpace(pos));
	}
	canMove = false;
}	
void RpgGame::step(float dt)
{
	if (ui)
		ui->tick();
	if (map)
		map->doTick();
}
NS_CC_END