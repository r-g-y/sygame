/**
 * 角色 的各种动作 和 特效机制 继承Cartoon 
 */

#pragma once
#include "cocos2d.h"
#include "RpgObject.h"
#include "Cartoon.h"
#include "xmlScript.h"
#include "Astar.h"
NS_CC_BEGIN
class RpgMap;
class RpgHpValueUI;// 血量条
class RpgDirAction{
public:
	std::string FramesName; // 帧名字
	std::string frameLoadType; // 帧加载类型
	std::vector<std::string> frames;
	RpgDirAction()
	{
		
	}
	/**
	 * 获取多帧
	 */
	bool getFrames(std::vector<CCSpriteFrame *> &frames);
	/**
	 * 处理节点
	 */
	void takeNode(script::tixmlCodeNode *node);
};

class RpgAction{
public:
	std::string actionName; // 动作名字
	int actionType; // 动作类型
	float needTime; // 需要时间
	int frameType; // 帧类型 时间 或者移动类型
	int cartoonType; // 动画类型 [SELF,OTHER]
	std::map<int,RpgDirAction> dirActions;
	typedef std::map<int,RpgDirAction>::iterator DIRACTIONS_ITER;
	/**
	 * 获取帧集合
	 * \param frames 帧集合 
	 */
	bool getFrames(int dir,std::vector<CCSpriteFrame *> &frames);
	RpgAction()
	{
		actionType = 0;
		needTime = 0;
		cartoonType = 0;
		frameType = 0;
	}
	/**
	 * 处理节点
	 */
	void takeNode(script::tixmlCodeNode *node);
};
/**
 * 组合拳
 */
class RpgConbineAction:public RpgAction{
public:
	int nextType; // 下一个类型
	/**
	 * 处理节点
	 */
	void takeNode(script::tixmlCodeNode *node);
	/**
	 * 构建动画
	 */
	CartoonInfo* getCartoonInfo(int dir);
};
/**
 * Monster 的配置数据
 */
class RpgMonsterData{
public:
	int id; // 编号
	std::string monsterName; // 自己的名字 目前是动作名字
	int step; // 当前的步长
	int eyeshort; // 自己的视野
	int maxAtkDistance; // 最大攻击距离
	int liveTime; // 生存时间
	int reliveTapTime; // 复活间隔时间
	int reliveCount; // 复活的次数 -1 标识永久
	int maxAtkValue; // 最大攻击力
	int minAtkValue; // 最小攻击力
	int maxHp; // 最大血量值
	int xmlX;
	int xmlY;
	int actionId; // 行为编号
	int walkPathId; // 路径号
	int activeRectId; // 活动区域号
	RpgMonsterData()
	{
		actionId = 4;
	}
	void takeNode(script::tixmlCodeNode *node)
	{
		if (node)
		{
			node->getAttr("rectid",activeRectId);
			node->getAttr("pathid",walkPathId);
			node->getAttr("monstername",this->monsterName);
			node->getAttr("step",this->step);
			node->getAttr("eyeshort",this->eyeshort);
			node->getAttr("livetime",this->liveTime);
			node->getAttr("maxatkdistance",this->maxAtkDistance);
			node->getAttr("maxatkvalue",this->maxAtkValue);
			node->getAttr("minatkvalue",this->minAtkValue);
			node->getAttr("maxhp",this->maxHp);
			node->getAttr("x",this->xmlX);
			node->getAttr("y",this->xmlY);
			node->getAttr("actionid",this->actionId);
			node->getAttr("id",this->id);
		}
	}
};
struct stOpTime{
	cc_timeval opTime;
	int lastTime;
	stOpTime()
	{
		lastTime = 0;
	}
};
class RpgMonster:public RpgObject,public zAStar<>{
public:
    int faceDir;
	CCRect activeRect; // 限制的活动范围
	std::vector<CCPoint> path; // 路径
	int dir; // 当前方向
	int hp; // 当前血量
	RpgHpValueUI *hpShow;
	//std::string monsterName; // 自己的名字 目前是动作名字
	//int step; // 当前的步长
	//int eyeshort; // 自己的视野
	//int maxAtkDistance; // 最大攻击距离
	RpgMonsterData monsterData;
	RpgMonster():RpgObject(RPG_MONSTER)
	{
		dir = 0;
		monsterData.step = 32;
		monsterData.eyeshort = 32;
		nowTarget = NULL;
		quickTargetId = -1;
		faceDir = -1;
		monsterData.maxAtkDistance = 3;
		nowDestination = GridPoint(-1,-1);
		deathTag = false;
		CCTime::gettimeofdayCocos2d(&birthTime, NULL);
		hpShow = NULL;
	}
	~RpgMonster()
	{
		rpgSprite = NULL;
		hpShow = NULL;
	}
	CCSprite* rpgSprite;
	cc_timeval birthTime;
	bool isReachDestination();
	/**
	 * 创建怪物
	 */
	static RpgMonster * create(int id);
	bool init(int id);
	enum{ // 越往后优先级越低 动作是被记录 但是是不能同时运作的
		ACTION_DEATH= 1 << 0, // 死亡动画
		ACTION_BIRTH = 1 << 1, // 出生动画
		ACTION_ATTACK = 1 << 2, // 攻击
		ACTION_MOVE = 1 << 3, // 走路
		ACTION_BE_ATTACK = 1 << 4, // 被击
		IDLE_ACTION = 1 << 5, // 空闲
	};
	void showHp();
	virtual void beAttacked(int atkValue);
	void actionEnd_removeLabel(CCNode *pSender,void *arg);
	void tryMove();
	void setDir(int dirType);
	/**
	 * 获取动画信息
	 * \param actionName 动作名字
	 * \param dir 动作方向
	 * \param info 动画信息
	 */
	bool getCartoonInfo(const std::string& actionName,int dir, CartoonInfo &info);

