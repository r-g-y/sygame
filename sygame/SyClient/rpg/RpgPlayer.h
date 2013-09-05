/**
 * 由移动盘 控制移动 或者指哪点哪
 * 在移动时更新自身的位置
 */

#pragma once
#include "cocos2d.h"
#include "RpgMonster.h"
#include "CalcDir.h"
NS_CC_BEGIN
class RpgMap;
class RpgBuild;
class RpgMinMapUI;
class RpgUI;
class RpgBuild;
class RpgSkillData{
public:
	int atkMinDistance; // 最小攻击距离
	int atkMaxDistance; // 最大攻击距离
	int atkMaxValue; // 最大攻击值
	int atkMinValue; // 最小攻击值
	int atkMaxNum; // 最大攻击数量
	int atkMinNum; // 最小攻击数量
	RpgSkillData()
	{
		atkMinDistance = atkMaxDistance = atkMaxValue = atkMinValue = atkMaxNum = atkMinNum = 0;
	}
	int getAtkDistance();
	int getAtkValue();
	int getAtkNum();
};
class RpgPlayer:public RpgMonster{
public:
    enum{
		MOVE_SPEED_UP,
	};
	RpgPlayer()
	{
		continueMoveDir = CalcDir::_DIR_NULL;
		objectType = RpgObject::RPG_PLAYER;
		miniMap = NULL;
		ui = NULL;
		this->monsterData.step = 96;
		inBuild = false;
		speedTime = 0.6f;
	}
	/**
	 * 通过地图创建玩家
	 */
	static RpgPlayer * create(RpgMap *map);

	bool init(RpgMap *map);
	virtual void v_putCartoon(CartoonInfo *action);
	virtual CartoonInfo* v_makeCartoon(int actionType);
	void putAtkSkill(const std::string& skillname,const RpgSkillData &skillData); 
	void visit();
	void beAttacked(int atkValue);
	/**
	 * 每一步结束后 处理是否访问到了npc 会自动更新主界面的展示
	 */
	virtual void nextStep();
	/**
	 * 系统中的建筑物编号
	 */
	int visitBuildID;
	RpgBuild *visitBuild;
	float speedTime; // 速度
	/**
	 * 系统中的Monster编号 当前移动会更改选中的Monster 攻击不会更改
	 */
	GridPoint monsterPoint;
	RpgMonster *nowMonster; // 当前选定的Monster 
	RpgMinMapUI *miniMap; // 小地图
	RpgUI *ui; // 主界面
	/**
	 * 移动
	 */
	virtual void setPosition(const CCPoint& point);

	void doVisitBuild(RpgBuild *build);
	bool inBuild;
	void doAttack(RpgMonster *monster);
public:
	// 相关存盘的信息
public:
	// 控制移动的消息
	void doContinueMove(int nowDir);
	void clearContinueMove();
	bool isContinueMove(int nowDir);
	int continueMoveDir;
};

NS_CC_END