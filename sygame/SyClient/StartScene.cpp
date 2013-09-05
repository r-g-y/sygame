#include "UIWindow.h"
#include "StartScene.h"
#include "StartDialog.h"
#include "TFActionManager.h"
#include <pthread.h>
#include "PngPack.h"
#include "RpgGame.h"
#include "RpgResourceManager.h"
#include "UITextArea.h"
//#include "NetInterface.h"
USING_NS_CC;

LoadProcess* LoadProcess::create(const char *backName,const char *backValue)
{
	LoadProcess *pRet = new LoadProcess();
    if (pRet && pRet->init(backName, backValue))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool LoadProcess::init(const char *backName,const char *backValue)
{
	this->maxValue = 100;
	this->value = 0;
	this->backSprite = CCSprite::create("load_back.png");
	this->backSprite->setAnchorPoint(ccp(0,0.5));
	this->addChild(this->backSprite);
	this->valueSprite = CCSprite::create("load_show.png");
	this->valueSprite->setAnchorPoint(ccp(0,0.5));
	this->addChild(this->valueSprite);
	setValue(value);

	this->setScaleX(700 / this->backSprite->getContentSize().width);

	UIMultiLineArea * text = UIMultiLineArea::create("hello,world",4,32,ccc3(255,0,0));
	this->addChild(text);
	//text->setPosition(ccp(0,0));
	return true;
}
void LoadProcess::setValue(float valuep)
{
	this->value = valuep * maxValue;
	float width = 433;
	width *= ((float)value) / maxValue;
	valueSprite->setTextureRect(CCRectMake(0,0,
	width,valueSprite->getContentSize().height));
}
void LoadProcess::setColor(const ccColor3B& color)
{
	this->backSprite->setColor(color);
	this->valueSprite->setColor(color);
}

CCScene* StartScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    StartScene *layer = StartScene::create();
	
    // add layer as a child to scene
    scene->addChild(layer);
	
    // return the scene
    return scene;
}

/**
 * 一个简单的游戏
 * 进入直接玩 有定时功能 主场景是个BAG
 */

bool StartScene::init()
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	window = UIWindow::create();
	if (!window) return false;
	CCSprite * sprite = CCSprite::create("start.png");
	sprite->setScaleX(visibleSize.width / sprite->getContentSize().width);
	sprite->setScaleY(visibleSize.height / sprite->getContentSize().height);
	this->addChild(sprite);
	sprite->setPosition(ccp(visibleSize.width / 2,visibleSize.height/2));
	process = LoadProcess::create("loadback.png","loadvalue.png");
	this->addChild(process);
	process->setPosition(ccp(visibleSize.width/2 - 350,20));
	startDialog.window = window;
	this->addChild(window);
	if ( !CCLayer::init() )
    {
        return false;
    }
	
    this->setTouchEnabled(true);
	
	std::string startui = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath("startui.xml");
	startDialog.initFromFile(startui.c_str());

	pthread_t hid;
	pthread_create(&hid, NULL, threadLoadResource, this);

	return true;
}
void StartScene::delayLoadResource()
{
	struct stLoadPack:stLoadPackCallback{
		void doGet(int allSize,int nowTap)
		{
			value += nowTap;
			process->setValue((value * 1.0)/allSize);
			if (allSize == nowTap)
			{
				
			}
		}
		stLoadPack(UIValue * show,LoadProcess * process):show(show),process(process)
		{
			value = 0;
		}
		int value;
		UIValue * show;
		LoadProcess * process;
	}loadpack(lineValue,process);
	
	//process->setColor(ccc3(244,0,0));
    std::string mapstr = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath("map.pack");
	PngPack::getMe().load(mapstr.c_str(),&loadpack);
	//loadpack.value = 0;
	//process->setValue(0);
	//process->setColor(ccc3(244,255,0));
	
	//RpgResourceManager::getMe().addResource(RpgResourceManager::RESOURCE_TYPE_UI,"uinew.pack",&loadpack);
}
void StartScene::visit()
{
	CCLayer::visit();
	if (resourceOver) // 资源加载后直接进入游戏场景
	{
		//CCDirector::sharedDirector()->replaceScene( CCTransitionFlipY::create(0.5,RpgGame::scene()) );
		doAddWelcomeUI();
		canStart = true;
		resourceOver = false;
	}
}
void* StartScene::threadLoadResource(void* object)
{
	StartScene * scene = (StartScene*) object;
	if (scene)
	{
		scene->delayLoadResource();
		scene->resourceOver = true;
	}
	return NULL;
}
void StartScene::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	
	/**
	 * 处理窗口
	 */
	if (window->touchDown(pos.x,pos.y)) return;
	
}
void StartScene::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	/**
	 * 处理窗口
	 */
	if (window->touchMove(pos.x,pos.y)) return;
}
void StartScene::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	/**
	 * 处理窗口
	 */
	window->touchEnd(pos.x,pos.y);
    //return;
	if (canStart)
	{
		CCDirector::sharedDirector()->replaceScene( CCTransitionFade::create(0.5,RpgGame::scene()) );
		canStart = false;
	}
}


void StartScene::doAddWelcomeUI()
{
	{
	CCSprite *touchUI = CCSprite::create();
	CCAnimation* animation = CCAnimation::create();
	std::string frameNames[] = {
		std::string("touch_000.png"),
		std::string("touch_001.png"),
		std::string("touch_002.png"),
		std::string("touch_003.png"),
	};
	for (int i = 0; i < 4; i++)
	{
		CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(frameNames[i].c_str());
		
		CCSpriteFrame *frame = CCSpriteFrame::frameWithTexture(texture,CCRectMake(0,0,texture->getContentSize().width,texture->getContentSize().height));
		
		animation->addSpriteFrame(frame);
	}
	
	animation->setDelayPerUnit(1.0f / 4);
	animation->setRestoreOriginalFrame(true);
	touchUI->runAction(CCRepeatForever::create(CCAnimate::create(animation)));
	this->addChild(touchUI);
	touchUI->setPosition(ccp(500,200));
	}
	return;
	{
		CCSprite *touchUI = CCSprite::create();
		CCAnimation* animation = CCAnimation::create();
		std::string frameNames[] = {
			std::string("point_000.png"),
			std::string("point_001.png"),
		};
		for (int i = 0; i < 2; i++)
		{
			CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(frameNames[i].c_str());
			
			CCSpriteFrame *frame = CCSpriteFrame::frameWithTexture(texture,CCRectMake(0,0,texture->getContentSize().width,texture->getContentSize().height));
			
			animation->addSpriteFrame(frame);
		}
		
		animation->setDelayPerUnit(1.5f / 2);
		animation->setRestoreOriginalFrame(true);
		touchUI->runAction(CCRepeatForever::create(CCAnimate::create(animation)));
		this->addChild(touchUI);
		touchUI->setPosition(ccp(650,280));
		touchUI->setScale(0.5);
		touchUI->setFlipX(true);
	}
}