	std::map<std::string,RpgAction> actions; // 动作
	typedef std::map<std::string,RpgAction> ACTIONS;
	typedef ACTIONS::iterator ACTIONS_ITER;
// 动画的相关实现
	/**
	 *  TODO 设置当前的格子
	 *  TODO 判断是否需要停止走路
	 *  TODO 判断是否需要触发访问事件
	 *  TODO 怪物动作结束后 思考下一步行为 随机移动 还是 攻击等
	 */
	virtual void nextStep();
	virtual CartoonInfo* v_makeCartoon(int actionType);
	virtual void v_putCartoon(CartoonInfo *action);
// 动作相关实现结束
	// 使用技能
	std::list<std::string> useatkskills; // 使用的攻击技能
	virtual void putAtkSkill(const std::string& skillname); 
	std::string nowSkillName;// 当前技能名字
	bool isSkill(const std::string& skillName){return nowSkillName == skillName;}
	// 技能相关的结束

	/*
	 *根据方向 获取下一步的移动点
	 */
	CCPoint getPixelPositionByDir(int dir);

	// 在视野范围内寻找对象 并且查看是否可以攻击 能攻击则攻击 否则走近
	void doAttack();
	RpgObject * nowTarget;
	int quickTargetId; // 当前的快速索引号
	RpgMap * getMap()
	{
		return (RpgMap*)(this->getParent());
	}
	GridPoint getGridPoint();
	/**
	 * 获取实际格子
	 */
	GridPoint getNowGridPoint();
	/**
	 * 计算距离
	 * \param point 目的点
	 */
	unsigned int calcDistance(const GridPoint &point);
	
	/**
	 * 获取一周内有效的点
	 */
	GridPoint getValidCirclePoint();

	GridPoint getRandomPoint(const CCSize &rect);
	/**
	 * 获取有效的攻击点
	 */
	GridPoint getValidAtkPoint();
	/**
	 * 检查点是否是可以攻击的点
	 */
	bool checkValidAtkPoint(const GridPoint &point);
	GridPoint nowDestination; //当前目标点 
	GridPoint oldGridPoint; // 上一次的格子点
	virtual bool moveable(const zPos &tempPos,const zPos &destPos,const int radius1 = 12);

	// 是否死亡
	bool deathTag; 
	bool isDeath(){return deathTag;}

