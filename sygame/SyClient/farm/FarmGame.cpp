#include "cocos2d.h"
#include "FarmGame.h"
#include "FarmMap.h"
#include "PngPack.h"
#include "UICenterBag.h"
NS_CC_BEGIN

bool FarmGame::init()
{
	window = UIWindow::create();
	if (!window) return false;
	ui.window = window;
	
	this->addChild(window);
	if ( !CCLayer::init() )
    {
        return false;
    }
	
    this->setTouchEnabled(true);
	
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	map = FarmMap::createFromFile("farmmap.xml");
	if (map)
	{
		this->addChild(map);
	}
	ui.map = map;
	ui.initFromFile("farmui.xml");
	
	bag = UISkillBag::create("bule_big.png","bule_big.png",CCSizeMake(64,64));
	if (bag)
	{
		{
			struct stDoClose:public UIEventCallback<stDoClose>{
				void doClick(UICenterItem *item)
				{
					CCDirector::sharedDirector()->end();
				}
			};
			UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCSizeMake(64,64));
			item->bind(stDoClose::create());
			item->setText("exit");
			bag->addItem(UICenterBag::LEFT_SET,item);
		}
		{
			UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCSizeMake(64,64));
			bag->addItem(UICenterBag::LEFT_SET,item);
		}
		{
			UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCSizeMake(64,64));
			bag->addItem(UICenterBag::LEFT_SET,item);
		}
		{
			UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCSizeMake(64,64));
			bag->addItem(UICenterBag::LEFT_SET,item);
		}
		{
			UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCSizeMake(64,64));
			bag->addItem(UICenterBag::LEFT_SET,item);
		}
		{
			UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCSizeMake(64,64));
			bag->addItem(UICenterBag::LEFT_SET,item);
		}
		{
			UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCSizeMake(64,64));
			bag->addItem(UICenterBag::LEFT_SET,item);
		}
		{
			UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCSizeMake(64,64));
			bag->addItem(UICenterBag::LEFT_SET,item);
		}
		/*
		UICenterBag *childBag = UICenterBag::create("bule_big.png","bule_big.png",CCRectZero);
		if (childBag)
		{
			{
			UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCRectZero);
			childBag->addItem(UICenterBag::UP_SET,item);
			}
			bag->addItem(UICenterBag::LEFT_SET,childBag);
		}
		*/
		this->addChild(bag);
		bag->setPosition(ccp(visibleSize.width - 32,32));
	}
	bag->hideWithOutMe();

	return true;
}
cocos2d::CCScene*  FarmGame::scene()
{
	// 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    FarmGame *layer = FarmGame::create();
	
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}
   
void FarmGame::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	
	/**
	 * 处理窗口
	 */
	if (window->touchDown(pos.x,pos.y)) return;
	if (map) map->doTouch(FarmMap::TOUCH_DOWN,pos);
	if (bag) bag->onTouch(UICenterItem::TOUCH_DOWN,pos);
}
void  FarmGame::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	/**
	 * 处理窗口
	 */
	if (window->touchMove(pos.x,pos.y)) return;
	if (bag) bag->onTouch(UICenterItem::TOUCH_MOVE,pos);
	if (map) map->doTouch(FarmMap::TOUCH_MOVE,pos);
	
}
void  FarmGame::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	/**
	 * 处理窗口
	 */
	window->touchEnd(pos.x,pos.y);
	if (map) map->doTouch(FarmMap::TOUCH_END,pos);
	if (bag) bag->onTouch(UICenterItem::TOUCH_END,pos);
}	

NS_CC_END