#pragma once
#include "cocos2d.h"
#include "xmlScript.h"
#include "Astar.h"
#include "HBMonster.h"
#include <map>
#include "UINumber.h"
NS_CC_BEGIN

class HBSky;
class HBMap;
class HBFollow:public CCFollow{
public:
	static HBFollow *create(HBSky * sky,CCNode *player);
	virtual void step(float dt);
	HBSky *sky;
	CCNode *player;
	HBFollow()
	{
		sky = NULL;
		player = NULL;
	}
};

class MonsterGenLogic{
public:
	int genTapTime;
	cc_timeval lastGenTime;
	int genCount;
	std::map<int,CCPoint> monsters;
	bool checkGenNow();
	typedef std::map<int,CCPoint>::iterator MONSTERS_ITER;
	void initWithNode(script::tixmlCodeNode *node);
	MonsterGenLogic();
};
/**
 * 提供两层 底图 和 物体层 物体层按y轴排序
 */
class HBMap:public CCLayer,public script::tixmlCode{
public:
	cc_timeval gameStartTime;
	enum{
		MAP_LAYER_BACK,
		MAP_LAYER_OBJECT,
	};
	/**
	 * 从配置文件中加载地图信息
	 * 主要是加载Monster信息
	 */
	static HBMap * createFromXml(const char *fileName);

	bool initFromXml(const char *fileName);
	/**
	 * 查找攻击者
	 */
	bool findEneries(HBMonster *monster,int eyeshort,std::vector<HBMonster*> &monsters);
	/**
	 * 通过id 获取monster
	 */
	HBMonster * getEneryByID(int id);
	/**
	 * 定时行为
	 */
	void tick();
	CCSprite * jianTou;
	CCSprite *opmove;
	void drawNowPoint(const CCPoint &src,const CCPoint &point);
	void showMissed(const CCPoint &point);
	/**
	 * 绘制方向
	 */
	void drawDir(int dir);
	void onTouch(const CCPoint &dest,int touchType);
	void setPosition(const CCPoint& pos);
	HBMap()
	{
		_player = NULL;
		_needSort = false;
		sky = NULL;
		jianTou = NULL;
		opmove = NULL;
		back = NULL;
		mapWidth = 0;
		mapHeight = 0;
		over = false;
		tickLbl = NULL;
		tickCount = 0;
		isEditBlock = true;
	}
	UINumber *tickLbl;
	int tickCount;
	void addMonster(HBMonster *monster);
	/**
	 * 需要排序 原来是增加物体的是否 调用排序 现在是在物体位置发生变化时 就调用
	 */
	void setReSort(){_needSort = true;}
	HBSky *sky; // 天空
	HBMonster *_player; // 主角
	bool over;
	/**
	 * 检查是否在地图上
	 * \param x 横坐标
	 * \param y 纵坐标
	 * \return true 在地图上 false 不在地图上
	 */
	bool checkInMap(float x,float y);
	CCNode * back;
	float mapWidth;
	float mapHeight;
	float gridWidth;
	float gridHeight;
	void setBlock(float x,float y);
	bool checkBlock(float x,float y);
	void clearBlock(float x,float y);
	int getIndex(float x,float y);
	zPos getGridPoint(float x,float y);
	CCPoint getPointByGrid(const zPos& pos);

	bool isEditBlock; // 是否在编辑阻挡点模式
private:
	std::vector<unsigned char > blocks;
	std::vector<CCSprite*> blockDebugInfos;
	std::vector<HBMonster*> _monsters; // 场景上的怪物
	typedef std::vector<HBMonster*>::iterator MONSTERS_ITER;

	std::vector<MonsterGenLogic> genLogics;
	/**
	 * 从配置文件中加载配置
	 * \param node 配置根节点
	 */
	void takeNode(script::tixmlCodeNode *node);
	/**
	 * 给物件层排序
	 */
	virtual void sortAllChildren();
private:
	bool _needSort;
	std::map<int,HBMonsterInfo> monsterInfos;
	typedef std::map<int,HBMonsterInfo>::iterator MONSTERINFOS_ITER;

	int monsterCount();
};
NS_CC_END