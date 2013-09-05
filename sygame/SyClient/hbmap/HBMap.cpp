#include "HBMap.h"
#include "HBMonster.h"
#include "HBSky.h"
#include "CalcDir.h"
#include "HBEnd.h"
#include "PngPack.h"
NS_CC_BEGIN

HBFollow *HBFollow::create(HBSky * sky,CCNode *player)
{
	HBFollow *pRet = new HBFollow();
    if (pRet && pRet->initWithTarget(player, CCRectZero))
    {
		pRet->sky = sky;
		pRet->player = player;
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
void HBFollow::step(float dt)
{
	CC_UNUSED_PARAM(dt);
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    if(m_bBoundarySet)
    {
        // whole map fits inside a single screen, no need to modify the position - unless map boundaries are increased
        if(m_bBoundaryFullyCovered)
            return;

        CCPoint tempPos = ccpSub( m_obHalfScreenSize, m_pobFollowedNode->getPosition());

        CCPoint nowPos = ccp(clampf(tempPos.x, m_fLeftBoundary, m_fRightBoundary), 
                                   clampf(tempPos.y, m_fBottomBoundary, m_fTopBoundary));
		if (nowPos.x >=0 || nowPos.x < visibleSize.width - sky->mapSize.width)
		{
			return;
		}
		m_pTarget->setPositionX(nowPos.x);
    }
    else
    {
        CCPoint nowPos = ccpSub(m_obHalfScreenSize, m_pobFollowedNode->getPosition());
		if (nowPos.x >=0 || nowPos.x < visibleSize.width - sky->mapSize.width)
		{
			return;
		}
		m_pTarget->setPositionX(nowPos.x);
    }
}
bool MonsterGenLogic::checkGenNow()
{
	cc_timeval nowTime;
	CCTime::gettimeofdayCocos2d(&nowTime, NULL);
	float tapTime = CCTime::timersubCocos2d(&lastGenTime,&nowTime) / 1000;
	if (tapTime < genTapTime) return false;
	lastGenTime = nowTime;
	return true;
}
void MonsterGenLogic::initWithNode(script::tixmlCodeNode *node)
{
	if (node)
	{
		node->getAttr("taptime",genTapTime);
		node->getAttr("firstcount",genCount);
		//<monster id="0" x="200" y="200"/>
		script::tixmlCodeNode monsterNode = node->getFirstChildNode("monster");
		while (monsterNode.isValid())
		{
			int id = 0;
			float x = 0;
			float y= 0;
			monsterNode.getAttr("id",id);
			monsterNode.getAttr("x",x);
			monsterNode.getAttr("y",y);
			monsters[id] = ccp(x,y);
			monsterNode = monsterNode.getNextNode("monster");
		}

	}
}
MonsterGenLogic::MonsterGenLogic()
{
	genTapTime = 0;
	genCount = 1;
	CCTime::gettimeofdayCocos2d(&lastGenTime, NULL);
}
HBMap *HBMap::createFromXml(const char *fileName)
{
	HBMap *pRet = new HBMap();
    if (pRet && pRet->initFromXml(fileName))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}
bool HBMap::initFromXml(const char *fileName)
{
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData(fileName,"rb",&nSize);
	if (!nSize)return false;
#if 0
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	sky = HBSky::create("Stage07_map_editor-hd.png",ccp(0,0),CCSizeMake(visibleSize.width * 4,visibleSize.height),CCSizeMake(visibleSize.width*1.5,visibleSize.height));
	CCSprite * back = CCSprite::create("1map.png");
	this->addChild(back);
	back->setScaleX(visibleSize.width * 4 / back->getContentSize().width);
	back->setScaleY(visibleSize.height / back->getContentSize().height);
#endif
	CCTime::gettimeofdayCocos2d(&gameStartTime, NULL);
	return script::tixmlCode::initFromString((char*) buffer);
	
}
void HBMap::drawNowPoint(const CCPoint &src,const CCPoint &point)
{
	// 将精灵移动到该点
	if (jianTou)
	{
		jianTou->setPosition(point);
		//jianTou->runAction(CCFadeIn::create(1.0f));
		//jianTou->runAction(CCMoveTo::create(0.3f,point));
	}
}

void HBMap::showMissed(const CCPoint &point)
{
	if (jianTou)
	{
		jianTou->setColor(ccc3(255,233,0));
		jianTou->runAction(CCMoveTo::create(0.02f,point));
		jianTou->runAction(CCFadeOut::create(0.5f));
	}
}
void HBMap::tick()
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	tickCount ++;
	
	if (tickLbl)
	{
		tickLbl->setNumber(tickCount);
	}
	for (MONSTERS_ITER iter = _monsters.begin(); iter != _monsters.end();++iter)
	{
		HBMonster *monster = *iter;
		if (monster /*&& monster->tempId != 0*/)
		{
			monster->tick();
		}
	}
	if (_player && _player->isDeath())
	{
		if (!over)
		{
			over = true;
			HBEndInfo info;
			cc_timeval nowTime;
			CCTime::gettimeofdayCocos2d(&nowTime, NULL);
			float tapTime = CCTime::timersubCocos2d(&gameStartTime,&nowTime) / 1000;
			info.time = tapTime;
			info.monsterCount = _monsters.size() - this->monsterCount();
			CCDirector::sharedDirector()->replaceScene( CCTransitionFlipX::create(0.5,HBEndScene::scene(info)) );
		}
		return;
	}
	if (monsterCount() < 5)
	{
		for ( std::vector<MonsterGenLogic>::iterator iter = genLogics.begin(); iter != genLogics.end();++iter)
		{
			if (iter->checkGenNow())
			{
				// 生成怪物
				int count = 0;
				for (MonsterGenLogic::MONSTERS_ITER pos = iter->monsters.begin(); pos != iter->monsters.end() && count < iter->genCount;++pos,++count)
				{
					int monsterId = pos->first;
					CCPoint location = pos->second;
					MONSTERINFOS_ITER miter = monsterInfos.find(monsterId);
					if (miter != monsterInfos.end())
					{
						HBMonster *monster = HBMonster::createFromInfo(miter->second);
						if (monster)
						{
							addMonster(monster);
							monster->setPosition(location);
						}
					}
				}
				iter->genCount *= 2;
			}
		}
	}
}
/**
 * 从配置文件中加载配置
 * \param node 配置根节点
 */
void HBMap::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
		if (!tickLbl)
		{
			tickLbl = UINumber::create(tickCount,"fps_images.png",12,32,'.');
			tickLbl->setScale(4);
			CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
			tickLbl->setPosition(visibleSize.width/2,visibleSize.height-64);
		}
		script::tixmlCodeNode mapNode = node->getFirstChildNode("mapinfo");
		if (mapNode.isValid())
		{
			mapWidth = 4026;
			mapHeight = 632;
			gridHeight = 125;
			gridWidth = 125;
			mapNode.getAttr("mapwidth",mapWidth);
			mapNode.getAttr("mapheight",mapHeight);
			mapNode.getAttr("gridheight",gridHeight);
			mapNode.getAttr("gridwidth",gridWidth);
			float skyWidth = 2048;
			float skyHeight = 512;
			mapNode.getAttr("skywidth",skyWidth);
			mapNode.getAttr("skyheight",skyHeight);
			std::string yuanjingPng = mapNode.getAttr("yuanjing");
			std::string backPng = mapNode.getAttr("mapback");
			blocks.resize((mapWidth / gridWidth) * (mapHeight / gridWidth));
			blockDebugInfos.resize(blocks.size());
			sky = HBSky::create(yuanjingPng.c_str(),ccp(0,0),CCSizeMake(mapWidth,mapHeight),CCSizeMake(skyWidth,skyHeight));
			back = PngPack::getMe().getNode(backPng.c_str());
			//CCSprite::create(backPng.c_str());
			this->addChild(back,-10);
			//back->setAnchorPoint(ccp(0,0));
			this->setPosition(ccp(0,0));
			back->setScaleX(mapWidth / 1024);
			back->setScaleY(mapHeight / 256);
			std::string jiantouPng = mapNode.getAttr("placejiantou");
			jianTou = CCSprite::create(jiantouPng.c_str());
			jianTou->setPosition(ccp(200,200));
		}
		// 记载Monster 目前只加载xml
		script::tixmlCodeNode atkMonsterNode = node->getFirstChildNode("atkmonster");
		if (atkMonsterNode.isValid() && !_player)
		{
			_player = HBMonster::createFromNode(&atkMonsterNode);
			addMonster(_player);
			_player->setColor(ccc3(255,0,55));
			HBFollow *follow = HBFollow::create(sky,_player);
			this->runAction(follow);
			
		}
		// 加载限制区域 
		// 加载背景图
		
		script::tixmlCodeNode monsterNode = node->getFirstChildNode("monster");
		while (monsterNode.isValid())
		{
			HBMonsterInfo info;
			info.initWithNode(&monsterNode);
			monsterInfos[info.uniqueId] = info;
			monsterNode = monsterNode.getNextNode("monster");
		}
		script::tixmlCodeNode genNode = node->getFirstChildNode("tickgen");
		while (genNode.isValid())
		{
			MonsterGenLogic genLogic;
			genLogic.initWithNode(&genNode);
			genLogics.push_back(genLogic);
			genNode = genNode.getNextNode("tickgen");
		}
	}
}

