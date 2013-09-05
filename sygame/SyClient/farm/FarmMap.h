/**
 * 农场的地图 可以在地图上放置任何的物体
 */
#pragma once
#include "cocos2d.h"
#include "xmlScript.h"
#include "FarmBuild.h"
NS_CC_BEGIN

/**
 * 放置建筑物在上面 
 */
class FarmMap:public CCNode,public script::tixmlCode{
public:
	/**
	 * 从文件中创建 地图
	 */
	static FarmMap* createFromFile(const  char *fileName);
	/**
	 * 从文件中初始化
	 * \param fileName 文件名字
	 */
	bool initFromFile(const char *fileName);
	/**
	 * 处理节点
	 */
	void takeNode(script::tixmlCodeNode *node);
	/**
	 * 测试是否能够放置建筑物
	 * \param build 建筑物
	 * \param gx 格子横id
	 * \param gy 格子竖id
	 * \return 是否能够放置
	 */
	bool canPlaceBuild(FarmBuild * build,int gx,int gy);
	/**
	 * 放置建筑物
	 * \param build 建筑物
	 * \param gx 格子横 id
	 * \param gy 格子竖 id
	 * \return 是否放置成功
	 */
	bool placeBuild(FarmBuild *build,int gx,int gy);
	/**
	 * 检查该index 是否在建筑的区域内
	 */
	bool checkIndexInBuild(FarmBuild *build,int gx,int gy);
	/**
	 * 查找建筑物
	 * \param tx touch横坐标
	 * \param ty touch纵坐标
	 * \return 建筑物指针 
	 */
	FarmBuild *pickBuild(float tx,float ty);
	/**
	 * 获取引索值
	 */
	int getIndex(int gx,int gy);
	/**
	 * 增加建筑物
	 */
	void addBuild(FarmBuild *build);
	/**
	 * 获取全局的点
	 */
	CCPoint getPixelPoint(int gx,int gy);
	/**
	 * 清楚阻挡点
	 */
	void clearBlock(const FarmBuildInfo &build);
	/**
	 * 设置阻挡点
	 */
	void setBlock(const FarmBuildInfo &build,int buildId);
	enum{
		TOUCH_DOWN,
		TOUCH_MOVE,
		TOUCH_END,
	};
	/**
	 * 处理touch
	 */
	void doTouch(int touchType,const CCPoint&touchPoint );

	FarmMap()
	{
		mapGridWidth = mapGridHeight = 0;
		nowMoveBuild = NULL;
	}
	FarmBuild *nowMoveBuild;
	std::map<DWORD,FarmBuildInfo> buildInfos;
	typedef std::map<DWORD,FarmBuildInfo>::iterator BUILD_INFOS_ITER;
	FarmBuildInfo findFarmInfoById(int id);

	void showBlocks();
private:
	int mapGridWidth; // 地图格子宽
	int mapGridHeight; // 地图格子高
	std::vector<int> _grids; // 地图上的格子数
	std::vector<CCSprite*> _gridSprites; // 地图上的格子数
	std::list<FarmBuild*> _builds; // 建筑物集合
	typedef std::list<FarmBuild*>::iterator BUILDS_ITER;
	typedef std::vector<int>::iterator GRIDS_ITER;
};

NS_CC_END