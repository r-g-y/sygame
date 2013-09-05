#include "FarmBuild.h"
#include "FarmMap.h"
NS_CC_BEGIN
bool FarmBuildInfo::initWithNode(script::tixmlCodeNode * node)
{
	node->getAttr("width",gridWidth);
	node->getAttr("height",gridHeight);
	node->getAttr("x",gridX);
	node->getAttr("y",gridY);
	node->getAttr("id",id);
	buildName = node->getAttr("pngname");
	return true;
}
/**
* 检查点是否在建筑上
**/
bool FarmBuild::checkIn(float tx,float ty)
{
	if (buildSprite)
	{
		float width = buildSprite->getTextureRect().size.width;
		float height = buildSprite->getTextureRect().size.height;
		CCPoint pos = ccpSub(getPosition(),ccp(width/2,height/2));
		CCRect rect = CCRectMake(
				pos.x,
				pos.y,
				width,
				height);
		if (rect.containsPoint(ccp(tx,ty)))
		{
			return true;
		}
	}
	return false;
}
/**
* 设置位置
*/
void FarmBuild::setPosition(float px,float py)
{
	CCNode::setPosition(ccp(((int)px / 32 ) * 32,((int) py / 32) * 32));
	if (map)
	{
		map->clearBlock(info);
		info.gridX = (int)px / 32 ;
		info.gridY = (int) py / 32;
		map->setBlock(info,uniqueId);
	}
}
/**
* 设置位置
*/
void FarmBuild::preSetPosition(float px,float py)
{
	CCNode::setPosition(ccp(((int)px / 32 ) * 32,((int) py / 32) * 32));
}
/**
* 从节点中创建建筑物
*/
FarmBuild * FarmBuild::createFromNode(script::tixmlCodeNode * node)
{
	FarmBuild * pRet = new FarmBuild();
	if (pRet && pRet->initWithNode(node))
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool FarmBuild::initWithNode(script::tixmlCodeNode * node)
{
	if (!node) return false;
	info.initWithNode(node);
	drawGrid();
	buildSprite = CCSprite::create(info.buildName.c_str());
	if (buildSprite)
	{
		this->addChild(buildSprite);
	}
	return true;
}

void FarmBuild::drawGrid()
{
	float startX = -32 * (info.gridWidth /2) + 16;
	float startY = -32 * (info.gridHeight /2) + 16;
	for (int i = 0; i < info.gridWidth;i++)
		for (int j = 0; j < info.gridHeight;j++)
		{
			CCSprite * back = CCSprite::create("back_grid.png");
			if (back)
			{
				back->setPosition(ccp(startX + i * 32,startY + j * 32));
				this->addChild(back);
				back->setColor(ccc3(0,255,0));
			}
			gridBacks.push_back(back);
		}
}
/**
* 复制一份Build
*/
FarmBuild *FarmBuild::createFromBuild(FarmBuildInfo *buildInfo)
{
	FarmBuild * pRet = new FarmBuild();
	if (pRet && pRet->initWithBuild(buildInfo))
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool FarmBuild::initWithBuild(FarmBuildInfo *buildInfo)
{
	info = *buildInfo;
	drawGrid();
	buildSprite = CCSprite::create(info.buildName.c_str());
	if (buildSprite)
	{
		this->addChild(buildSprite);
	}
	return true;
}

void FarmBuild::showInvalidGridBack()
{
	for (int i = 0; i < gridBacks.size(); i++)
	{
		CCSprite *back = gridBacks[i];
		if (back)
		{
			back->setColor(ccc3(255,0,0));
			back->setOpacity(80);
		}
	}
}
void FarmBuild::showValidGridBack()
{
	for (int i = 0; i < gridBacks.size(); i++)
	{
		CCSprite *back = gridBacks[i];
		if (back)
		{
			back->setColor(ccc3(0,255,0));
			back->setOpacity(255);
		}
	}
}
NS_CC_END