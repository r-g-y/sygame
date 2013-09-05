#pragma once
#include "MTAction.h"
#include "cocos2d.h"
#include "xmlScript.h"
#include "UIItem.h"
#include "UIValue.h"
#include "Astar.h"
NS_CC_BEGIN
/**
 * 横版PK 游戏制作
 * 这里主要是 提供横版Monster 的PK 走路行为 包含一个简单的直线寻路行为
 */
#define OP_SIZE 256
class HBMap;
class HBMonsterInfo{
public:
	int uniqueId; // 唯一号
	int atkActionId;
	int walkActionId;
	int beAttackActionId;// 被攻击的actionId
	int idleActionId; // 空闲的id
	int monsterType; // 怪物类型
	int deathActionId; // 死亡执行行为
	float _hp; // 当前血量
	float _maxHp; // 当前最大血量
	int _step; // 每步
	float _atkTime; // 攻击时间
	float _eyeshort; // 可视距离
	int _atkDistance;
	float x;
	float y;
	HBMonsterInfo()
	{
		x = y = 0;
		
		idleActionId = -1;
		atkActionId = -1;
		walkActionId = -1;
		deathActionId = -1;
		uniqueId = -1;
		beAttackActionId = -1;
		
		_atkTime = 0;
		_eyeshort = 0;
		monsterType = 0;
		_atkDistance = -1;
		
		_hp = 0;
		_maxHp = 0;
	}

	void initWithNode(script::tixmlCodeNode *node);
};
class HBMonster:public CCSprite,public MTAction,public zAStar<>{
public:
	int tempId; // 在容器中的位置
	int oldActionId; // 当前执行的行为
	int oldDir;
	// 目前支持 出生 死亡 攻击 被击 走路 清闲
	// 优先级 出生 > 死亡 > 走路 > 被击  > 攻击 > 清闲
	enum{
		ACTION_DEATH= 1 << 0, // 死亡动画
		ACTION_BIRTH = 1 << 1, // 出生动画
		ACTION_MOVE = 1 << 2, // 走路
		ACTION_ATTACK = 1 << 3, // 攻击
		ACTION_BE_ATTACK = 1 << 4, // 被击
		IDLE_ACTION = 1 << 5, // 空闲
	};
	enum{
		_DIR_LEFT = 0,
		_DIR_RIGHT = 1,
	};
	bool isDeath();
	void setPosition(const CCPoint& pos);
	bool init();
	static HBMonster* create();
	bool initWithNode(script::tixmlCodeNode *node);
	static HBMonster * createFromNode(script::tixmlCodeNode *node);

	bool initWithInfo(const HBMonsterInfo &info);
	static HBMonster * createFromInfo(const HBMonsterInfo &info);
	/**
	 * 定时行为 检查可视范围内的地方 并走到攻击距离内 进行攻击
	 */
	void tick();
	/**
	 * 执行攻击
	 */
	void doAttack();
	CCFiniteTimeAction* v_makeAction(int actionid);
	void v_putAction(CCFiniteTimeAction *action);
	void actionEnd1();
	/**
	 * 操作板相关
	 */
	CCPoint opMoveCenter;
	bool nowMove; // 当前是否在操作移动
	int opMoveTap; // 判断系数
	void actionEnd();
	/**
	 * 可以并行的处理一些行为 不同的行为可以有存留
	 */
	void conbineActionEnd();
	int nowDir;
	void updateOpMoveCenter(const CCPoint &dest);
	/**
	 * 尝试走到该点
	 */
	void onTouch(const CCPoint& point);
	
	/**
	 * TOUCH 控制移动的新方式
	 */
	enum{
		ON_TOUCH_DOWN,
		ON_TOUCH_MOVE,
		ON_TOUCH_END,
	};
	void onTouch(const CCPoint &point,int moveType);
	CCPoint getStepPointByDir(int dir);
	/**
	 * 横版地图
	 */
	HBMap *map;
	HBMonsterInfo monsterInfo; // 怪物数据
	CCSprite *sprite;
	CCPoint getStepPointBy(const CCPoint &dest);
	/**
	 * 释放攻击动作
	 */
	void putAttack(int actionId);
	/**
	 * 计算距离
	 */
	float calcDistance(HBMonster *monster);
	std::vector<int> atktemppoints;
	CCPoint getPkDestination(HBMonster *attaker,int dir);
	
	/**
	 * 是否在cd 时间内
	 */
	bool checkInCdTime(float cdtime);
	cc_timeval lastAtkTime; // 上次行为的时间
	/**
	 * 更新血量
	 */
	void updateHp(int hp);
	void actionEnd_removeLabel(CCNode*,void*);

	CCPoint calcBeAtkPosition(HBMonster * monster);

	int getReverseDir(int nowDir);
	CCPoint getBackPointByDir(int dir);

	bool checkInXLine(HBMonster *monster);

	void visit();
private:
	HBMonster()
	{
		oldActionId = 0;
		oldDir= 0;
		opMoveTap = 32;
		nowMove = false;
		map = NULL;
		_nowTargetId = -1;
		sprite = NULL;
		lineValue = NULL;
	}
	UIValue *lineValue;
	
	cc_timeval _lastActionTime; // 上次行为的时间
	CCPoint _nowDestination;
	int _nowTargetId;
	
	
	/**
	 * 计算实际方向
	 */
	int calcDir(const CCPoint &dest);

	/**
	 * 计算模型方向
	 */
	int calcMonsterDir(const CCPoint &dest);

	virtual bool moveable(const zPos &tempPos,const zPos &destPos,const int radius1 = 12);
};
NS_CC_END