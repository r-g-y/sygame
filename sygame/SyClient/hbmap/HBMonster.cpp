#include "HBMonster.h"
#include "HBMap.h"
#include "CalcDir.h"
#include "TFActionManager.h"
NS_CC_BEGIN
void HBMonsterInfo::initWithNode(script::tixmlCodeNode *node)
{
	if (node)
	{
		node->getAttr("id",uniqueId);
		node->getAttr("x",x);
		node->getAttr("y",y);
		node->getAttr("atktime",this->_atkTime);
		node->getAttr("atkdistance",this->_atkDistance);
		node->getAttr("idleid",this->idleActionId);
		node->getAttr("walkid",this->walkActionId);
		node->getAttr("atkskillid",this->atkActionId);
		node->getAttr("beatkskillid",this->beAttackActionId);
		node->getAttr("deathskillid",this->deathActionId);
		node->getAttr("eyeshort",this->_eyeshort);
		node->getAttr("step",this->_step);
		node->getAttr("type",this->monsterType);
		node->getAttr("hp",this->_hp);
		node->getAttr("maxhp",this->_maxHp);
	}
}
HBMonster* HBMonster::create()
{
	HBMonster *pRet = new HBMonster();
    if (pRet && pRet->init())
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

bool HBMonster::initWithNode(script::tixmlCodeNode *node)
{
	CCSprite::initWithFile("test.png");
	// <monster id="0" x="" y="" atktime="" atkdistance="" atkskillid="" idleid="" walkid="" eyeshort=""/>
	if (node)
	{
		monsterInfo.initWithNode(node);
		this->setPosition(ccp(monsterInfo.x,monsterInfo.y));
	}
	updateHp(0);
	start(IDLE_ACTION);
	CCTime::gettimeofdayCocos2d(&_lastActionTime, NULL);
	CCTime::gettimeofdayCocos2d(&lastAtkTime, NULL);
	return true;
}
HBMonster * HBMonster::createFromNode(script::tixmlCodeNode *node)
{
	HBMonster *pRet = new HBMonster();
    if (pRet && pRet->initWithNode(node))
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
bool HBMonster::initWithInfo(const HBMonsterInfo &info)
{
	CCSprite::initWithFile("test.png");
	if (node)
	{
		monsterInfo = info;
		this->setPosition(ccp(monsterInfo.x,monsterInfo.y));
	}
	updateHp(0);
	start(IDLE_ACTION);
	CCTime::gettimeofdayCocos2d(&_lastActionTime, NULL);
	CCTime::gettimeofdayCocos2d(&lastAtkTime, NULL);
	return true;
}
HBMonster * HBMonster::createFromInfo(const HBMonsterInfo &info)
{
	HBMonster *pRet = new HBMonster();
    if (pRet && pRet->initWithInfo(info))
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
bool HBMonster::init()
{
	CCSprite::init();
	CCTime::gettimeofdayCocos2d(&_lastActionTime, NULL);
	CCTime::gettimeofdayCocos2d(&lastAtkTime, NULL);
	start(IDLE_ACTION);
	return true;
}
void HBMonster::tick()
{
	if (!map) return;
	if (!this->isVisible()) return;
	cc_timeval nowTime;
	CCTime::gettimeofdayCocos2d(&nowTime, NULL);
	float tapTime = CCTime::timersubCocos2d(&_lastActionTime,&nowTime) / 1000;
	if (monsterInfo._atkTime)
	{
		if (tapTime > this->monsterInfo._atkTime)
		{
			_lastActionTime = nowTime;
			if (monsterInfo._hp < monsterInfo._maxHp && !isNowAction(ACTION_DEATH))
			{
				monsterInfo._hp ++;
				updateHp(0);
			}
			// 处理攻击
			//doAttack();
		}
	}
}
/**
* 执行攻击
*/
void HBMonster::doAttack()
{
	// 在可视范围内查找到目标 
	HBMonster * monster = map->getEneryByID(_nowTargetId);
	if (!monster)
	{
		std::vector<HBMonster*> monsters;
		map->findEneries(this,this->monsterInfo._eyeshort,monsters);
		if (monsters.size())
		{
			monster = monsters.at(0); // 取第一个
			_nowTargetId = monster->tempId;
		}
		else
		{
			_nowTargetId = -1;
			return;
		}
	}
	if (monster)
	{
		float distance = this->calcDistance(monster);

		if (checkInXLine(monster) && distance > 0 && distance < this->monsterInfo._atkDistance)
		{
			if (this->oldActionId == ACTION_BE_ATTACK || this->oldActionId == ACTION_DEATH)
			{
				return;
			}
			this->tryNextAction(ACTION_ATTACK); // 向某个方向播放技能
			monster->tryNextAction(ACTION_BE_ATTACK);
		}
		else
		{
			if (monsterInfo.monsterType != 0)
			{
				_nowDestination = monster->getPkDestination(this,0);
			
				this->tryNextAction(ACTION_MOVE); // 尝试移动
			}
		}
	}
}
bool HBMonster::checkInXLine(HBMonster *monster)
{
	if (!monster) return false;
	zPos srcPos = zPos(this->getPositionX() / 32,this->getPositionY() / 32);
	zPos destPos = zPos(monster->getPositionX() / 32,monster->getPositionY() / 32);
	return srcPos.y == destPos.y;
}
CCFiniteTimeAction* HBMonster::v_makeAction(int actionid)
{
	// 根据行为获取Action
	switch(actionid)
	{
		case ACTION_ATTACK:
		{
			HBMonster * monster = map->getEneryByID(_nowTargetId);
			if (monster && monster->isVisible())
			{
				_nowDestination = monster->getPosition();
				int dir = calcMonsterDir(_nowDestination);
				if (dir != -1 && map->checkInMap(_nowDestination.x,_nowDestination.y))
				{
					oldActionId = monsterInfo.atkActionId;
					return TFActionManager::getMe().getAction(monsterInfo.atkActionId,dir,(_nowDestination),NULL);
				}
			}
			else
			{
				_nowTargetId = -1;
				oldActionId = ACTION_ATTACK;
				return TFActionManager::getMe().getAction(monsterInfo.atkActionId,oldDir,(_nowDestination),NULL);
			}
		}break;
		case ACTION_MOVE:
		{
			int dir = calcMonsterDir(_nowDestination);
			if (dir != -1 && map->checkInMap(_nowDestination.x,_nowDestination.y))
			{
#if 0
				oldDir = dir;
				oldActionId = ACTION_MOVE;
				return TFActionManager::getMe().getAction(walkActionId,dir,getStepPointBy(_nowDestination),NULL);
#endif
				zPos out;
				zPos srcPos = map->getGridPoint(this->getPositionX(),this->getPositionY());
				zPos destPos = map->getGridPoint(_nowDestination.x,_nowDestination.y);
				if (this->getNextPos(srcPos,destPos,out))
				{
					CCPoint dest = map->getPointByGrid(out);
					oldDir = dir;
					oldActionId = ACTION_MOVE;
				//	map->clearBlock(srcPos.x,srcPos.y);
				//	map->setBlock(out.x,out.y);
					return TFActionManager::getMe().getAction(monsterInfo.walkActionId,dir,dest,NULL);
				}

			}
		}break;
		case ACTION_BE_ATTACK:
		{
			oldActionId = ACTION_BE_ATTACK;
			return TFActionManager::getMe().getAction(monsterInfo.beAttackActionId,oldDir,(_nowDestination),NULL);
		}break;
		case ACTION_DEATH: // 死亡动画
		{
			oldActionId = ACTION_DEATH;
			return TFActionManager::getMe().getAction(monsterInfo.deathActionId,oldDir,(_nowDestination),NULL);
		}break;
	}
	oldActionId = IDLE_ACTION;
	return TFActionManager::getMe().getAction(this->monsterInfo.idleActionId,oldDir,ccp(0,0),NULL);
}
CCPoint HBMonster::getStepPointBy(const CCPoint &dest)
{
	int dir = calcDir(dest);
	switch(dir)
	{
	case CalcDir::_DIR_DOWN:return ccpAdd(this->getPosition(),ccp(0,monsterInfo._step));
	case CalcDir::_DIR_LEFT:return ccpAdd(this->getPosition(),ccp(-monsterInfo._step,0));
	case CalcDir::_DIR_RIGHT:return ccpAdd(this->getPosition(),ccp(monsterInfo._step,0));
	case CalcDir::_DIR_UP:return ccpAdd(this->getPosition(),ccp(0,-monsterInfo._step));
	case CalcDir::_DIR_DOWN_LEFT:return ccpAdd(this->getPosition(),ccp(-monsterInfo._step,monsterInfo._step));
	case CalcDir::_DIR_RIGHT_DOWN:return ccpAdd(this->getPosition(),ccp(monsterInfo._step,monsterInfo._step));
	case CalcDir::_DIR_UP_RIGHT:return ccpAdd(this->getPosition(),ccp(monsterInfo._step,-monsterInfo._step));
	case CalcDir::_DIR_LEFT_UP:return ccpAdd(this->getPosition(),ccp(-monsterInfo._step,-monsterInfo._step));
	}
	return getPosition();
}
CCPoint HBMonster::getStepPointByDir(int dir)
{
	switch(dir)
	{
		case CalcDir::_DIR_DOWN:return ccpAdd(this->getPosition(),ccp(0,monsterInfo._step));
		case CalcDir::_DIR_LEFT:return ccpAdd(this->getPosition(),ccp(-monsterInfo._step,0));
		case CalcDir::_DIR_RIGHT:return ccpAdd(this->getPosition(),ccp(monsterInfo._step,0));
		case CalcDir::_DIR_UP:return ccpAdd(this->getPosition(),ccp(0,-monsterInfo._step));
		case CalcDir::_DIR_DOWN_LEFT:return ccpAdd(this->getPosition(),ccp(-monsterInfo._step,monsterInfo._step));
		case CalcDir::_DIR_RIGHT_DOWN:return ccpAdd(this->getPosition(),ccp(monsterInfo._step,monsterInfo._step));
		case CalcDir::_DIR_UP_RIGHT:return ccpAdd(this->getPosition(),ccp(monsterInfo._step,-monsterInfo._step));
		case CalcDir::_DIR_LEFT_UP:return ccpAdd(this->getPosition(),ccp(-monsterInfo._step,-monsterInfo._step));
	}
	return getPosition();
}
void HBMonster::v_putAction(CCFiniteTimeAction *action)
{
	if (action)
	{
		CCAction *seqaction = CCSequence::create(action,
					CCCallFunc::create(this, callfunc_selector(HBMonster::conbineActionEnd)),NULL);
		this->runAction(seqaction);
	}
}
void HBMonster::conbineActionEnd()
{
	if (isNowAction(ACTION_BE_ATTACK))
	{
		int hp = CCRANDOM_0_1() * 5 + 2;
		monsterInfo._hp -= hp;
		if (monsterInfo._hp < 0)
		{
			monsterInfo._hp = 0;
		}
		updateHp(hp);
		if (monsterInfo._hp <= 0)
		{
			this->tryConbineAction(ACTION_DEATH); // 执行死亡
			clearAction(ACTION_BE_ATTACK);
		}
	}
	if (isNowAction(ACTION_DEATH)) // 当前的动作 在putConbineAction 后 失效
	{
		this->setVisible(false);
		return;
	}
	if ( isNowAction(ACTION_ATTACK) )
	{
		std::vector<HBMonster*> monsters;
		map->findEneries(this,this->monsterInfo._eyeshort,monsters);
		HBMonster *monster = NULL;
		if (monsters.size())
		{
			monster = monsters.at(0); // 取第一个
			_nowTargetId = monster->tempId;
			
			for ( std::vector<HBMonster*>::iterator iter = monsters.begin(); iter != monsters.end();++iter)
			{
				HBMonster * temp = *iter;
				if (temp && this->oldActionId != ACTION_BE_ATTACK && temp->oldActionId != ACTION_DEATH && temp->monsterInfo._hp > 0)
				{
					temp->_nowDestination = temp->calcBeAtkPosition(this);
					temp->tryNextAction(ACTION_BE_ATTACK);
				}
			}
		}
		else
		{
			_nowTargetId = -1;
		}
	}
	if (nowMove)
	{
		_nowDestination = getStepPointByDir(nowDir);
		this->tryConbineAction(ACTION_MOVE); // 执行移动
	}
	
	this->putConbineAction();
	/**
	 *检查当前是否需要继续某些操作
	 */
	zPos srcPos = map->getGridPoint(this->getPositionX(),this->getPositionY());
	zPos destPos = map->getGridPoint(_nowDestination.x,_nowDestination.y);
	// 检查是否还需要继续走路
	if (srcPos.x == destPos.x && srcPos.y == destPos.y)
	{
		clearAction(ACTION_MOVE);
	}
	else
	{
		this->tryConbineAction(ACTION_MOVE);
	}
	// 检查是否可以 攻击
	if (tempId != 0)
		doAttack();
	
}
void HBMonster::actionEnd1()
{
	int nowActionType = actionType;

	zPos srcPos = map->getGridPoint(this->getPositionX(),this->getPositionY());
	zPos destPos = map->getGridPoint(_nowDestination.x,_nowDestination.y);
	if (srcPos.x == destPos.x && srcPos.y == destPos.y)
	{
		actionType = IDLE_ACTION;
	}

	CCFiniteTimeAction * action = makeAction(nowActionType);
	if (action != NULL)
	{
		putAction(action);
	}
	else
	{
		// TODO error 中断处理流程
		putAction(makeAction(IDLE_ACTION));
	}
	// 处理行为结束后的逻辑
	switch (oldActionId)
	{
		case ACTION_BE_ATTACK:
		{
			int hp = CCRANDOM_0_1() * 5 + 2;
			monsterInfo._hp -= hp;
			updateHp(hp);
			if (monsterInfo._hp <= 0)
			{
				this->tryNextAction(ACTION_DEATH);
				return;
			}
		}break;
		case ACTION_DEATH:
		{
			
		}break;
	}
	if (monsterInfo._hp <= 0)
	{
		this->setVisible(false);
		return;
	}
	// 尝试设置下一个动作
	switch(nowActionType)
	{
		case ACTION_BE_ATTACK:
		{
			actionType = IDLE_ACTION;
		}break;
		case ACTION_ATTACK:
		{
			actionType = IDLE_ACTION;
		}break;
	}
	// 设置自动攻击
	doAttack();
}
/**
 * 处理操作板的移动pk
 */
void HBMonster::actionEnd()
{
	int nowActionType = actionType;
	if (nowMove)
	{
		_nowDestination = getStepPointByDir(nowDir);
		actionType = this->ACTION_MOVE;
	}
	else
	{
		actionType = IDLE_ACTION;
	}
	CCFiniteTimeAction * action = makeAction(nowActionType);
	if (action != NULL)
	{
		putAction(action);
	}
	else
	{
		putAction(makeAction(IDLE_ACTION));
	}
}
void HBMonster::setPosition(const CCPoint& pos)
{
	CCNode::setPosition(pos);
	if (map)
	{
		map->setReSort();
	}
}
/**
 * 尝试走到该点
 */
void HBMonster::onTouch(const CCPoint& point)
{
	_nowDestination = point;
	CCPoint dest = map->getPointByGrid(map->getGridPoint(point.x,point.y));
	if (map->checkInMap(_nowDestination.x,_nowDestination.y))
	{
		map->drawNowPoint(point,dest);
	}
	else
	{
		map->showMissed(dest);
	}
	this->tryNextAction(ACTION_MOVE); // 尝试移动
}
/**
 * TOUCH 控制移动的新方式
 */
void HBMonster::onTouch(const CCPoint &dest,int moveType)
{
	CCPoint point = map->convertToNodeSpace(dest);
	
	
#ifdef TEST_NEW_OP_MOVE
	static int nowDir = CalcDir::_DIR_RIGHT; // 默认朝右
	static CCPoint nowTouchPoint;
	float nowDistance = ccpDistance(nowTouchPoint,point);
	switch(moveType)
	{
		case ON_TOUCH_DOWN:
		{
			if (nowDistance >= 10)
			{
				CCPoint src = ccp((int)(nowTouchPoint.x / _step),(int)(nowTouchPoint.y / _step)); 
				CCPoint dest = ccp((int)(point.x / _step),(int)(point.y / _step)); 
				nowDir = CalcDir::calcDir(src,dest);
			}
			nowTouchPoint = point;
		}break;
		case ON_TOUCH_MOVE:
		{

		}
		case ON_TOUCH_END:
		{	

		}break;
	}
	if (moveType == ON_TOUCH_END) // 只在down的时候移动
	{
		// 移动到某方向
		_nowDestination = getStepPointByDir(nowDir);
		this->putNextActionId(ACTION_MOVE); // 尝试移动
	}
	if (map)
	{
		map->drawNowPoint(nowTouchPoint);
		map->drawDir(nowDir);
	}
#endif
	opMoveCenter = ccp(100,100);
	switch(moveType)
	{
		case ON_TOUCH_DOWN:
		{
			if (ccpDistance(dest,opMoveCenter) > OP_SIZE / 2) // 只在圆盘内操作有效
			{ 
				nowMove = false;
				return ;
			}
			nowMove = true;
			CCPoint tsrc = ccp((int)(opMoveCenter.x / opMoveTap),(int)(opMoveCenter.y / opMoveTap)); 
			CCPoint tdest = ccp((int)(dest.x / opMoveTap),(int)(dest.y / opMoveTap)); 
			nowDir = CalcDir::calcDir(tsrc,tdest);
			updateOpMoveCenter(dest);
			return;
		}break;
		case ON_TOUCH_MOVE:
		{
			CCPoint tsrc = ccp((int)(opMoveCenter.x / opMoveTap),(int)(opMoveCenter.y / opMoveTap)); 
			CCPoint tdest = ccp((int)(dest.x / opMoveTap),(int)(dest.y / opMoveTap)); 
			nowDir = CalcDir::calcDir(tsrc,tdest);
			updateOpMoveCenter(dest);
			return;
		}
		break;
		case ON_TOUCH_END:
		{	
			if (nowMove)
			{
				nowMove = false;
				return;
			}
		}break;
	}
	if (moveType == ON_TOUCH_END)
	{
		//onTouch(point);
	}
	return;
}
/**
 * 防扎堆算法
 **/
CCPoint HBMonster::getPkDestination(HBMonster *attacker,int dir)
{
	if (attacker)
	{
		if (atktemppoints.size() >= 12) atktemppoints.clear();
		int atkid = atktemppoints.size() / 6 + 1;
		float r = atkid * 128;
		float angle = ((atktemppoints.size() % 6) / 6.0) * 2 * 3.1415926;
		atktemppoints.push_back(attacker->tempId);
		return ccp ( this->getPositionX() + r * cos(angle),this->getPositionY() + r * sin(angle));
	}
	return this->getPosition();
}
void HBMonster::updateOpMoveCenter(const CCPoint &dest)
{
	map->drawNowPoint(dest,opMoveCenter);
	map->drawDir(nowDir);
}
/**
 * 计算距离
 */
float HBMonster::calcDistance(HBMonster *monster)
{
	if (!monster) return 0;
	return ccpDistance(this->getPosition(),monster->getPosition());
}
/**
 * 计算实际方向
 */
int HBMonster::calcDir(const CCPoint &dest)
{
	CCPoint srcg = ccp((int)(getPosition().x / monsterInfo._step),(int)(getPosition().y / monsterInfo._step)); 
	CCPoint destg = ccp((int)(dest.x /monsterInfo._step),(int)(dest.y / monsterInfo._step)); 
	return CalcDir::calcDir(srcg,destg);
}

/**
 * 计算模型方向
 */
int HBMonster::calcMonsterDir(const CCPoint &dest)
{
	CCPoint nowPos = this->getPosition();
	nowPos = ccp(nowPos.x / monsterInfo._step,nowPos.y / monsterInfo._step);
	CCPoint destPos = ccp(dest.x / monsterInfo._step,dest.y / monsterInfo._step);
	if (nowPos.x == destPos.x && nowPos.y == destPos.y)
	{
		return oldDir;
	}
	if (nowPos.x == destPos.x)
	{
		return oldDir;
	}
	if (nowPos.x > destPos.x )
	{
		return _DIR_LEFT;
	}
	else
	{
		return _DIR_RIGHT;
	}
}
/**
 * 释放攻击动作
 */
void HBMonster::putAttack(int actionId)
{
	if (checkInCdTime(0.5)) return;
	monsterInfo.atkActionId = actionId;
#if (0)
	std::vector<HBMonster*> monsters;
	map->findEneries(this,this->monsterInfo._eyeshort,monsters);
	HBMonster *monster = NULL;
	if (monsters.size())
	{
		monster = monsters.at(0); // 取第一个
		_nowTargetId = monster->tempId;
		
		for ( std::vector<HBMonster*>::iterator iter = monsters.begin(); iter != monsters.end();++iter)
		{
			HBMonster * temp = *iter;
			if (temp && this->oldActionId != ACTION_BE_ATTACK && temp->oldActionId != ACTION_DEATH && temp->monsterInfo._hp > 0)
			{
				temp->_nowDestination = temp->calcBeAtkPosition(this);
				temp->tryNextAction(ACTION_BE_ATTACK);
			}
		}
	}
	else
	{
		_nowTargetId = -1;
		//return;
	}
	monsterInfo.atkActionId = actionId;
#endif
	this->tryNextAction(ACTION_ATTACK);
}
CCPoint HBMonster::calcBeAtkPosition(HBMonster * monster)
{
	if (!monster) return ccp(0,0);

	return this->getBackPointByDir((this->calcMonsterDir(monster->getPosition())));
}
CCPoint HBMonster::getBackPointByDir(int dir)
{
	switch(dir)
	{
		case _DIR_LEFT:return ccpAdd(this->getPosition(),ccp(monsterInfo._step,0));
		case _DIR_RIGHT:return ccpAdd(this->getPosition(),ccp(-monsterInfo._step,0));
	}
	return getPosition();
}
int HBMonster::getReverseDir(int nowDir)
{
	/*
	switch(nowDir)
	{
		case CalcDir::_DIR_DOWN: return CalcDir::_DIR_UP;
		case CalcDir::_DIR_LEFT:return CalcDir::_DIR_RIGHT;
		case CalcDir::_DIR_RIGHT:return CalcDir::_DIR_LEFT;
		case CalcDir::_DIR_UP:return CalcDir::_DIR_DOWN;
		case CalcDir::_DIR_DOWN_LEFT:return CalcDir::_DIR_UP_RIGHT;
		case CalcDir::_DIR_RIGHT_DOWN:return CalcDir::_DIR_DOWN_LEFT;
		case CalcDir::_DIR_UP_RIGHT:return CalcDir::_DIR_DOWN_LEFT;
		case CalcDir::_DIR_LEFT_UP:return CalcDir::_DIR_RIGHT_DOWN;
	}
	return CalcDir::_DIR_NULL;
	*/
	switch(nowDir)
	{
		case _DIR_LEFT:return _DIR_RIGHT;
		case _DIR_RIGHT:return _DIR_LEFT;
	}
	return oldDir;
}
void HBMonster::updateHp(int hp)
{
	if (!lineValue)
	{
		lineValue = UIValue::create("hp_back.png","hp_red.png",this->monsterInfo._maxHp);
		this->addChild(lineValue);
		lineValue->setPosition(256,320);
		lineValue->setScaleX(0.5);
	}
	if (lineValue && monsterInfo._maxHp != lineValue->getMaxValue())
	{
		this->removeChild(lineValue,true);
		lineValue = UIValue::create("hp_back.png","hp_red.png",this->monsterInfo._maxHp);
		this->addChild(lineValue);
		lineValue->setPosition(256,320);
		lineValue->setScaleX(0.5);
	}
	if (lineValue)
	{	
		lineValue->setValue(monsterInfo._hp);
	}
	if (hp != 0)
	{
		std::stringstream ss;
		ss << hp;
		CCLabelAtlas *labelAtlas = CCLabelAtlas::create(ss.str().c_str(),"fps_images.png", 12, 32, '.');
		labelAtlas->setScale(3.0);
		labelAtlas->setVisible(true);
		labelAtlas->setPosition(ccp(256,258));

		CCAction *spawn = CCSpawn::create(CCMoveBy::create(0.5f,ccp(0,64)),CCFadeOut::create(0.5),NULL);
		labelAtlas->runAction(spawn);
		this->addChild(labelAtlas,2);
		this->runAction(CCSequence::create(CCDelayTime::create(0.6),
			CCCallFuncND::create(this, callfuncND_selector(HBMonster::actionEnd_removeLabel), (void*)labelAtlas),NULL));
	}
	if (monsterInfo._hp <= 0)
	{
		return;
	}
}
bool HBMonster::isDeath()
{
	if (monsterInfo._hp <= 0 && isNowAction(ACTION_DEATH))
	{
		return true;
	}
	return false;
}
void HBMonster::actionEnd_removeLabel(CCNode *pSender,void *arg)
{
	if (pSender->getParent())
	{
		CCLabelAtlas *lbl = (CCLabelAtlas*) arg;
		pSender->removeChild(lbl,true);
	}
}

bool HBMonster::moveable(const zPos &tempPos,const zPos &dest,const int radius1)
{
	return true;
	if (map &&monsterInfo.monsterType == 1) // 是怪物的话
	{
		CCPoint point = map->getPointByGrid(dest);
		if (map->checkBlock(point.x,point.y))
			return true;
		else
			return false;
	}
	else
		return true;
}

/**
 * 是否在cd 时间内
 */
bool HBMonster::checkInCdTime(float cdtime)
{
	cc_timeval nowTime;
	CCTime::gettimeofdayCocos2d(&nowTime, NULL);
	float tapTime = CCTime::timersubCocos2d(&lastAtkTime,&nowTime) / 1000;
	if (tapTime >= cdtime)
	{
		lastAtkTime = nowTime;
		return false;
	}
	return true;
}

void HBMonster::visit()
{
	ccBlendFunc tmp_oBlendFunc = {GL_DST_COLOR, GL_SRC_COLOR};
	//this->setBlendFunc(tmp_oBlendFunc);
	CCSprite::visit();
}
NS_CC_END