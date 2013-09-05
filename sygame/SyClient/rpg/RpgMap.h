/**
 * rpg 地图 承载多层地表 建筑物层 和 怪物层 建筑物 和 地表均来自pack
 */
#pragma once
#include "cocos2d.h"
#include "RpgBuild.h"
#include "RpgPlayer.h"
#include "RpgMonster.h"
#include "RpgTerrain.h"
#include "xmlScript.h"
NS_CC_BEGIN
class RpgUI;
class RpgMap;
class RpgFollow:public CCFollow{
public:
	static RpgFollow *create(const CCSize & mapSize,CCNode *player);
	virtual void step(float dt);
	CCSize mapSize;
	CCNode *player;
	RpgFollow()
	{
		player = NULL;
	}
};

/**
 * 生成策略
 */
class stGenLogic:public CCNode{
public:
	stGenLogic()
	{
		timer = 0;
		maxCount = 0;
		monsterid = 0;
		CCTime::gettimeofdayCocos2d(&lastGenTime, NULL);
	}
	int maxCount;
	int timer;
	std::vector<CCPoint> positions;
	int monsterid;
	cc_timeval lastGenTime;
	CCPoint getRandomPos();
	/**
	 * 处理节点
	 */
	void takeNode(script::tixmlCodeNode *node);
};
/**
 * 轮次策略
 */
class stStepLogic:public CCNode{
public:
	bool hadStart;
	cc_timeval startTime;
	int delayTime; // 延时开始轮次
	int stepTime; // 轮次时间
	std::string adContent; // 广告时间
	RpgMap *map;
	stStepLogic()
	{
		hadStart = false;
		map = NULL;
		stepTime = delayTime = 0;
		valid = false;
	}
	/**
	 * 怪物数据集合
	 */
	std::vector<RpgMonsterData> monsterDatas;
	/**
	 * 处理节点
	 */
	void takeNode(script::tixmlCodeNode *node);

	/**
	 * 开启
	 */
	void start();

	/**
	 * 初始化
	 */
	void init(RpgMap *map);

	/**
	 * 结束了
	 */
	bool isOver();

	/**
	 * 定时行为
	 */
	void doTick();
	bool valid;
	bool isInvalid(){return !valid;}
	bool isValid(){return valid;}
	void setInvalid(){valid = false;}
};
/**
 * 遍历物体的回调
 */
struct stExecEachTarget{
	virtual void exec(RpgObject *object) = 0;
};
/**
 * 地图的阻挡点展示
 */
class MapBlockSprite:public CCNode{
public:
	static MapBlockSprite* create();
	void show(int blockType);
	bool init();
	std::vector<CCSprite*> taps;
};
struct stMonstersYSort{
public:
	int yValue;
	std::list<RpgMonster*> monsters;
	stMonstersYSort()
	{
		yValue = 0;
	}
};

class RpgMap:public CCNode,public script::tixmlCode{
public:
	RpgMap()
	{
		nowMoveBuild = NULL;
		sceneNode = NULL;
		nowSetBlockType = BUILD_CANWALK;
		doTouchType = DO_TOUCH_BUILD;
		infoLog = NULL;
		
		player = NULL;
		ui = NULL;
		score = 0;
		activeSprite = NULL;
		preActiveSprite = NULL;
		barrierSprite = NULL;
	}
	CCSprite *activeSprite;
	CCSprite *preActiveSprite;
	CCSprite *barrierSprite;
    CCRect activeRect;
	int score;
	RpgUI *ui;
	std::string adContent;
	RpgPlayer * player;
	std::string mapName;
	CCNode *  sceneNode;
	std::list<CCNode*> childs;
	typedef std::list<CCNode*>::iterator CHILDS_ITER;
	void addChild(CCNode * child); // 增加节点
	CCLabelTTF *infoLog;
	void removeChild(CCNode* child, bool cleanup); // 删除节点
	/**
	 * 从文件中加载地图
	 */
	static RpgMap* create(const char *mapName,CCNode *parent);
	bool init(const char *mapName,CCNode *parent);
	/**
	 * 从配置文件中加载配置
	 * \param node 配置根节点
	 */
	void takeNode(script::tixmlCodeNode *node);
	/**
	 * 选择有效的节点进行绘制
	 */
	virtual void visit();
	/**
	 * 获取周围8方向上是否有有效点 若有多个随机取一个
	 * \param point 预测点
	 * \param size 所占的格子数
	 * \return 当前有效的点
	 */
	GridPoint getValidGridPoint(const GridPoint &point,const CCSize &size);
	/**
	 * 检查有效点
	 * \param point 测试点
	 * \param blockType 自身不能通过的类型 
	 */
	GridPoint getValidGridPoint(const GridPoint &point,int blockType);
	/**
	 * 通过像素点 获取网格点
	 * \param point 像素点
	 * \return 网格点
	 */
	GridPoint getGridPointByPixelPoint(const CCPoint &point);
	/**
	 * 通过网格点 获取像素点
	 * \param point 网格点
	 * \return 像素点
	 */
	CCPoint GetPixelPointByGridPoint(const GridPoint &point);
	/**
	 * 通过网格点 获取引索
	 * \param point 网格点
	 * \return 引索
	 */
	int getIndexByGridPoint(const GridPoint &point);
	/**
	 * 根据编号获取网格点
	 */
	GridPoint getGridPointByIndex(int index);
	/**
	 * 检查该点是否允许通过
	 * \param point 测点
	 * \param size 当前区域大小
	 * \return 是否有效
	 */
	bool checkValid(const CCPoint &point,const CCSize &size);
	/**
	 * 获取周围的目标
	 * \param point 当前点
	 * \param distance 距离
	 * \param exec 回调
	 */
	void execTargets(const GridPoint &point,int distance,stExecEachTarget *exec);

