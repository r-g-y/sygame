#include "FarmUI.h"
#include "StartScene.h"
#include "FarmBuild.h"
#include "FarmMap.h"
NS_CC_BEGIN
bool FarmUI::initFromFile(const char *fileName)
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
void FarmUI::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
		/**
		 * 技能面板 携带有
		 */
 		script::tixmlCodeNode mainNode = node->getFirstChildNode("opfarm");
		if (mainNode.isValid() && !mainDialog)
		{
			mainDialog = UIPanel::createFromNode(&mainNode);
			window->addPanel(mainDialog);
		}
	}
}
/**
 * 初始化界面系统的逻辑处理模块
 */
void FarmUI::doInitEvent()
{
	struct stBindAction:public stExecPanelEach{
		void exec(UIBase *base)
		{
			base->bind(UIBase::EVENT_CLICK_DOWN,ui_object_function(panel,FarmUI::doTouchDown));
			CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
			base->setPosition(visibleSize.width /2 - base->w /2 -base->x ,visibleSize.height/2 - base->h /2 - base->y);
		} 
		FarmUI *panel;
		stBindAction(FarmUI *panel):panel(panel)
		{
		
		}
	}exec(this);
	mainDialog->execEach(&exec);
}
void FarmUI::doTouchDown(UIBase *item)
{
	// 尝试触发技能
	FarmBuildInfo buildInfo = map->findFarmInfoById(item->uniqueId);
	FarmBuild * build = FarmBuild::createFromBuild(&buildInfo);
	if (build)
	{
		if (!map->placeBuild(build,build->info.gridX,build->info.gridY))
		{
			build->release(); // 释放build 指针
		}
	}
}
NS_CC_END