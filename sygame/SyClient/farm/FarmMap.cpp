#include "FarmMap.h"
#include "FarmBuild.h"
NS_CC_BEGIN
/**
* 从文件中创建 地图
*/
FarmMap* FarmMap::createFromFile(const  char *fileName)
{
	FarmMap *pRet = new FarmMap();
    if (pRet && pRet->initFromFile(fileName))
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
/**
* 从文件中初始化
* \param fileName 文件名字
*/
bool FarmMap::initFromFile(const char *fileName)
{
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData(fileName,"rb",&nSize);
	if (!nSize)return false;
	return script::tixmlCode::initFromString((char*) buffer);
}
/**
* 处理节点
*/
void FarmMap::takeNode(script::tixmlCodeNode *node)
{
	if (!node) return;
	if (node && node->equal("Config"))
	{
		script::tixmlCodeNode mapNode = node->getFirstChildNode("map");
		if (mapNode.isValid())
		{
			mapNode.getAttr("width",mapGridWidth);
			mapNode.getAttr("height",mapGridHeight);
			script::tixmlCodeNode buildNode = mapNode.getFirstChildNode("build");
			while (buildNode.isValid())
			{
				FarmBuild * build = FarmBuild::createFromNode(&buildNode);
				if (build)
				{
					if (!placeBuild(build,build->info.gridX,build->info.gridY))
					{
						build->release(); // 释放build 指针
					}
				}
				buildNode = buildNode.getNextNode("build");
			}
			script::tixmlCodeNode preBuildNode = mapNode.getFirstChildNode("prebuild");
			while (preBuildNode.isValid())
			{
				FarmBuildInfo info;
				info.initWithNode(&preBuildNode);
				buildInfos[info.id] = info;
				preBuildNode = preBuildNode.getNextNode("prebuild");
			}
		}
	}
}
/**
* 测试是否能够放置建筑物
* \param build 建筑物
* \param gx 格子横id
* \param gy 格子竖id
* \return 是否能够放置
*/
bool FarmMap::canPlaceBuild(FarmBuild * build,int gx,int gy)
{
	int startX = -build->info.gridWidth / 2;
	int startY = -build->info.gridHeight / 2;
	for (int i = startX; i < build->info.gridWidth / 2;i++)
		for (int j = startY; j < build->info.gridHeight /2;j++)
		{
			int dgx = gx + i;
			int dgy = gy + j;
			int index = getIndex(dgx,dgy);
			if (index <= -1) continue;
			if (index < _grids.size())
			{
				if (_grids[index] && (_grids[index] != build->uniqueId))
					return false;
			}
			else
			{
				_grids.resize(index + 1);
			}
		}
	return true;
}
/**
 * 检查该index 是否在建筑的区域内
 */
bool FarmMap::checkIndexInBuild(FarmBuild *build,int gx,int gy)
{
	int  startX = build->info.gridX;
	int width = build->info.gridWidth;
	int startY = build->info.gridY;
	int height = build->info.gridHeight;
	if (gx >= startX && gx <= startX + width && gy >= startY && gy <= startY + height)
	{
		return true;
	}
	return false;
}
/**
* 放置建筑物
* \param build 建筑物
* \param gx 格子横 id
* \param gy 格子竖 id
* \return 是否放置成功
*/
bool FarmMap::placeBuild(FarmBuild *build,int gx,int gy)
{
	if (canPlaceBuild(build,gx,gy))
	{
		addBuild(build);
		// 设定建筑物的位置
		CCPoint point = getPixelPoint(gx,gy);
		build->setPosition(point.x,point.y);
		showBlocks();
		return true;
	}
	return false;
}
/**
* 查找建筑物
* \param tx touch横坐标
* \param ty touch纵坐标
* \return 建筑物指针 
*/
FarmBuild *FarmMap::pickBuild(float tx,float ty)
{
	for (BUILDS_ITER iter = _builds.begin(); iter != _builds.end();++iter)
	{
		if ((*iter)->checkIn(tx,ty))
		{
			return *iter;
		}
	}
	return NULL;
}
/**
* 获取引索值
*/
int FarmMap::getIndex(int gx,int gy)
{
	return gx + gy * mapGridWidth;
}

/**
* 增加建筑物
*/
void FarmMap::addBuild(FarmBuild *build)
{
	for (BUILDS_ITER iter = _builds.begin(); iter != _builds.end();++iter)
	{
		if (build == *iter)
		{
			return;
		}
	}
	_builds.push_back(build);
	build->uniqueId = _builds.size();
	build->map = this;
	if (!build->getParent())
	{
		this->addChild(build);
	}
}
/**
* 清楚阻挡点
*/
void FarmMap::clearBlock(const FarmBuildInfo &build)
{
	int startX = -build.gridWidth / 2;
	int startY = -build.gridHeight / 2;
	for (int i = startX; i < build.gridWidth / 2;i++)
		for (int j = startY; j < build.gridHeight /2;j++)
		{
			int dgx = build.gridX + i;
			int dgy = build.gridY + j;
			int index = getIndex(dgx,dgy);
			if (index <= -1) continue;
			if (index >= _grids.size())
			{
				_grids.resize(index + 1);
			}
			_grids[index] = 0;
		}
}
/**
* 设置阻挡点
*/
void FarmMap::setBlock(const FarmBuildInfo &build,int unqiueId)
{
	int startX = -build.gridWidth / 2;
	int startY = -build.gridHeight / 2;
	for (int i = startX; i < build.gridWidth / 2;i++)
		for (int j = startY; j < build.gridHeight /2;j++)
		{
			int dgx = build.gridX + i;
			int dgy = build.gridY + j;
			int index = getIndex(dgx,dgy);
			if (index <= -1) continue;
			if (index >= _grids.size())
			{
				_grids.resize(index + 1);
			}
			_grids[index] = unqiueId;
		}
}
CCPoint FarmMap::getPixelPoint(int gx,int gy)
{
	return ccp(gx * 32,gy * 32);
}

/**
* 处理touch
*/
void FarmMap::doTouch(int touchType,const CCPoint&touchPoint )
{
	switch(touchType)
	{
		case TOUCH_DOWN:
		{
			nowMoveBuild = this->pickBuild(touchPoint.x,touchPoint.y);
			if (nowMoveBuild)
			{
				nowMoveBuild->setZOrder(2);
			}
		}break;
		case TOUCH_MOVE:
		{
			if (nowMoveBuild)
			{
				if (canPlaceBuild(nowMoveBuild,touchPoint.x / 32 ,touchPoint.y / 32))	
				{
					nowMoveBuild->setPosition(touchPoint.x,touchPoint.y);
					showBlocks();
					nowMoveBuild->showValidGridBack();
				}
				else
				{
					nowMoveBuild->preSetPosition(touchPoint.x,touchPoint.y);
					nowMoveBuild->showInvalidGridBack();
					showBlocks();
				}
			}
		}break;
		case TOUCH_END:
		{
			if (nowMoveBuild)
			{
				nowMoveBuild->setZOrder(0);
			}
			nowMoveBuild = NULL;
		}break;
	}
}

FarmBuildInfo FarmMap::findFarmInfoById(int id)
{
	BUILD_INFOS_ITER iter = buildInfos.find(id);
	if (iter != buildInfos.end())
	{
		return iter->second;
	}
	return FarmBuildInfo();
}

void FarmMap::showBlocks()
{
	return;
	int index = 0;
	_gridSprites.resize(_grids.size());
	for (GRIDS_ITER iter = _grids.begin(); iter != _grids.end(); ++iter,++index)
	{
		if (NULL == _gridSprites[index])
		{
			CCSprite * sprite = CCSprite::create("test.png");
			sprite->setScaleX(32 / sprite->getContentSize().width);
			sprite->setScaleY(32 / sprite->getContentSize().height);
			_gridSprites[index] = sprite;
			sprite->setPosition(ccp((index % mapGridWidth) * 32 - 16,(index / mapGridHeight) * 32-16));
			this->addChild(sprite);
		}
		if (*iter == 1)
		{
			// 绘制有效
			_gridSprites[index]->setVisible(false);
		}
		else
		{
			// 绘制无效
			_gridSprites[index]->setVisible(true);
		}
	}
}
NS_CC_END;