	/**
	 * 增加怪物 只有一个格子的阻挡属性
	 */
	void addMonster(RpgMonster *monster);

	/**
	 * 删除怪物
	 */
	void removeMonster(RpgMonster *monster);

	/**
	 * 更新怪物 实际为设置位置
	 */
	void updateMonster(RpgMonster *monster);

	void preUpdateMonster(RpgMonster *monster,const GridPoint &nowPoint);
	/**
	 * 增加怪物 到格子点
	 */
	void addMonsterByGridPoint(RpgMonster *monster,const GridPoint &point);
	/**
	 * 删除怪物到格子点
	 */
	void removeMonsterByGridPoint(RpgMonster *monster,const GridPoint &point);
	/**
	 * 设置位置
	 */
	void setPosition(const CCPoint& pos);
	/**
	 * 增加地表 不增加格子的阻挡信息
	 */
	void addTerrain(RpgTerrain *terrain);
	/**
	 * 删除地表
	 */
	void removeTerrain(RpgTerrain *terrain);
	/**
	 * 通过网格获取怪物
	 */
	RpgMonster *findMonsterByGridPoint(const GridPoint &point);
	
	/**
	 * 定时行为 在出生点周围 生成怪物
	 */
	void doTick();

	std::vector<stGenLogic> genLogics; // 生成策略
	typedef std::vector<stGenLogic>::iterator GEN_LOGICS_ITER;
	
	std::list<stStepLogic> stepLogics; // 轮次策略
	typedef std::list<stStepLogic>::iterator STEP_LOGICS_ITER;
	stStepLogic nowStep; // 当前步骤

	int getMonsterCount();

	RpgMonster *pickMonster(const CCPoint& touchPoint);
public:
	/**
	 * 从文件中加载阻挡点信息 
	 */
	void loadBlock(const char *blockFile);
	/**
	 * 将信息保存到文件中
	 */
	void saveBlock(const char *blockFile);
	/**
	 * 格子信息
	 */
	enum{
		NULL_BLOCK = 0, // 没有阻挡
		TERRAIN_BLOCK = 1 << 0, // 地阻挡 
		TERRAIN_CANWALK = 1 << 1, // 地可通
		BUILD_BLOCK = 1 << 2, // 建筑物阻挡
		BUILD_CANWALK = 1 << 3, // 建筑物可通
		PLAYER_BLOCK = 1 << 4, // 人物阻挡
		MONSTER_BLOCK = 1 << 5, // 怪物阻挡
	};
	std::vector<unsigned char> blocks; // 地表信息
	std::vector<MapBlockSprite*> mapblockSprites; // 地表阻挡点信息展示
	/**
	 * 设置阻挡点
	 */
	void setBlock(int index,int blockType);
	/**
	 * 清除阻挡点
	 */
	void clearBlock(int index,int blockType);
	/**
	 * 检查阻挡点
	 */
	bool checkBlock(int index,int blockType);
	std::list<RpgTerrain*> terrains; // 地表集合
	typedef std::list<RpgTerrain*>::iterator TERRAINS_ITER;


	std::vector<std::vector<RpgMonster*> > monsters; // 怪物
	typedef std::vector<std::vector<RpgMonster*> >::reverse_iterator MONSTERS_MAP_ITER;
	typedef std::vector<RpgMonster*>::iterator MONSTERS_ITER;
	
