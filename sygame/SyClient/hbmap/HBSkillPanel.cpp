#include "HBSkillPanel.h"
#include "HBMonster.h"
#include "StartScene.h"
NS_CC_BEGIN
bool HBSkillPanel::initFromFile(const char *fileName)
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
void HBSkillPanel::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
		/**
		 * 技能面板 携带有
		 */
 		script::tixmlCodeNode mainNode = node->getFirstChildNode("skillpanel");
		if (mainNode.isValid() && !mainDialog)
		{
			mainDialog = UIPanel::createFromNode(&mainNode);
			window->addPanel(mainDialog);
			mainDialog->setZOrder(12);
		}
		/**
		 * 操作面板
		 */
		script::tixmlCodeNode opNode = node->getFirstChildNode("oppanel");
		if(opNode.isValid() && !opDialog)
		{
			opDialog = UIPanel::createFromNode(&opNode);
			window->addPanel(opDialog);
			opDialog->setZOrder(13);
		}
	}
}
/**
 * 初始化界面系统的逻辑处理模块
 */
void HBSkillPanel::doInitEvent()
{
	struct stBindAction:public stExecPanelEach{
		void exec(UIBase *base)
		{
			base->bind(UIBase::EVENT_CLICK_DOWN,ui_object_function(panel,HBSkillPanel::doTouchDown));
			CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
			base->setPosition(visibleSize.width /2 - base->w /2 -base->x ,visibleSize.height/2 - base->h /2 - base->y);
		} 
		HBSkillPanel *panel;
		stBindAction(HBSkillPanel *panel):panel(panel)
		{
		
		}
	}exec(this);
	mainDialog->execEach(&exec);

	GET_UI_BYNAME(opDialog,UIButton,giveupbtn,"giveup"); // 战斗
	if (giveupbtn)
	{
		giveupbtn->bind(UIBase::EVENT_CLICK_DOWN,ui_object_function(this,HBSkillPanel::doGiveup));
		CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		giveupbtn->setPosition(-(visibleSize.width /2 - giveupbtn->w /2),visibleSize.height/2 - giveupbtn->h /2);
	}
}
void HBSkillPanel::doTouchDown(UIBase *item)
{
	// 尝试触发技能
	if (player)
	{
		player->putAttack(item->uniqueId);
	}
}

void HBSkillPanel::doGiveup(UIBase *item)
{
	CCDirector::sharedDirector()->replaceScene(StartScene::scene());
}
NS_CC_END