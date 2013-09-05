/**
 * 系统中存在两种建筑物
 * 召唤塔 和 访问塔
 */
#pragma once
#include "cocos2d.h"
#include "xmlScript.h"
#include "RpgObject.h"
NS_CC_BEGIN
class RpgMap;
class RpgBuild;
class RpgHpValueUI;// 血量条
class RpgBuildEventcallback{
public:
	virtual void doTouch(RpgBuild *build ) = 0;
};
/**
 * 建筑物信息
 */
class RpgBuildInfo{
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
	/**
	 * 操作类型
	 **/
	int opType; 

	/**
	 * 血量
	 */
	int maxHp;
	std::string name;
	enum {
		OP_NULL = -1,
		CAN_PICK = 0, // 可以捡起
		CAN_MOVE = 1, // 可以移动
		CAN_VISIT = 2, // 可以访问
	};
	RpgBuildInfo()
	{
		id = -1;
		gridWidth = gridHeight = -1;
		gridX = gridY = -1;
		opType =CAN_MOVE;
	}
	RpgBuildInfo & operator = (const RpgBuildInfo &info)
	{
		gridX = info.gridX;
		gridY = info.gridY;
		gridWidth = info.gridWidth;
		gridHeight = info.gridHeight;
		id = info.id;
		buildName = info.buildName;
		this->opType = info.opType;
		return *this;
	}
};
/**
 * 建筑物
 */
class RpgBuild:public CCNode{
public:
	enum{
		VISIT_TYPE = 0, // 访问塔
		SUMMON_TYPE = 1,// 召唤塔
		BUILD_NULL = -1, // 无效建筑物
	};
	int buildType;
	RpgBuild()
	{
		buildType = BUILD_NULL;
		buildSprite = NULL;
		actionId = -1;
		map = NULL;
		uniqueId = 0;
		callback = NULL;
		hp = 5;
		hpShow = NULL;
	}
	int hp;
	RpgHpValueUI *hpShow;
	void setCanMove(){info.opType = RpgBuildInfo::CAN_MOVE;}
	void setCanVisit(){info.opType= RpgBuildInfo::CAN_VISIT;}
	/**
	 * 设置位置
	 */
	void setPosition(float px,float py);
	/**
	 * 检查点是否在建筑上
	 **/
	bool checkIn(float tx,float ty);
	RpgBuildInfo info;
	/**
	 * 从节点中创建建筑物
	 */
	static RpgBuild * createFromNode(script::tixmlCodeNode * node);
	bool initWithNode(script::tixmlCodeNode * node);
	/**
	 * 复制一份Build
	 */
	static RpgBuild *createFromBuild(RpgBuildInfo *buildInfo);
	bool initWithBuild(RpgBuildInfo *buildInfo);
	int actionId; // 当前的行为

	CCSprite *buildSprite;
	RpgMap *map;
	std::vector<CCSprite*> gridBacks;
	void showInvalidGridBack();
	void showValidGridBack();
	void showNonBack();
	void drawGrid();
	int uniqueId;
	void preSetPosition(float px,float py);
	void showHp();
	void doTouchDown();
	void bind(RpgBuildEventcallback * call){callback = call;}
	RpgBuildEventcallback *callback;
	virtual CartoonInfo* v_makeCartoon(int actionType){return NULL;}
	virtual void v_putCartoon(CartoonInfo *action){return ;}
};

class RpgBuildManager:public script::tixmlCode{
public:
	RpgBuildManager();
	static RpgBuildManager &getMe()
	{
		static RpgBuildManager rbm;
		return rbm;
	}
	void init();
	void takeNode(script::tixmlCodeNode *node);
	bool findBuildInfo(int id,RpgBuildInfo &info);
	std::map<int,RpgBuildInfo> buildInfos;
	typedef std::map<int,RpgBuildInfo>::iterator BUILD_INFOS_ITER;
};
NS_CC_END