	std::list< stMonstersYSort > newMonsters; // 新的怪物管理器
	typedef std::list<stMonstersYSort >::reverse_iterator NEW_MONSTERS_MAP_ITER;
	typedef std::list<RpgMonster*>::iterator NEW_MONSTERS_ITER;
	CCSize mapSize; // 地图大小
	CCSize canBlockSize; // 可以通过的大小
	CCSize gridSize; // 每个格子大小
	/**
	 * 检查像素点 是否在地图上
	 */
	bool checkInMap(const CCPoint& pixelPoint);
	/**
	 * 检查像素点 是否在阻挡点上
	 * \param blockType 阻挡点类型
	 * \param pixelPoint 像素点 相对于当前的
	 */
	bool checkInBlock(const CCPoint& pixelPoint,int blockType);

	/**
	 * 展示阻挡点
	 */
	void showMapBlocks();

	/**
	 * 检查点是否是可以攻击的点
	 */
	bool checkValidAtkPoint(const GridPoint &src,const GridPoint &point);
	/**
	 * 获取有效的攻击点
	 */
	GridPoint getValidAtkPoint(const GridPoint &point);
public:
///////////////////////////////// 处理建筑物 仿Farm//////////////////////////////////
	/**
	 * 测试是否能够放置建筑物
	 * \param build 建筑物
	 * \param gx 格子横id
	 * \param gy 格子竖id
	 * \return 是否能够放置
	 */
	bool canPlaceBuild(RpgBuild * build,int gx,int gy);
	/**
	 * 放置建筑物
	 * \param build 建筑物
	 * \param gx 格子横 id
	 * \param gy 格子竖 id
	 * \return 是否放置成功
	 */
	bool placeBuild(RpgBuild *build,int gx,int gy);
	/**
	 * 检查该index 是否在建筑的区域内
	 */
	bool checkIndexInBuild(RpgBuild *build,int gx,int gy);
	/**
	 * 查找建筑物
	 * \param tx touch横坐标
	 * \param ty touch纵坐标
	 * \return 建筑物指针 
	 */
	RpgBuild *pickBuild(float tx,float ty);
	RpgBuild *pickBuild(const GridPoint &point);
	/**
	 * 获取引索值
	 */
	int getIndex(int gx,int gy);
	/**
	 * 增加建筑物
	 */
	void addBuild(RpgBuild *build);
	/**
	 * 删除建筑物
	 */
	void removeBuild(RpgBuild *build);
	/**
	 * 获取全局的点
	 */
	CCPoint getPixelPoint(int gx,int gy);
	/**
	 * 清楚阻挡点
	 */
	void clearBlock(const RpgBuildInfo &build);
	/**
	 * 设置阻挡点
	 */
	void setBlock(const RpgBuildInfo &build,int buildId);
	
	/**
	 * 处理点击
	 */
	bool doTouch(int touchType,const CCPoint&touchPoint);

	enum{
		DO_TOUCH_BUILD, // 建筑物
		DO_TOUCH_MOVE, // 移动
		DO_TOUCH_SET, // 设置
	};
	int doTouchType;
	/**
	 * 处理touch 目前也就处理建筑物方面
	 */
	bool doTouchPlace(int touchType,const CCPoint&touchPoint );

	void doTouchBuild(RpgBuild *build);
	CCPoint nowTouchPoint; // 当前touch点
	/**
	 * 控制地图的移动
	 */
	bool doTouchMove(int touchType,const CCPoint &touchPoint);
	/**
	 * 点击设定地图的阻挡点信息
	 */
	int nowSetBlockType; // 当前应该设置的阻挡点类型
	bool doTouchSet(int touchType,const CCPoint &touchPoint);

	RpgBuild *nowMoveBuild;
	std::map<int,RpgBuildInfo> buildInfos;
	typedef std::map<int,RpgBuildInfo>::iterator BUILD_INFOS_ITER;
	RpgBuildInfo findFarmInfoById(int id);

	void showBlocks();
	void actionEnd_removeTemp(CCNode *pSender,void *arg);
	bool checkCollide(RpgMonster *monster,const GridPoint &point);
private:
	std::vector<int> _grids; // 地图上的格子数
	std::vector<CCSprite*> _gridSprites; // 地图上的格子数
	std::vector<RpgBuild*> _builds; // 建筑物集合 是不会相交的
	typedef std::vector<RpgBuild*>::iterator BUILDS_ITER;
	typedef std::vector<int>::iterator GRIDS_ITER;
/////////////////////// 处理建筑物 仿Farm 结束//////////////////////////////////
};

NS_CC_END
