#include "StartDialog.h"
#include "UIButton.h"
#include "UIWindow.h"
#include "RpgGame.h"
NS_CC_BEGIN
bool StartDialog::initFromFile(const char *fileName)
{
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData(fileName,"rb",&nSize);
	if (!nSize)return false;
	if (script::tixmlCode::initFromString((char*)buffer))
	{
		doInitEvent();
		return true;
	}
	return false;
}
/**
	* 从配置文件中加载配置
	* \param node 配置根节点
	*/
void StartDialog::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
 		script::tixmlCodeNode mainNode = node->getFirstChildNode("start");
		if (mainNode.isValid() && !mainDialog)
		{
			mainDialog = UIPanel::createFromNode(&mainNode);
			window->addPanel(mainDialog);
			mainDialog->setZOrder(12);
		}
	}
}

void StartDialog::doInitEvent()
{
	
	CCSize s = CCDirector::sharedDirector()->getWinSize();
    /*
    CCProgressTo *to1 = CCProgressTo::create(2, 100);
   
    showLoad = CCProgressTimer::create(CCSprite::create("build05.png"));
    showLoad->setType( kCCProgressTimerTypeBar );
	showLoad->setBarChangeRate(ccp(1,0));
	showLoad->setMidpoint(ccp(0,0));
    window->addChild(showLoad);
    showLoad->setPosition(CCPointMake(100, s.height/2));
	showLoad->setPercentage(10);
    //show->runAction( CCRepeatForever::create(to1));
	showLoad->retain();
  
	*/
	do{
		GET_UI_BYNAME(mainDialog,UIButton,startButton,"1");
		if (startButton)
		{
			struct stStartDone:public UICallback{
				void callback(UIBase *base)
				{
					CCDirector::sharedDirector()->resume();
					CCTextureCache::sharedTextureCache()->removeAllTextures();
					CCDirector::sharedDirector()->replaceScene( CCTransitionFlipY::create(0.5,RpgGame::scene()) );
				}
				stStartDone(UIWindow *window):window(window)
				{
				
				}
				UIWindow *window;
			};
			startButton->bind(UIBase::EVENT_CLICK_DOWN,new stStartDone(window));
		}
		GET_UI_BYNAME(mainDialog,UIButton,startTFButton,"2");
		if (startTFButton)
		{
			struct stStartDone:public UICallback{
				void callback(UIBase *base)
				{
					CCDirector::sharedDirector()->resume();
//					CCDirector::sharedDirector()->replaceScene( CCTransitionFlipY::create(0.5,TFGame::scene()) );
				}
			};
			startTFButton->bind(UIBase::EVENT_CLICK_DOWN,new stStartDone());
		}
		GET_UI_BYNAME(mainDialog,UIButton,startMtButton,"startmt");
		if (startMtButton)
		{
			struct stStartMtDone:public UICallback{
				void callback(UIBase *base)
				{
					CCDirector::sharedDirector()->resume();
			//		CCDirector::sharedDirector()->replaceScene( CCTransitionFlipX::create(0.5,MTNewLevelScene::scene()) );
				}
			};
			startMtButton->bind(UIBase::EVENT_CLICK_DOWN,new stStartMtDone());
		}
		GET_UI_BYNAME(mainDialog,UIButton,exitButton,"exit");
		if (exitButton)
		{
			struct stExitDone:public UICallback{
				void callback(UIBase *base)
				{
		//			CCDirector::sharedDirector()->end();
				}
			};
			exitButton->bind(UIBase::EVENT_CLICK_DOWN,new stExitDone());
		}
		GET_UI_BYNAME(mainDialog,UIButton,backButton,"help");
		if (backButton)
		{
			
		}
	} while (false);
}

void StartDialog::doBtnDown(UIBase *base)
{

}

NS_CC_END
