#pragma once
#include "cocos2d.h"
#include "xmlScript.h"
#include "UIPanel.h"
#include "UIWindow.h"
NS_CC_BEGIN
class FarmMap;
class FarmUI:public script::tixmlCode,public UIPanel{
public:
	bool initFromFile(const char *fileName);
	/**
	 * 从配置文件中加载配置
	 * \param node 配置根节点
	 */
	void takeNode(script::tixmlCodeNode *node);
	
	UIWindow *window;
	FarmMap *map;
	FarmUI()
	{
		window = NULL;
		mainDialog = NULL;
		map = NULL;
	}
	void doTouchDown(UIBase *item);
	
private:
	UIPanel * mainDialog;
	void doInitEvent();
};

NS_CC_END