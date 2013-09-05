#include "HBEnd.h"
#include "HBGame.h"
#include "StartScene.h"
NS_CC_BEGIN
HBEnd* HBEnd::createFromNode(script::tixmlCodeNode *snode)
{
	HBEnd *node = new HBEnd();
	if (node && node->initFromNode(snode))
	{
		node->autorelease();
		node->initEvents();
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}
bool HBEnd::initFromNode(script::tixmlCodeNode *node)
{
	script::tixmlCodeNode lblNode = node->getFirstChildNode("label");
	while (lblNode.isValid())
	{
		UILabel *lbl = this->createLblFromNode(&lblNode);
		if (lbl)
		{
			this->addUI(lbl);
			CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		}
		lblNode = lblNode.getNextNode("label");
	}
	script::tixmlCodeNode btnNode = node->getFirstChildNode("button");
	while (btnNode.isValid())
	{
		UIButton *btn = this->createBtn(btnNode);
		if (btn)
		{
			this->addUI(btn);
			CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		}
		btnNode = btnNode.getNextNode("button");
	}
	script::tixmlCodeNode numNode = node->getFirstChildNode("number");
	while (numNode.isValid())
	{
		UINumber *num = this->createNumFromNode(&numNode);
		if (num)
		{
			this->addUI(num);
		}
		numNode = numNode.getNextNode("number");
	}

	float w = 0;
	float h = 0;
	node->getAttr("w",w);
	node->getAttr("h",h);
	std::string backName = node->getAttr("backname");
	this->setSize(w,h);
	this->replaceBack(backName.c_str());
	return true;
}
void HBEnd::backToScene(UIBase *base)
{
	CCDirector::sharedDirector()->replaceScene( CCTransitionFlipX::create(0.5,StartScene::scene()) );
}
void HBEnd::initEvents()
{
	GET_UI_BYNAME(this,UIButton,backButton,"back");
	if (backButton)
	{
		backButton->bind(UIBase::EVENT_CLICK_DOWN,ui_function(HBEnd::backToScene));
		CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		backButton->setPositionX(-visibleSize.width / 2 + backButton->w / 2);
		backButton->setPositionY(visibleSize.height / 2 - backButton->h / 2);
	}
}

void HBEnd::showInfo(const HBEndInfo &info)
{
	GET_UI_BYNAME(this,UINumber,number,"time");
	if (number)
	{
		number->setNumber(info.time);
	}
	GET_UI_BYNAME(this,UINumber,killCount,"killcount");
	if (killCount)
	{
		killCount->setNumber(info.monsterCount);
	}
}

bool HBEndScene::init(const HBEndInfo &info)
{
	window = UIWindow::create();
	if (!window) return false;
	this->addChild(window);
	if ( !CCLayer::init() )
    {
        return false;
    }
	this->setTouchEnabled(true);
	this->info = info;
	std::string endui = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath("end.xml");
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData(endui.c_str(),"rb",&nSize);
	if (!nSize)return false;
	if (script::tixmlCode::initFromString((char*)buffer))
	{
		return true;
	}
	return true;
}
void HBEndScene::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
		script::tixmlCodeNode panelNode = node->getFirstChildNode("panel");
		if (panelNode.isValid())
		{
			HBEnd * end = HBEnd::createFromNode(&panelNode);
			CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
			end->setPosition(visibleSize.width/2,visibleSize.height/2);
			end->showInfo(this->info);
			window->addPanel(end);
		}
	}
}
CCScene* HBEndScene::scene(const HBEndInfo &info)
{
	// 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HBEndScene *layer = HBEndScene::create();
    // add layer as a child to scene
    scene->addChild(layer);
	layer->init(info);
    // return the scene
    return scene;
}
void HBEndScene::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	
	/**
	 * 处理窗口
	 */
	if (window->touchDown(pos.x,pos.y)) return;
}
void HBEndScene::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	/**
	 * 处理窗口
	 */
	if (window->touchMove(pos.x,pos.y)) return;
}
void HBEndScene::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)(* pTouches->begin());
    CCPoint pos = touch->getLocation();
	/**
	 * 处理窗口
	 */
	window->touchEnd(pos.x,pos.y);
}

NS_CC_END