void HBMap::addMonster(HBMonster *monster)
{
	if (monster)
	{
		monster->map = this;
		monster->tempId = _monsters.size();
		_monsters.push_back(monster);
		this->addChild(monster,2);
	}
}
void HBMap::onTouch(const CCPoint &dest,int touchType)
{
	CCPoint target = this->convertToNodeSpace(dest);
	if (isEditBlock && touchType == HBMonster::ON_TOUCH_DOWN)
	{
		CCPoint blockPos = target;
		if (checkBlock(blockPos.x,blockPos.y))
		{
			clearBlock(blockPos.x,blockPos.y);
		}
		else
			setBlock(blockPos.x,blockPos.y);
	}
	
	_player->onTouch(dest,touchType);
}
/**
 * 查找攻击者
 */
bool  HBMap::findEneries(HBMonster *monster,int eyeshort,std::vector<HBMonster*> &monsters)
{
	for (MONSTERS_ITER iter = _monsters.begin(); iter != _monsters.end(); ++iter)
	{
		HBMonster *target = *iter;
		if (target && target->calcDistance(monster) < eyeshort && target != monster && target->monsterInfo.monsterType != monster->monsterInfo.monsterType)
		{
			monsters.push_back(target);
		}
	}
	return monsters.size();
}
/**
 * 通过id 获取monster
 */
