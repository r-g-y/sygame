#pragma once
#include "cocos2d.h"
#include "xmlScript.h"
#include "UIPanel.h"
#include "UIWindow.h"
NS_CC_BEGIN
class HBMap;
class HBMonster;
/**
 * 横版的技能操作
 * 主要是为了针对 横版操作而设计
 * 一些按钮 %100 比的位置，大小设定 
 **/
class HBSkillPanel:public script::tixmlCode,public UIPanel{
public:
	bool initFromFile(const char *fileName);
	/**
	 * 从配置文件中加载配置
	 * \param node 配置根节点
	 */
	void takeNode(script::tixmlCodeNode *node);
	HBMap *map;
	UIWindow *window;
	HBMonster *player;
	HBSkillPanel()
	{
		map = NULL;
		window = NULL;
		player = NULL;
		mainDialog = NULL; 
		opDialog = NULL;
	}
private:
	UIPanel *mainDialog;
	UIPanel *opDialog;
	void doInitEvent();
	void doTouchDown(UIBase *item);

	void doGiveup(UIBase *item);
};

NS_CC_END