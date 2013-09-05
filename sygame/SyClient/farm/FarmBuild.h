#pragma once
#include "cocos2d.h"
#include "xmlScript.h"
NS_CC_BEGIN
class FarmMap;
class FarmBuildInfo{
public:
	/**
	 * 格子横坐标
	 */
	int gridX;
	/**
	 * 格子纵坐标
	 */
	int gridY;
	/**
	 * 横向格子数
	 */
	int gridWidth;
	/**
	 * 纵向格子数
	 */
	int gridHeight;

	bool initWithNode(script::tixmlCodeNode * node);
	/**
	 * 编号
	 */
	int id ;
	/**
	 * 建筑物名字
	 */
	std::string buildName;
	FarmBuildInfo()
	{
		id = -1;
		gridWidth = gridHeight = -1;
		gridX = gridY = -1;
	}
	FarmBuildInfo & operator = (const FarmBuildInfo &info)
	{
		gridX = info.gridX;
		gridY = info.gridY;
		gridWidth = info.gridWidth;
		gridHeight = info.gridHeight;
		id = info.id;
		buildName = info.buildName;
		return *this;
	}
};
/**
 * 农场建筑物
 */
class FarmBuild:public CCNode{
public:
	/**
	 * 设置位置
	 */
	void setPosition(float px,float py);
	/**
	 * 检查点是否在建筑上
	 **/
	bool checkIn(float tx,float ty);
	FarmBuildInfo info;
	/**
	 * 从节点中创建建筑物
	 */
	static FarmBuild * createFromNode(script::tixmlCodeNode * node);
	bool initWithNode(script::tixmlCodeNode * node);
	/**
	 * 复制一份Build
	 */
	static FarmBuild *createFromBuild(FarmBuildInfo *buildInfo);
	bool initWithBuild(FarmBuildInfo *buildInfo);
	int actionId; // 当前的行为

	CCSprite *buildSprite;
	FarmMap *map;
	std::vector<CCSprite*> gridBacks;
	void showInvalidGridBack();
	void showValidGridBack();
	void drawGrid();
	FarmBuild()
	{
		buildSprite = NULL;
		actionId = -1;
		map = NULL;
		uniqueId = 0;
	}
	int uniqueId;

	void preSetPosition(float px,float py);
};

NS_CC_END