HBMonster *  HBMap::getEneryByID(int id)
{
	if (id < _monsters.size())
		return _monsters.at(id);
	return NULL;
}


void HBMap::sortAllChildren()
{
	if (m_bReorderChildDirty || _needSort)
    {
        int i,j,length = m_pChildren->data->num;
        CCNode ** x = (CCNode**)m_pChildren->data->arr;
        CCNode *tempItem;

        // insertion sort
        for(i=1; i<length; i++)
        {
            tempItem = x[i];
            j = i-1;

            //continue moving element downwards while zOrder is smaller or when zOrder is the same but mutatedIndex is smaller
			while(j>=0 && ( 
				tempItem->getZOrder() < x[j]->getZOrder()
				|| ( tempItem->getZOrder()== x[j]->getZOrder() && tempItem->getOrderOfArrival() < x[j]->getOrderOfArrival() && tempItem->getZOrder() != 2) 
				|| ( tempItem->getZOrder()== x[j]->getZOrder() && tempItem->getZOrder() == 2 && x[j]->getPositionY() < tempItem->getPositionY())
				))
            {
                x[j+1] = x[j];
                j = j-1;
            }
            x[j+1] = tempItem;
        }

        //don't need to check children recursively, that's done in visit of each child

        m_bReorderChildDirty = false;
		_needSort = false;
	}
}
void HBMap::setPosition(const CCPoint& pos)
{
	CCLayer::setPosition(pos);
	if (sky)
	{
		sky->updatePoistion(pos);
	}
}