	void doDeath(); // 检查自己是否已经死亡
	void visit();

	void dropItems();
	bool checkIn(const GridPoint &point,int tag = 1);
	
	/**
	 * 检查是否是否超时
	 */
	bool checkTimeOut(int opId);
	/**
	 * 检查是否在时间内
	 */
	bool checkInTime(int opId);
	/**
	 * 增加个时间
	 */
	void addTime(int opId,int lastTime);
	/**
	 * 操作时间集合
	 **/
	std::map<int,stOpTime> optimes;
	typedef std::map<int,stOpTime>::iterator OPTIMES_ITER;
};
/**
 * 怪物管理
 */
class MonsterManager:public script::tixmlCode{
public:
	static MonsterManager & getMe();
	/**
	 * 加载配置
	 */
	void loadConfig();
	/**
	 * 从配置文件中加载配置
	 * \param node 配置根节点
	 */
	void takeNode(script::tixmlCodeNode *node);
	/**
	 * 获取动作集合
	 */
	bool getActionByName(const char *actionName,RpgMonster::ACTIONS &actionmap);
	/**
	 * 系统中的动作
	 */
	std::map<std::string,RpgMonster::ACTIONS> actionmaps;
	typedef std::map<std::string,RpgMonster::ACTIONS>::iterator ACTIONMAPS_ITER; 

	/**
	 * 系统中的组合拳动作
	 */
	typedef std::vector<RpgConbineAction> COBINE_ACTIONS;
	std::map<std::string,COBINE_ACTIONS > conbineactionmaps;
	typedef std::map<std::string,COBINE_ACTIONS>::iterator CONBINEACTIONMAPS_ITER; 
	std::map<int,RpgMonsterData> monsterDatas;
	typedef std::map<int,RpgMonsterData>::iterator MONSTERDATAS_ITER;

	/**
	 * 获取动作
	 */
	CartoonInfo * getCobineCartoon(const char *actionName,int dir);

	bool initMonsterData(int id,RpgMonsterData &data);

	bool initMonseterPath(int pathId,std::vector<CCPoint> &path);
	std::map<int ,std::vector<CCPoint> > paths;
	typedef std::map<int,std::vector<CCPoint> >::iterator PATHS_ITER;

	bool initMonsterRect(int rectId,CCRect &rect);
	std::map<int,CCRect> activeRects; // 系统的限定的活动区域
	typedef std::map<int,CCRect>::iterator ACTIVE_RECTS_ITER;
};
typedef void (*ACTION_EXT) (RpgMonster *monster);
class MonsterActionTable{
public:
	MonsterActionTable()
	{
		init();
	}
	static std::vector<ACTION_EXT> functions;
	void init()
	{
		functions.push_back(&MonsterActionTable::doAttackAndWalk);
		functions.push_back(&MonsterActionTable::doOnlyAttack);
		functions.push_back(&MonsterActionTable::doOnlyMove);
		functions.push_back(&MonsterActionTable::doStop);
		functions.push_back(&MonsterActionTable::doAttackPlayerFirstOnlyOneTarget);
		functions.push_back(&MonsterActionTable::doMovePathAndAttack);
	}
	static MonsterActionTable & getMe()
	{
		static MonsterActionTable mat;
		return mat;
	}
	static void doAction(int id,RpgMonster *monster);
	static void doAttackAndWalk(RpgMonster *monster);
	static void doOnlyAttack(RpgMonster *monster);
	static void doOnlyMove(RpgMonster *monster);
	static void doStop(RpgMonster *monster);
	/**
	 *  攻击人物优先 同时只攻击一个目标 这组怪协同作战
	 */
	static void doAttackPlayerFirstOnlyOneTarget(RpgMonster *monster);

	/**
	 * 沿着既定路径行走 并且遇到敌人在附近时攻击 不移动
	 */
	static void doMovePathAndAttack(RpgMonster *monster);
    
    static void doAttackMutiTargetDefaultWalkInPath(RpgMonster *monster);
};

#define theActionLib MonsterActionTable::getMe()
NS_CC_END