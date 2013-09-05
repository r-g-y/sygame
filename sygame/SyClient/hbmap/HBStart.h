/**
 * 处理横版游戏的开始选择界面
 */
#pragma once
#include "cocos2d.h"
#include "xmlScript.h"
#include "UIPanel.h"
#include "UIWindow.h"
NS_CC_BEGIN

class HBStartScene{
public:

};
/**
 * 开始界面 - 战斗场景 - 结束界面
 */
class HBStart{
public:
	HBStart()
	{
		_bottomDialog = NULL;
		_heroDialog = NULL;
		_mapDialog = NULL;
		window = NULL;
	}
	bool initFromFile(const char *fileName);
	/**
	 * 从配置文件中加载配置
	 * \param node 配置根节点
	 */
	void takeNode(script::tixmlCodeNode *node);
	/**
	 * 界面元素
	 */
	UIWindow *window;
private:
	/**
	 * 决斗按钮 点击进入副本战斗场景
	 * 英雄信息Label 点击 可展示详细信息 可以点击其装备系统 进入装备Scene 待做 <UIRichText信息>
	 * 当前副本信息 点击可展示详细信息 详细信息面板有选择按钮 点击进入选择场景
	 * 放弃按钮
	 */
	UIPanel * _bottomDialog;
	/**
	 * 英雄的选择界面 目前从本地获取英雄信息 点击场景时 _bottonDialog 消失 当选中其上物体时 _bottonDilog 展示
	 * 默认展示场景
	 * 在联网状态下 会不断的获取hero 的更新信息 且是自动排布
	 **/
	UIPanel * _heroDialog; 
	/**
	 * 副本信息 展示副本信息 目前从本地获取 默认隐藏 
	 **/
	UIPanel * _mapDialog; 


};

NS_CC_END