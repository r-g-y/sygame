#include "RpgBuild.h"
#include "RpgMap.h"
#include "RpgUI.h"
NS_CC_BEGIN
bool RpgBuildInfo::initWithNode(script::tixmlCodeNode * node)
{
	node->getAttr("width",gridWidth);
	node->getAttr("height",gridHeight);
	node->getAttr("x",gridX);
	gridX = gridX / 32;
	node->getAttr("y",gridY);
	gridY = gridY / 32;
	node->getAttr("id",id);
	node->getAttr("maxhp",maxHp);
	node->getAttr("name",name);
	buildName = node->getAttr("pngname");
	return true;
}
/**
* 检查点是否在建筑上
**/
bool RpgBuild::checkIn(float tx,float ty)
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
void RpgBuild::setPosition(float px,float py)
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
void RpgBuild::showHp()
{
	CCSize size = CCSizeMake(15,64);
	if (!hpShow)
	{
		hpShow = RpgHpValueUI::create(hp,info.maxHp,"hp_line_up.png","hp_line_back.png");
		this->addChild(hpShow);
		hpShow->setScaleY(0.5);
	}
	hpShow->setPosition(ccp(size.width,size.height));
	hpShow->setValue(hp);
}
/**
* 设置位置
*/
void RpgBuild::preSetPosition(float px,float py)
{
	CCNode::setPosition(ccp(((int)px / 32 ) * 32,((int) py / 32) * 32));
}
/**
* 从节点中创建建筑物
*/
RpgBuild * RpgBuild::createFromNode(script::tixmlCodeNode * node)
{
	RpgBuild * pRet = new RpgBuild();
	if (pRet && pRet->initWithNode(node))
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool RpgBuild::initWithNode(script::tixmlCodeNode * node)
{
	if (!node) return false;
	info.initWithNode(node);
	initWithBuild(&info);
	return true;
}

void RpgBuild::drawGrid()
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
RpgBuild *RpgBuild::createFromBuild(RpgBuildInfo *buildInfo)
{
	RpgBuild * pRet = new RpgBuild();
	if (pRet && pRet->initWithBuild(buildInfo))
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool RpgBuild::initWithBuild(RpgBuildInfo *buildInfo)
{
	info = *buildInfo;
	if (info.maxHp)
		hp = info.maxHp;
	drawGrid();
	if (info.buildName != "")
		buildSprite = CCSprite::create(info.buildName.c_str());
	else
		buildSprite = CCSprite::create();
	if (buildSprite)
	{
		this->addChild(buildSprite);
		CartoonInfo *info = MonsterManager::getMe().getCobineCartoon(buildInfo->name.c_str(), 0);
		if (info)
		{
			CCAnimate* animation = CCAnimate::create(info->createAnimation());
			buildSprite->runAction(CCRepeatForever::create(animation));
		}
	}
	showHp();
	return true;
}

void RpgBuild::showInvalidGridBack()
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
void RpgBuild::showNonBack()
{
	for (int i = 0; i < gridBacks.size(); i++)
	{
		CCSprite *back = gridBacks[i];
		if (back)
		{
			back->setColor(ccc3(255,0,0));
			back->setOpacity(0);
		}
	}
}
void RpgBuild::showValidGridBack()
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
void RpgBuild::doTouchDown()
{
	if (callback)  callback->doTouch(this);
}

RpgBuildManager::RpgBuildManager()
{
	init();
}
void RpgBuildManager::init()
{
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData("builds.xml","rb",&nSize);
	if (!nSize)return;
	script::tixmlCode::initFromString((char*) buffer);
}
void RpgBuildManager::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
		script::tixmlCodeNode preBuildNode = node->getFirstChildNode("build");
		while (preBuildNode.isValid())
		{
			RpgBuildInfo info;
			info.initWithNode(&preBuildNode);
			buildInfos[info.id] = info;
			preBuildNode = preBuildNode.getNextNode("build");
		}
	}
}
bool RpgBuildManager::findBuildInfo(int id,RpgBuildInfo &info)
{
	BUILD_INFOS_ITER iter = buildInfos.find(id);
	if (iter != buildInfos.end())
	{
		info = iter->second;
		return true;
	}
	return false;
}
NS_CC_END