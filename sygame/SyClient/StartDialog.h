#pragma once
/**
 * 游戏主界面
 */
#include "cocos2d.h"
#include "UIPanel.h"
#include "UIItem.h"
#include "UIChoiceList.h"
#include "UIBag.h"
#include "UILabel.h"
#include "UIEditField.h"
#include "UIWindow.h"
NS_CC_BEGIN

class StartDialog:public script::tixmlCode,public UIPanel{
public:
	bool initFromFile(const char *fileName);
	/**
	 * 从配置文件中加载配置
	 * \param node 配置根节点
	 */
	void takeNode(script::tixmlCodeNode *node);
	StartDialog()
	{
		mainDialog = NULL;
		window = NULL;
		showLoad = NULL;
	}
	UIWindow *window;
	CCProgressTimer *showLoad;
private:
	void doInitEvent();
	void doBtnDown(UIBase *base);
	UIPanel *mainDialog;
	
};

NS_CC_END