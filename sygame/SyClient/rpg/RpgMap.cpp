#include "RpgMap.h"
#include "RpgBuild.h"
#include "RpgUI.h"
#include "serialize.h"
NS_CC_BEGIN

RpgFollow *RpgFollow::create(const CCSize & mapSize,CCNode *player)
{
	RpgFollow *pRet = new RpgFollow();
    if (pRet && pRet->initWithTarget(player, CCRectZero))
    {
		pRet->mapSize = mapSize;
		pRet->player = player;
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
void RpgFollow::step(float dt)
{
	CC_UNUSED_PARAM(dt);
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint nowPos;
    if(m_bBoundarySet)
    {
        // whole map fits inside a single screen, no need to modify the position - unless map boundaries are increased
        if(m_bBoundaryFullyCovered)
            return;

        CCPoint tempPos = ccpSub( m_obHalfScreenSize, m_pobFollowedNode->getPosition());

        nowPos = ccp(clampf(tempPos.x, m_fLeftBoundary, m_fRightBoundary), 
                                   clampf(tempPos.y, m_fBottomBoundary, m_fTopBoundary));	
    }
    else
    {
        nowPos = ccpSub(m_obHalfScreenSize, m_pobFollowedNode->getPosition());
    }
	if (!(nowPos.x >=0 || nowPos.x < visibleSize.width - mapSize.width))
	{
		m_pTarget->setPositionX(nowPos.x);
	}

	if (!(nowPos.y >=0 || nowPos.y < visibleSize.height - mapSize.height))
	{
		m_pTarget->setPositionY(nowPos.y);
	}
}

MapBlockSprite* MapBlockSprite::create()
{
	MapBlockSprite * pRet = new MapBlockSprite();
	if (pRet && pRet->init())
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool MapBlockSprite::init()
{
	// 创建6个CCSprite 分别安放在指定位置上
	for (unsigned int x = 0; x < 2;x++)
		for (unsigned int y = 0; y < 3;y++)
		{
			int index = y * 2 + x;
			if (index >= taps.size())
			{
				taps.resize(index + 1);
			}
			taps[index] = CCSprite::create("test.png");
			taps[index]->setScaleX(16 / taps[y * 2 + x]->getContentSize().width);
			taps[index]->setScaleY(11 / taps[y * 2 + x]->getContentSize().height);
			this->addChild(taps[index]);
			taps[index]->setAnchorPoint(ccp(0,0));
			taps[index]->setPosition(ccp(x*16,y*11));
		}
	CCSprite *back = CCSprite::create("test.png");
	if (back)
	{
		back->setScaleX(32 / back->getContentSize().width);
		back->setScaleY(32 / back->getContentSize().height);
		this->addChild(back);
		back->setAnchorPoint(ccp(0,0));
		back->setOpacity(30);
		back->setVisible(false);
		//back->setPosition(ccp(8,5));
	}
	return true;
}
void MapBlockSprite::show(int blockType)
{
	for (int i = 0; i < 6;i++)
	{
		taps[i]->setVisible(false);
		if (blockType & 1 << i)
		{
			taps[i]->setColor(ccc3(i * 25,i*30,0));
			taps[i]->setVisible(true);
		}
	}
}
//////////////产生规则///////////////////////////
/**
 * 处理节点
 */
void stGenLogic::takeNode(script::tixmlCodeNode *node)
{
	if (node)
	{
		node->getAttr("timer",timer);
		node->getAttr("monsterid",monsterid);
		script::tixmlCodeNode posNode = node->getFirstChildNode("pos");
		node->getAttr("maxcount",maxCount);
		while (posNode.isValid())
		{
			CCPoint pos;
			posNode.getAttr("x",pos.x);
			posNode.getAttr("y",pos.y);
			this->positions.push_back(pos);
			posNode = posNode.getNextNode("pos");
		}
	}
}
CCPoint stGenLogic::getRandomPos()
{
	if (positions.empty())
	{
		return ccp(0,0);
	}
	return positions.at(CCRANDOM_0_1() * positions.size());
}
//////////////////////////轮次规则///////////////////

/**
 * 处理节点
 */
void stStepLogic::takeNode(script::tixmlCodeNode *node)
{
	if (node)
	{
		node->getAttr("delaytime",delayTime);
		node->getAttr("steptime",stepTime);
		node->getAttr("adcontent",adContent);
		script::tixmlCodeNode mNode = node->getFirstChildNode("monsterdata");
		while (mNode.isValid())
		{
			RpgMonsterData mData;
			mData.takeNode(&mNode);
			monsterDatas.push_back(mData);
			mNode = mNode.getNextNode("monsterdata");
		}
	}
}

/**
 * 开启
 */
void stStepLogic::start()
{
	if (map)
	{
		map->ui->stepEnd->setVisible(false);
		// 启动刷怪
		for (int i = 0; i < monsterDatas.size();i++)
		{
			RpgMonsterData &data = monsterDatas[i];
			RpgMonster *monster = RpgMonster::create(data.id);
			if (monster)
			{
				monster->setPosition(ccp(data.xmlX,data.xmlY));
				monster->hp = data.maxHp;
				monster->monsterData = data;
				map->addMonster(monster);
			}
		}
	}
}

/**
 * 初始化
 */
void stStepLogic::init(RpgMap *map)
{
	CCTime::gettimeofdayCocos2d(&startTime,NULL);
	this->map = map;
	valid = true;
	map->ui->adUI->addContent(adContent);
}

/**
 * 结束了
 */
bool stStepLogic::isOver()
{
	cc_timeval nowTime;
	CCTime::gettimeofdayCocos2d(&nowTime,NULL);
	if (CCTime::timersubCocos2d(&startTime,&nowTime) / 1000 > delayTime + stepTime)
	{
		return true;
	}
	return false;
}
/**
 * 定时行为
 */
void stStepLogic::doTick()
{
	cc_timeval nowTime;
	CCTime::gettimeofdayCocos2d(&nowTime,NULL);
	if (CCTime::timersubCocos2d(&startTime,&nowTime) / 1000 > delayTime && !hadStart)
	{
		hadStart = true;
		start();
	}
}
//////////////////////////RPG地图////////////////////
RpgMap* RpgMap::create(const char *mapName,CCNode *parent)
{
	RpgMap * pRet = new RpgMap();
	if (pRet && pRet->init(mapName,parent))
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool RpgMap::init(const char *mapName,CCNode *parent)
{
	this->sceneNode = parent;
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData(mapName,"rb",&nSize);
	if (!nSize)return false;
	gridSize = CCSizeMake(32,32);
	mapSize = CCSizeMake(2350,768); 
	canBlockSize = CCSizeMake(2350,564);
	unsigned int blockSize = (2350 * 768) / (32 * 32);
	blocks.resize(blockSize*2);
	mapblockSprites.resize(blockSize*2);
	infoLog = CCLabelTTF::create("","Arial",16);
	parent->addChild(infoLog,11);
	infoLog->setPosition(ccp(200,200));
	return script::tixmlCode::initFromString((char*) buffer);
}
/**
 * 从配置文件中加载配置
 * \param node 配置根节点
 */
void RpgMap::takeNode(script::tixmlCodeNode *node)
{
	/**
	* <map name="xxx">
	*		<!--可以定义多个地表层-->
	*		<terrain name="" x="" y="" width="" height=""/>
	*		<terrain name="" x="" y="" width="" height=""/>
	*		<block size="">
	*			base64 code 设定为blocks
	*		</block>
	*		<npc x="" y="" id="" /> <!--设定npc-->
	*		<build id="" x="" y="" /> <!--建筑默认生产-->
	* </map>
	*/
	if (node && node->equal("Config"))
	{
		script::tixmlCodeNode mapNode = node->getFirstChildNode("map");
		if (mapNode.isValid())
		{
#if (1)
            float width = mapNode.getFloat("width");
            float height = mapNode.getFloat("height");
            mapSize = CCSizeMake(width, height);
            gridSize.width = mapNode.getFloat("gridwidth");
            gridSize.height = mapNode.getFloat("gridheight");
            canBlockSize.width = mapNode.getFloat("canblockwidth");
            canBlockSize.height = mapNode.getFloat("canblockheight");
            script::tixmlCodeNode activeNode = mapNode.getFirstChildNode("activerect");
            if (activeNode.isValid())
            {
                activeRect.origin.x = activeNode.getInt("x");
                activeRect.origin.y = activeNode.getInt("y");
                activeRect.size.width = activeNode.getInt("width");
                activeRect.size.height = activeNode.getInt("height");
				activeSprite = CCSprite::create(activeNode.getAttr("spritename"));
				this->addChild(activeSprite);
				activeSprite->setAnchorPoint(ccp(0,0));
				activeSprite->setPosition(ccp(activeRect.origin.x,activeRect.origin.y));
				activeSprite->setScaleX(activeRect.size.width / activeSprite->getContentSize().width);
				activeSprite->setScaleY(activeRect.size.height / activeSprite->getContentSize().height);
				activeSprite->setOpacity(125);
/*
				barrierSprite = CCSprite::create("weiqiang.png");
				this->addChild(barrierSprite);
				barrierSprite->setAnchorPoint(ccp(0,0));
				activeSprite->setPosition(ccp(activeRect.getMaxX(),activeRect.getMaxY()));
*/
            }
#endif
			adContent = mapNode.getAttr("adcontent");
			std::string blockName = mapNode.getAttr("blockname");
			this->loadBlock(blockName.c_str());
			mapName = mapNode.getAttr("name");
			script::tixmlCodeNode terrainNode = mapNode.getFirstChildNode("terrain");
			while (terrainNode.isValid())
			{
				std::string terrainName = terrainNode.getAttr("name");
				CCPoint pos;
				terrainNode.getAttr("x",pos.x);
				terrainNode.getAttr("y",pos.y);
				CCSize size;
				terrainNode.getAttr("width",size.width);
				terrainNode.getAttr("height",size.height);
				RpgTerrain *terrain = RpgTerrain::create(terrainName.c_str(),pos,size);
				addTerrain(terrain);
				terrainNode = terrainNode.getNextNode("terrain");
			} // 初始化地形
			script::tixmlCodeNode monsterNode = mapNode.getFirstChildNode("monster");
			while(monsterNode.isValid())
			{
				CCPoint pos;
				monsterNode.getAttr("x",pos.x);
				monsterNode.getAttr("y",pos.y);
				int id = 0;
				monsterNode.getAttr("id",id);
				RpgMonster *monster = RpgMonster::create(id);
				if (monster)
				{
					monster->setPosition(pos);
					this->addMonster(monster);
				}
				monsterNode = monsterNode.getNextNode("monster");
			}
			script::tixmlCodeNode genlogicNode = mapNode.getFirstChildNode("genlogic");
			while(genlogicNode.isValid())
			{
				stGenLogic logic;
				logic.takeNode(&genlogicNode);
				genLogics.push_back(logic);
				genlogicNode = genlogicNode.getNextNode("genlogic");
			}
			script::tixmlCodeNode steplogicNode = mapNode.getFirstChildNode("genstep");
			while(steplogicNode.isValid())
			{
				stStepLogic step;
				step.takeNode(&steplogicNode);
				stepLogics.push_back(step);
				
				steplogicNode = steplogicNode.getNextNode("genstep");
			}
			script::tixmlCodeNode preBuildNode = mapNode.getFirstChildNode("prebuild");
			while (preBuildNode.isValid())
			{
				RpgBuildInfo info;
				info.initWithNode(&preBuildNode);
				buildInfos[info.id] = info;
				preBuildNode = preBuildNode.getNextNode("prebuild");
			}

			script::tixmlCodeNode buildNode = mapNode.getFirstChildNode("build");
			while (buildNode.isValid())
			{
				RpgBuild * build = RpgBuild::createFromNode(&buildNode);
				if (build)
				{
					if (!placeBuild(build,build->info.gridX,build->info.gridY))
					{
						addBuild(build);
						build->preSetPosition(build->info.gridX *32,build->info.gridY * 32);
					}
				}
				buildNode = buildNode.getNextNode("build");
			}
		}
		this->showMapBlocks();
	}
}
void RpgMap::visit()
{
    // quick return if not visible. children won't be drawn.
    if (!m_bIsVisible)
    {
        return;
    }
    kmGLPushMatrix();

     if (m_pGrid && m_pGrid->isActive())
     {
         m_pGrid->beforeDraw();
     }

    this->transform();

	
///处理新的绘制方案
	
	for (TERRAINS_ITER iter = terrains.begin(); iter != terrains.end();++iter)
	{
		RpgTerrain *terrain = *iter;
		if (terrain)
		{
			terrain->visit(); // 绘制地表
		}
	}
	bool tag = false;
	for (CHILDS_ITER iter = childs.begin(); iter != childs.end();++iter)
	{
		if (*iter)
		{
			(*iter)->visit();
			tag = true;
		}
	}
	for (BUILDS_ITER iter = _builds.begin(); iter != _builds.end(); ++iter)
	{
		RpgBuild *build = *iter;
		if (build)
		{
			build->visit(); // 绘制建筑物
		}
	}
	
	for ( MONSTERS_MAP_ITER iter = monsters.rbegin(); iter != monsters.rend();++iter)
	{
		for (MONSTERS_ITER pos = iter->begin(); pos != iter->end();++pos)
		{
			if (*pos)
			{
				/*
				CCPoint point = (*pos)->getPosition();
				(*pos)->setOpacity(255);
*/
				(*pos)->visit(); // 绘制怪物
				/*
				(*pos)->setPosition(ccpAdd(point,ccp(50,0)));
				(*pos)->setOpacity(125);
				(*pos)->visit();
				(*pos)->setPosition(point);*/
			}
		}
	}
	for ( NEW_MONSTERS_MAP_ITER iter = newMonsters.rbegin(); iter != newMonsters.rend();++iter)
	{
		stMonstersYSort &sort = *iter;
		for (NEW_MONSTERS_ITER pos = sort.monsters.begin(); pos != sort.monsters.end();++pos)
		{
			if (*pos)
			{
				(*pos)->visit(); // 绘制怪物;
			}
		}
	}
	
	if (player && player->isVisible() && tag)
	{
		player->setOpacity(125);
		player->visit();
		player->setOpacity(255);
	}
/// 处理结束
    m_nOrderOfArrival = 0;

     if (m_pGrid && m_pGrid->isActive())
     {
         m_pGrid->afterDraw(this);
    }
 
    kmGLPopMatrix();
	
}
/**
 * 系统更新地图位置的回调
 * \param pos 地图位置
 */
void RpgMap::setPosition(const CCPoint& pos)
{
	CCNode::setPosition(pos);
	for (TERRAINS_ITER iter = terrains.begin(); iter != terrains.end();++iter)
	{
		RpgTerrain *terrain = *iter;
		if (terrain)
		{
		//	mapSize = CCSizeMake(4026,468);
			terrain->updatePosition(pos,mapSize);
		}
	}
}
void RpgMap::addChild(CCNode * child)
{
	childs.push_back(child);
	child->retain();
	child->setParent(this);
    child->onEnter();
    child->onEnterTransitionDidFinish();
}
void RpgMap::removeChild(CCNode* child, bool cleanup)
{
	for (CHILDS_ITER iter = childs.begin(); iter != childs.end();++iter)
	{
		if (child == *iter)
		{
			child->onExitTransitionDidStart();
			child->onExit();
			child->cleanup();
			child->setParent(NULL);
			child->release();
			childs.erase(iter);
			return;
		}
	}
}
/**
 * 增加怪物 只有一个格子的阻挡属性
 */
void RpgMap::addMonster(RpgMonster *monster)
{
	do{
		for ( NEW_MONSTERS_MAP_ITER iter = newMonsters.rbegin(); iter != newMonsters.rend();++iter)
		{
			stMonstersYSort &sort = *iter;
			for (NEW_MONSTERS_ITER pos = sort.monsters.begin(); pos != sort.monsters.end();++pos)
			{
				if (*pos == monster)
				{
					return;
				}
			}
		}
		GridPoint point = getGridPointByPixelPoint(monster->getPosition());
		int index = getIndexByGridPoint(point);
		if (-1 != index)
		{
			setBlock(index,MONSTER_BLOCK);
		}
		addMonsterByGridPoint(monster,point);
		monster->retain();
		monster->setParent(this);
	  //  if( m_bIsRunning )
		{
			monster->onEnter();
			monster->onEnterTransitionDidFinish();
		}
	}while(false);
	return;
	for ( MONSTERS_MAP_ITER iter = monsters.rbegin(); iter != monsters.rend();++iter)
	{
		for (MONSTERS_ITER pos = iter->begin(); pos != iter->end();++pos)
		{
			if (*pos == monster)
			{
				return;
			}
		}
	}
	GridPoint point = getGridPointByPixelPoint(monster->getPosition());
	int index = getIndexByGridPoint(point);
	if (-1 != index)
	{
		setBlock(index,MONSTER_BLOCK);
	}
	addMonsterByGridPoint(monster,point);
	monster->retain();
	monster->setParent(this);
  //  if( m_bIsRunning )
    {
        monster->onEnter();
        monster->onEnterTransitionDidFinish();
    }
}

/**
 * 删除怪物
 */
void RpgMap::removeMonster(RpgMonster *monster)
{
	for ( std::list<stMonstersYSort >::iterator  iter = newMonsters.begin(); iter != newMonsters.end();++iter)
	{
		stMonstersYSort &sort = *iter;
		bool tag = false;
		for (NEW_MONSTERS_ITER pos = sort.monsters.begin(); pos != sort.monsters.end();++pos)
		{
			if (*pos == monster)
			{
				sort.monsters.erase(pos);
				GridPoint point = getGridPointByPixelPoint(monster->getPosition());
				int index = getIndexByGridPoint(point);
				if (-1 != index)
				{
					clearBlock(index,MONSTER_BLOCK);
				}
				monster->onExitTransitionDidStart();
				monster->onExit();
				monster->cleanup();
				monster->setParent(NULL);
				monster->release();
				tag = true;
				break;
			}
		}
		if (tag)
		{
			if (sort.monsters.empty())
			{
				newMonsters.erase(iter);
			}
			break;
		}
	}
	
	return;
	for ( MONSTERS_MAP_ITER iter = monsters.rbegin(); iter != monsters.rend();++iter)
	{
		for (MONSTERS_ITER pos = iter->begin(); pos != iter->end();++pos)
		{
			if (*pos == monster)
			{
				*pos = NULL;
			}
		}
	}
	GridPoint point = getGridPointByPixelPoint(monster->getPosition());
	int index = getIndexByGridPoint(point);
	if (-1 != index)
	{
		clearBlock(index,MONSTER_BLOCK);
	}
	removeMonsterByGridPoint(monster,point);
	monster->onExitTransitionDidStart();
	monster->onExit();
	monster->cleanup();
	monster->setParent(NULL);
	monster->release();
}
/**
 * 更新怪物
 */
void RpgMap::updateMonster(RpgMonster *monster)
{
	do{
	const GridPoint point = this->getGridPointByPixelPoint(monster->getPosition());
	int oldIndex = getIndexByGridPoint(monster->oldGridPoint);
	int newIndex = getIndexByGridPoint(point);
	if (-1 != oldIndex)
	{
		clearBlock(oldIndex,MONSTER_BLOCK);
	}
	if (-1 != newIndex)
	{
		setBlock(newIndex,MONSTER_BLOCK);
	}
	for ( NEW_MONSTERS_MAP_ITER iter = newMonsters.rbegin(); iter != newMonsters.rend();++iter)
	{
		stMonstersYSort &sort = *iter;
		bool tag = false;
		for (NEW_MONSTERS_ITER pos = sort.monsters.begin(); pos != sort.monsters.end();++pos)
		{
			if (*pos == monster)
			{
				sort.monsters.erase(pos);
				tag = true;
				break;
			}
		}
		if (tag)
		{
			break;
		}
	}
	addMonsterByGridPoint(monster,point);
	this->showMapBlocks();
	}while(false);
	return;
	const GridPoint point = this->getGridPointByPixelPoint(monster->getPosition());
	RpgMonster *tempmpnster = findMonsterByGridPoint(monster->oldGridPoint);
	if (tempmpnster != monster) // 出错啦
	{
		for ( int x = 0; x < monsters.size();++x)
		{
			for (int y = 0; y < monsters[x].size();++y)
			{
				if (monster == monsters[x][y])
				{
					monster->oldGridPoint.x = y;
					monster->oldGridPoint.y = x;
				}
			}
		}
	}
	for ( int x = 0; x < monsters.size();++x)
		{
			for (int y = 0; y < monsters[x].size();++y)
			{
				if (monsters[x][y] == NULL)
				{
					int index = getIndexByGridPoint(GridPoint(y,x));
					if (index < blocks.size() && blocks.at(index))
						this->clearBlock(index,RpgMap::MONSTER_BLOCK);
					if (index < mapblockSprites.size() && mapblockSprites.at(index))
						mapblockSprites[index]->setVisible(false);
				}
			}
		}
	int oldIndex = getIndexByGridPoint(monster->oldGridPoint);
	int newIndex = getIndexByGridPoint(point);
	if (-1 != oldIndex)
	{
		clearBlock(oldIndex,MONSTER_BLOCK);
	}
	if (-1 != newIndex)
	{
		setBlock(newIndex,MONSTER_BLOCK);
	}
	char buffer[1024]={'\0'};
	sprintf(buffer,"set oldIndex:%u newIndex:%u\n",oldIndex,newIndex);
//	infoLog->setString(buffer);
	removeMonsterByGridPoint(monster,monster->oldGridPoint);
	addMonsterByGridPoint(monster,point);
	this->showMapBlocks();
}
int RpgMap::getMonsterCount()
{
	int count = 0;
	for ( NEW_MONSTERS_MAP_ITER iter = newMonsters.rbegin(); iter != newMonsters.rend();++iter)
	{
		stMonstersYSort &sort = *iter;
		for (NEW_MONSTERS_ITER pos = sort.monsters.begin(); pos != sort.monsters.end();++pos)
		{
			if (*pos && (*pos)->isVisible())
			{
				count ++;
			}
		}
	}
	return  count;
}
void RpgMap::preUpdateMonster(RpgMonster *monster,const GridPoint &nowPoint)
{
	const GridPoint point = nowPoint;
	RpgMonster *tempmpnster = findMonsterByGridPoint(monster->oldGridPoint);
	if (tempmpnster != monster) // 出错啦
	{
		for ( int x = 0; x < monsters.size();++x)
		{
			for (int y = 0; y < monsters[x].size();++y)
			{
				if (monster == monsters[x][y])
				{
					monster->oldGridPoint.x = y;
					monster->oldGridPoint.y = x;
				}
			}
		}
	}
	for ( int x = 0; x < monsters.size();++x)
		{
			for (int y = 0; y < monsters[x].size();++y)
			{
				if (monsters[x][y] == NULL)
				{
					int index = getIndexByGridPoint(GridPoint(y,x));
					if (index < blocks.size() && blocks.at(index))
						this->clearBlock(index,RpgMap::MONSTER_BLOCK);
					if (index < mapblockSprites.size() && mapblockSprites.at(index))
						mapblockSprites[index]->setVisible(false);
				}
			}
		}
	int oldIndex = getIndexByGridPoint(monster->oldGridPoint);
	int newIndex = getIndexByGridPoint(point);
	
	if (-1 != newIndex)
	{
		setBlock(newIndex,MONSTER_BLOCK);
	}
	
	this->showMapBlocks();
}
/**
* 通过网格获取怪物
*/
RpgMonster *RpgMap::findMonsterByGridPoint(const GridPoint &point)
{
	if (point.y < monsters.size() && point.x < monsters[point.y].size())
		return monsters[point.y][point.x];
	return NULL;
}
void RpgMap::addMonsterByGridPoint(RpgMonster *monster,const GridPoint &point)
{
	if (point.isInvalid())  return;
	bool tag = false;
	stMonstersYSort *newsort = NULL;
	// 先排序y轴的 从低到高
	for (std::list<stMonstersYSort >::iterator iter = newMonsters.begin(); iter != newMonsters.end();++iter)
	{
		stMonstersYSort &sort = *iter;
		if (sort.yValue == point.y)
		{
			// 直接插入
			newsort = &(*iter);
			tag = true;
		}
		else if (point.y < sort.yValue) 
		{
			//需要插入新的节点
			stMonstersYSort newSort;
			std::list<stMonstersYSort >::iterator pos = newMonsters.insert(iter,newSort);
			newsort = &(*pos);
			tag = true;
		}
	}
	if (!newsort)
	{
		stMonstersYSort newSort;
		std::list<stMonstersYSort >::iterator pos = newMonsters.insert(newMonsters.end(),newSort);
		newsort = &(*pos);
		tag = true;
	}
	if (tag)
	{
		tag = false;
		for (NEW_MONSTERS_ITER pos = newsort->monsters.begin(); pos != newsort->monsters.end();++pos)
		{
			RpgMonster *tempmonster = *pos;
			
			if (tempmonster && point.x <= tempmonster->getGridPoint().x)
			{
				tag = true;
				newsort->monsters.insert(pos,monster);
				monster->setPosition(GetPixelPointByGridPoint(point));
				monster->oldGridPoint = point;
				break;
			}
		}
		if (!tag)
		{
			newsort->monsters.push_back(monster);
			monster->setPosition(GetPixelPointByGridPoint(point));
			monster->oldGridPoint = point;
		}
	}
	return;
	if (point.y >= monsters.size())
	{
		monsters.resize(point.y + 1);
	}
	if (point.x >= monsters[point.y].size())
	{
		monsters[point.y].resize(point.x+1);
	}
	monsters[point.y][point.x] = monster;
	monster->setPosition(GetPixelPointByGridPoint(point));
	monster->oldGridPoint = point;
}
void RpgMap::removeMonsterByGridPoint(RpgMonster *monster,const GridPoint &point)
{
	if (point.isInvalid())  return;
	if (point.y < monsters.size() && point.x < monsters[point.y].size())
	{
		if (monsters[point.y][point.x] == monster)
		{
			monsters[point.y][point.x] = NULL;
		}
	}
}
/**
* 获取周围8方向上是否有有效点 若有多个随机取一个
* \param point 预测点
* \param size 所占的格子数
* \return 当前有效的点
*/
GridPoint RpgMap::getValidGridPoint(const GridPoint &point,const CCSize &size)
{
	static GridPoint dPoints[] = {
		GridPoint(1,0), // 右
		GridPoint(1,1), // 右上
		GridPoint(0,1), // 上
		GridPoint(-1,1), // 左上
		GridPoint(-1,0), // 左
		GridPoint(-1,-1), // 左下
		GridPoint(-1,0), // 下
		GridPoint(-1,1), // 右下
	};

	return GridPoint(0,0);
}
GridPoint RpgMap::getValidGridPoint(const GridPoint &point,int blockType)
{
	static GridPoint dPoints[] = {
		GridPoint(1,0), // 右
		GridPoint(1,1), // 右上
		GridPoint(0,1), // 上
		GridPoint(-1,1), // 左上
		GridPoint(-1,0), // 左
		GridPoint(-1,-1), // 左下
		GridPoint(-1,0), // 下
		GridPoint(-1,1) // 右下
	};
	static int iMax = 13;
	for(int step = 1; step < iMax;step++)
	{
		GridPoint tempPoint = point;
		std::vector<GridPoint> points;
		for (int i = 0; i < 8;i++)
		{
			tempPoint = GridPoint(dPoints[i].x * step + point.x ,dPoints[i].y * step + point.y);
			if (!this->checkBlock(this->getIndexByGridPoint(tempPoint),blockType) && !tempPoint.isInvalid())
			{
				points.push_back(tempPoint);
			}
		}
		if (!points.empty())
		{
			int index = CCRANDOM_0_1() * points.size();
			if (index < points.size())
			{
				tempPoint = points.at(index);
			}
			return tempPoint;
		}
	}
	return GridPoint(-1,-1);
}

/**
 * 获取有效的攻击点
 */
GridPoint  RpgMap::getValidAtkPoint(const GridPoint &point)
{
	static GridPoint dPoints[] = {
		GridPoint(1,0), // 右
		GridPoint(1,1), // 右上
		GridPoint(0,1), // 上
		GridPoint(-1,1), // 左上
		GridPoint(-1,0), // 左
		GridPoint(-1,-1), // 左下
		GridPoint(-1,0), // 下
		GridPoint(-1,1) // 右下
	};
	static int iMax = 6;
	for(int step = 1; step < iMax;step++)
	{
		GridPoint tempPoint = point;
		std::vector<GridPoint> points;
		for (int i = 0; i < 8;i++)
		{
			tempPoint = GridPoint(dPoints[i].x * step + point.x ,dPoints[i].y * step + point.y);
			if (!this->checkBlock(this->getIndexByGridPoint(tempPoint),RpgMap::MONSTER_BLOCK) && !tempPoint.isInvalid())
			{
				if (this->checkValidAtkPoint(point,tempPoint))
					points.push_back(tempPoint);
			}
		}
		if (!points.empty())
		{
			tempPoint = points.at(CCRANDOM_0_1() * points.size());
			return tempPoint;
		}
	}
	return GridPoint(-1,-1);
	return GridPoint(-1,-1);
}
/**
 * 检查点是否是可以攻击的点
 */
bool RpgMap::checkValidAtkPoint(const GridPoint &src,const GridPoint &point)
{
	return true;
	if (src.x != point.x && point.y >= src.y) return true;
	return false;
}
/**
* 通过像素点 获取网格点
* \param point 像素点
* \return 网格点
*/
GridPoint RpgMap::getGridPointByPixelPoint(const CCPoint &point)
{
	return GridPoint(point.x / gridSize.width,point.y / gridSize.height);
}
/**
* 通过网格点 获取像素点
* \param point 网格点
* \return 像素点
*/
CCPoint RpgMap::GetPixelPointByGridPoint(const GridPoint &point)
{
	return ccp(point.x * gridSize.width,point.y * gridSize.height);
}
/**
* 通过网格点 获取引索
* \param point 网格点
* \return 引索
*/
int RpgMap::getIndexByGridPoint(const GridPoint &point)
{
	return point.x * mapSize.height + point.y;
}
/**
 * 根据编号获取网格点
 */
GridPoint RpgMap::getGridPointByIndex(int index)
{
	return GridPoint(index / mapSize.height,index %  (int)mapSize.height);
}
/**
 * 获取周围的目标
 * \param point 当前点
 * \param distance 距离
 * \param exec 回调
 */
void RpgMap::execTargets(const GridPoint &point,int distance,stExecEachTarget *exec)
{
	for ( std::list<stMonstersYSort >::iterator  iter = newMonsters.begin(); iter != newMonsters.end();++iter)
	{
		stMonstersYSort &sort = *iter;
		for (NEW_MONSTERS_ITER pos = sort.monsters.begin(); pos != sort.monsters.end();++pos)
		{
			RpgMonster *monster = *pos;
			if (monster)
			{
				GridPoint oldPoint = monster->getGridPoint();
				if (abs(oldPoint.x - point.x) < distance && abs(oldPoint.y - point.y) < distance)
				{
					exec->exec(monster);
				}
			}
		}
	
	}
	return;
	GridPoint startPoint = GridPoint(point.x - distance/2,point.y - distance/2);
	if (startPoint.x <0) startPoint.x = 0;
	if (startPoint.y <0) startPoint.y = 0;
	for (int x = 0; x < distance;x++)
		for (int y = 0; y < distance;y++)
		{
			GridPoint tempPoint = GridPoint(startPoint.x + x,startPoint.y + y);
			RpgMonster *monster = this->findMonsterByGridPoint(tempPoint);
			if (monster)
			{
				exec->exec(monster);
			}
		}
}
/**
 * 设置阻挡点
 */
void RpgMap::setBlock(int index,int blockType)
{
	if (index < 0) return;
	if (index >= blocks.size())
	{
		blocks.resize(index+1);
	}
	int tag = blocks[index];
	blocks[index] |= blockType;
}
/**
 * 清除阻挡点
 */
void RpgMap::clearBlock(int index,int blockType)
{
	if (index < 0) return;
	if (index >= blocks.size())
	{
		blocks.resize(index+1);
	}
	unsigned int tag =  ~blockType;
	blocks[index] &= (tag);
	tag = blocks[index];
}
/**
 * 检查阻挡点
 */
bool RpgMap::checkBlock(int index,int blockType)
{
	if (index < 0) return false;
	if (index >= blocks.size())
	{
		blocks.resize(index+1);
	}
	if (index < blocks.size())
	{
		unsigned int tag1 = blocks[index] ;
		unsigned int tag = (blocks[index] & blockType);
		if (tag)
		{
			return true;
		}
		else
			return false;
	}
	return false;
}
/**
 * 展示阻挡点
 */
void RpgMap::showMapBlocks()
{
	return;
	std::stringstream logInfo;
	for (unsigned int i = 0; i < blocks.size();i++)
	{
		if (i < mapblockSprites.size() && mapblockSprites[i])
		{
			mapblockSprites[i]->setVisible(false);
		}
		if (blocks[i])
		{
			if (i >= mapblockSprites.size())
			{
				mapblockSprites.resize(i+1);
			}
			if (mapblockSprites[i] == NULL)
			{
				mapblockSprites[i] = MapBlockSprite::create();
				std::stringstream ss;
				GridPoint nowPos = this->getGridPointByIndex(i);
				ss <<""<< nowPos.x <<"," << nowPos.y;
				CCLabelTTF *label = CCLabelTTF::create(ss.str().c_str(),"Arial",12);
				mapblockSprites[i]->addChild(label);
				this->addChild(mapblockSprites[i]);
				mapblockSprites[i]->setPosition(ccpAdd(this->GetPixelPointByGridPoint(getGridPointByIndex(i)),ccp(16,16)));
				CCLog("create%d",i);
			}
			
			mapblockSprites[i]->show(blocks[i]);
			mapblockSprites[i]->setVisible(true);
			logInfo<<"<" << blocks[i] <<">";
		}
	}
	CCLOG("%s\n",logInfo.str().c_str());
}
///////////////////////////////// 地形相关 ////////////////////////////////////////////
/**
 * 增加地表 不增加格子的阻挡信息
 */
void RpgMap::addTerrain(RpgTerrain *terrain)
{ 
	terrains.push_back(terrain);
	terrain->retain();
	terrain->setParent(this);
  //  if( m_bIsRunning )
    {
        terrain->onEnter();
        terrain->onEnterTransitionDidFinish();
    }
//	addChild(terrain);
}
/**
* 删除地表
*/
void RpgMap::removeTerrain(RpgTerrain *terrain)
{
	for (TERRAINS_ITER iter = terrains.begin(); iter != terrains.end(); ++iter)
	{
		if (terrain == *iter)
		{
			//terrain->removeFromParentAndCleanup(true);
			terrain->onExitTransitionDidStart();
			terrain->onExit();
			terrain->cleanup();
			terrain->setParent(NULL);
			terrain->release();
			terrains.erase(iter);
			break;
		}
	}
}
///////////////////////////////// 建筑物相关///////////////////////////////////////////
/**
* 测试是否能够放置建筑物
* \param build 建筑物
* \param gx 格子横id
* \param gy 格子竖id
* \return 是否能够放置
*/
bool RpgMap::canPlaceBuild(RpgBuild * build,int gx,int gy)
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
bool RpgMap::checkIndexInBuild(RpgBuild *build,int gx,int gy)
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
bool RpgMap::placeBuild(RpgBuild *build,int gx,int gy)
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
RpgBuild *RpgMap::pickBuild(float tx,float ty)
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
RpgBuild *RpgMap::pickBuild(const GridPoint &point)
{
	for (BUILDS_ITER iter = _builds.begin(); iter != _builds.end();++iter)
	{
		if ((*iter)->checkIn(point.x * gridSize.width,point.y * gridSize.height))
		{
			return *iter;
		}
	}
	return NULL;
}
/**
* 获取引索值
*/
int RpgMap::getIndex(int gx,int gy)
{
	return gx + gy * mapSize.width;
}

/**
* 增加建筑物
*/
void RpgMap::addBuild(RpgBuild *build)
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
 * 删除建筑物
 */
void RpgMap::removeBuild(RpgBuild *build)
{
	for (BUILDS_ITER iter = _builds.begin(); iter != _builds.end();++iter)
	{
		if (build == *iter)
		{
			clearBlock(build->info);
			_builds.erase(iter);
			build->removeFromParentAndCleanup(true);
			return;
		}
	}
	
}
/**
* 清楚阻挡点
*/
void RpgMap::clearBlock(const RpgBuildInfo &build)
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
void RpgMap::setBlock(const RpgBuildInfo &build,int unqiueId)
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
CCPoint RpgMap::getPixelPoint(int gx,int gy)
{
	return ccp(gx * 32,gy * 32);
}
/**
 * 处理点击
 */
bool RpgMap::doTouch(int touchType,const CCPoint&touchPoint)
{
	switch(doTouchType)
	{
		case DO_TOUCH_BUILD: // 建筑物
			{
				return this->doTouchPlace(touchType,touchPoint);
			}break;
		case DO_TOUCH_MOVE: // 移动
			{
				//return this->doTouchMove(touchType,touchPoint);
			}break;
		case DO_TOUCH_SET: // 设置
			{
				return this->doTouchSet(touchType,touchPoint);
			}break;
	}
	return false;
}
/**
* 处理touch 放置建筑物
*/
bool RpgMap::doTouchPlace(int touchType,const CCPoint&point )
{
	CCPoint touchPoint = this->convertToNodeSpace(point);
	switch(touchType)
	{
		case RpgUI::TOUCH_DOWN:
		{
			nowMoveBuild = this->pickBuild(touchPoint.x,touchPoint.y);
			if (nowMoveBuild && nowMoveBuild->info.opType != RpgBuildInfo::CAN_MOVE)
			{
				doTouchBuild(nowMoveBuild);
				nowMoveBuild = NULL;
			}
			if (nowMoveBuild)
			{
				nowMoveBuild->setZOrder(2);
				nowMoveBuild->retain();
				return true;
			}
			RpgMonster *monster = this->pickMonster(touchPoint);
			if (monster && ((RpgMonster*)monster) != ui->player)
			{
				if (ui && ui->player)
					ui->player->doAttack(monster);
				return true;
			}
		}break;
		case RpgUI::TOUCH_MOVE:
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
				return true;
			}
		}break;
		case RpgUI::TOUCH_END:
		{
			if (nowMoveBuild)
			{
				nowMoveBuild->setZOrder(0);
				if (canPlaceBuild(nowMoveBuild,touchPoint.x / 32 ,touchPoint.y / 32))
					nowMoveBuild->showNonBack();
				nowMoveBuild->release();
			}
			else
			{
				RpgMonster *monster = this->pickMonster(touchPoint);
				if (monster)
				{
					
				}
			}
			nowMoveBuild = NULL;
		}break;
	}
	return false;
}
bool RpgMap::doTouchMove(int touchType,const CCPoint &touchPoint)
{
	switch(touchType)
	{
		case RpgUI::TOUCH_DOWN:
		{
			nowTouchPoint = touchPoint;
		}break;
		case RpgUI::TOUCH_MOVE:
		{
			CCPoint nowPoint = this->getPosition();
			this->setPosition(ccp(nowPoint.x + touchPoint.x - nowTouchPoint.x,
						  nowPoint.y + touchPoint.y - nowTouchPoint.y));
			nowTouchPoint = touchPoint;
		}break;
		case RpgUI::TOUCH_END:
		{

		}break;
	}
	return false;
}
bool RpgMap::doTouchSet(int touchType,const CCPoint &touchPoint)
{
	CCPoint pos = this->convertToNodeSpace(touchPoint);
	switch(touchType)
	{
		case RpgUI::TOUCH_DOWN:
		{
		}break;
		case RpgUI::TOUCH_MOVE:
		{
		}break;
		case RpgUI::TOUCH_END:
		{
			// 设定阻挡点信息
			int index = getIndexByGridPoint(this->getGridPointByPixelPoint(pos));
			if (this->checkBlock(index,this->nowSetBlockType))
				this->clearBlock(index,this->nowSetBlockType);
			else
				this->setBlock(index,this->nowSetBlockType);
			showMapBlocks();
		}break;
	}
	return false;
}
RpgBuildInfo RpgMap::findFarmInfoById(int id)
{
	BUILD_INFOS_ITER iter = buildInfos.find(id);
	if (iter != buildInfos.end())
	{
		return iter->second;
	}
	return RpgBuildInfo();
}

void RpgMap::showBlocks()
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
			sprite->setPosition(ccp((index % (int)mapSize.width) * 32 - 16,(index / (int)mapSize.height) * 32-16));
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
/**
 * 将信息保存到文件中
 */
void RpgMap::saveBlock(const char *blockFile)
{
	serialize::Stream ss(&blocks[0],blocks.size());
	ss.toBase64();
	ss.writeToFile(blockFile);
}
/**
 * 从文件中加载阻挡点信息 
 */
void RpgMap::loadBlock(const char *blockFile)
{
	serialize::Stream ss;
	ss.readFromFile(blockFile);
	ss.fromBase64();
	blocks.resize(ss.size());
	if (blocks.size())
	memcpy(&blocks[0],ss.c_str(),ss.size());
}

/**
 * 定时行为 在出生点周围 生成怪物
 */
void RpgMap::doTick()
{
	cc_timeval nowTime;
	CCTime::gettimeofdayCocos2d(&nowTime, NULL);
	for (GEN_LOGICS_ITER iter = genLogics.begin(); iter != genLogics.end();++iter)
	{
		stGenLogic&  genLogic = *iter;
		float tapTime = CCTime::timersubCocos2d(&genLogic.lastGenTime,&nowTime) / 1000;
		if (tapTime >= genLogic.timer && (genLogic.maxCount > 0 || genLogic.maxCount <=-1) )
		{
			// 处理生存时间
			CCPoint pos = genLogic.getRandomPos();
			if (getMonsterCount() < 200)
			{
				RpgMonster *monster = RpgMonster::create(genLogic.monsterid);
				if (monster)
				{
					monster->setPosition(pos);
					this->addMonster(monster);
					showMapBlocks();
				}
				genLogic.maxCount --;
				genLogic.lastGenTime = nowTime;
			}
		}
	}
	if (nowStep.isInvalid() && stepLogics.size())
	{
		nowStep = *stepLogics.begin();
		stepLogics.pop_front();
		nowStep.init(this);
	}
	if (nowStep.isValid())
	{
		nowStep.doTick();
	}
	if (nowStep.isValid() && nowStep.isOver())
	{
		nowStep.setInvalid();
		ui->stepEnd->setVisible(true);
	}
}
void RpgMap::doTouchBuild(RpgBuild *build)
{
	RpgItem * item = RpgItem::create(0);
	ui->playerBag->addItem(item->info.itemId,1);
	CCSprite * test = CCSprite::create("test.png");
	ui->addChild(test);
	test->setPosition(build->getPosition());
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	removeBuild(build);
	test->runAction(CCSequence::create(CCDelayTime::create(0.1),CCMoveTo::create(1.0f,ccp(visibleSize.width,300)),
			CCCallFuncND::create(this, callfuncND_selector(RpgMap::actionEnd_removeTemp), (void*)test),NULL));
}
void RpgMap::actionEnd_removeTemp(CCNode *pSender,void *arg)
{
	CCSprite *temp = (CCSprite*) arg;
	if (temp)
	{
		temp->removeFromParentAndCleanup(true);
	}
}

bool RpgMap::checkCollide(RpgMonster *monster,const GridPoint &point)
{
	for ( NEW_MONSTERS_MAP_ITER iter = newMonsters.rbegin(); iter != newMonsters.rend();++iter)
	{
		for (NEW_MONSTERS_ITER pos = iter->monsters.begin(); pos != iter->monsters.end();++pos)
		{
			if (*pos)
			{
				RpgMonster *destmonster = *pos;
				if (monster != destmonster)
				{
					if (destmonster->checkIn(monster->getGridPoint()))
						return true;
				}
			}
		}
	}
	return false;
	for ( MONSTERS_MAP_ITER iter = monsters.rbegin(); iter != monsters.rend();++iter)
	{
		for (MONSTERS_ITER pos = iter->begin(); pos != iter->end();++pos)
		{
			if (*pos)
			{
				RpgMonster *destmonster = *pos;
				if (monster != destmonster)
				{
					if (destmonster->checkIn(monster->getGridPoint()))
						return true;
				}
			}
		}
	}
	return false;
}
RpgMonster *RpgMap::pickMonster(const CCPoint& touchPoint)
{
	for ( NEW_MONSTERS_MAP_ITER iter = newMonsters.rbegin(); iter != newMonsters.rend();++iter)
	{
		for (NEW_MONSTERS_ITER pos = iter->monsters.begin(); pos != iter->monsters.end();++pos)
		{
			if (*pos)
			{
				RpgMonster *destmonster = *pos;
				if (destmonster &&destmonster->isVisible())
				{
					if (destmonster->checkIn(this->getGridPointByPixelPoint(touchPoint),2))
						return destmonster;
				}
			}
		}
	}
	return NULL;
}
///////////////////////////////// 建筑物结束///////////////////////////////////////////
NS_CC_END