void HBMap::drawDir(int dir)
{
	if (!jianTou) return;
	switch(dir)
	{
		case CalcDir::_DIR_DOWN:
			{
				jianTou->initWithFile("upjiantou.png");
			}break;
		case CalcDir::_DIR_LEFT:
			{
				jianTou->initWithFile("leftjiantou.png");
			}break;
		case CalcDir::_DIR_RIGHT:
			{
				jianTou->initWithFile("rightjiantou.png");
			}break;
		case CalcDir::_DIR_UP:
			{
				jianTou->initWithFile("downjiantou.png");
			}break;
		case CalcDir::_DIR_DOWN_LEFT:
			{
				//jianTou->initWithFile("downleftjiantou.png");
				jianTou->initWithFile("leftupjiantou.png");
			}break;
		case CalcDir::_DIR_RIGHT_DOWN:
			{
				//jianTou->initWithFile("rightdownjiantou.png");
				jianTou->initWithFile("uprightjiantou.png");
			}break;
		case CalcDir::_DIR_UP_RIGHT:
			{
				jianTou->initWithFile("rightdownjiantou.png");
				//jianTou->initWithFile("uprightjiantou.png");
			}break;
		case CalcDir::_DIR_LEFT_UP:
			{
				//jianTou->initWithFile("leftupjiantou.png");
				jianTou->initWithFile("downleftjiantou.png");
			}break;
	}
}

/**
 * 检查是否在地图上
 * \param x 横坐标
 * \param y 纵坐标
 * \return true 在地图上 false 不在地图上
 */
bool HBMap::checkInMap(float x,float y)
{
	// 使用栅栏的形式
	if (x < 0 || x > mapWidth) return false;
	if (y < 0 || y > mapHeight) return false;
	return true;
}

void HBMap::setBlock(float x,float y)
{
	int index = getIndex(x,y);
	if (index >= 0 && index < blocks.size())
	{
		blocks[index] = 1;
		if (this->isEditBlock)
		{
			if (!blockDebugInfos[index])
			{
				blockDebugInfos[index] = CCSprite::create("test.png");
				this->addChild(blockDebugInfos[index],20);
				blockDebugInfos[index]->setPosition(getPointByGrid(getGridPoint(x,y)));
				blockDebugInfos[index]->setScaleX(gridWidth / blockDebugInfos[index]->getContentSize().width);
				blockDebugInfos[index]->setScaleY(gridHeight / blockDebugInfos[index]->getContentSize().height);
			}
			if (blockDebugInfos[index])
			{
				blockDebugInfos[index]->setVisible(true);
			}
		}
	}
}
int HBMap::getIndex(float x,float y)
{
	int gx = x / this->gridWidth;
	int gy = y / this->gridHeight;
	int gridWidthCount = this->mapWidth / this->gridWidth;
	int gridHeightCount  = this->mapHeight / this->gridHeight;
	if ( gx < gridWidthCount && gy < gridHeightCount && gx >=0 && gy >=0)
	{
		int index = gy * gridWidthCount + gx;
		return index;
	}
	return -1;
}
zPos HBMap::getGridPoint(float x,float y)
{
	int gx = x / this->gridWidth;
	int gy = y / this->gridHeight;
	return zPos(gx,gy);
}
bool HBMap::checkBlock(float x,float y)
{
	int index = getIndex(x,y);
	if (index >= 0 && index < blocks.size())
	{
		return blocks[index] == 1;
	}
	return false;
}
void HBMap::clearBlock(float x,float y)
{
	int index = getIndex(x,y);
	if (index >= 0 && index < blocks.size())
	{
		blocks[index] = 0;
		if (!blockDebugInfos[index])
		{
			blockDebugInfos[index] = CCSprite::create("test.png");
			this->addChild(blockDebugInfos[index],20);
			blockDebugInfos[index]->setPosition(getPointByGrid(getGridPoint(x,y)));
			blockDebugInfos[index]->setScaleX(gridWidth / blockDebugInfos[index]->getContentSize().width);
			blockDebugInfos[index]->setScaleY(gridHeight / blockDebugInfos[index]->getContentSize().height);
		}
		if (blockDebugInfos[index])
		{
			blockDebugInfos[index]->setVisible(false);
		}
	}
}

CCPoint HBMap::getPointByGrid(const zPos& pos)
{
	return ccp(pos.x * this->gridWidth + this->gridWidth /2 ,pos.y * this->gridHeight + this->gridHeight/2);
}

int HBMap::monsterCount()
{
	int count = 0;
	int index = 0;
	for (MONSTERS_ITER iter = _monsters.begin(); iter != _monsters.end();++iter,++index)
	{
		HBMonster *hbmonster = *iter;
		if (hbmonster && !hbmonster->isDeath())
		{
			count++;
		}
		if (index != 0 && hbmonster && hbmonster->isDeath())
		{
			this->removeChild(hbmonster,true);
			_monsters[index] = 0;
		}
	}
	
	return count;
}
NS_CC_END