#include "RpgPlayer.h"
#include "RpgMap.h"
#include "RpgUI.h"
#include "SoudManager.h"
#include "RpgGame.h"
NS_CC_BEGIN

int RpgSkillData::getAtkDistance()
{
	return CCRANDOM_0_1() * (this->atkMaxDistance - this->atkMinDistance) + this->atkMinDistance;
}
int RpgSkillData::getAtkValue()
{
	return CCRANDOM_0_1() * (this->atkMaxValue - this->atkMinValue) + this->atkMinValue;
}
int RpgSkillData::getAtkNum()
{
	return CCRANDOM_0_1() * (this->atkMaxNum - this->atkMinNum) + this->atkMinNum;
}
RpgPlayer * RpgPlayer::create(RpgMap *map)
{
	RpgPlayer * pRet = new RpgPlayer();
	if (pRet && pRet->init(map))
	{
        pRet->autorelease();
		theObjectM.addObject(pRet);
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool RpgPlayer::init(RpgMap *map)
{
	this->setPosition(ccp(200,200));
	map->addMonster(this);
	map->player = this;
   
	RpgMonster::init(5); // 初始化动作
	this->setDir(CalcDir::_DIR_RIGHT);
	return true;
}
/**
* 每一步结束后 处理是否访问到了npc 会自动更新主界面的展示
*/
void RpgPlayer::nextStep()
{
	RpgMap * map = (RpgMap*) this->getParent();
	
	if (isDeath())
	{
		return;
	}
	if (this->isNowAction(ACTION_DEATH))
	{
		deathTag = true;
        map->ui->stepEnd->show();
		return;
	}
	// 查看是否在移动中 
	if (this->isNowAction(ACTION_MOVE))
	{
		CCPoint touchPoint = this->getPosition();
		RpgBuild * build = map->pickBuild(touchPoint.x,touchPoint.y);
		if (build)
		{
			if (!inBuild)
			{
				doVisitBuild(build);
			}
			inBuild = true;
		}
		else
		{
			inBuild = false;
		}
		if (map)
		{
			map->updateMonster(this);
		}
		// 如果当前是移动状态 查看是否到达目标 若到达 尝试进行IDEL 行为
		if (isContinueMove(CalcDir::_DIR_NULL))
			this->tryConbineAction(IDLE_ACTION);
		else
			this->tryConbineAction(ACTION_MOVE);
	}
	else if (this->isNowAction(ACTION_BE_ATTACK))
	{
		// 如果当前是被击状态
		this->tryConbineAction(IDLE_ACTION);
	}
	else if (this->isNowAction(ACTION_ATTACK))
	{
		// 尝试计算攻击数值
		/*struct stFindTarget:stExecEachTarget{
			void exec(RpgObject *object)
			{
				if (objects.size() < 5 && object->isType(RpgObject::RPG_MONSTER))
				{
					objects.push_back(object);
					RpgMonster * monster = (RpgMonster*) object;
					if (monster)
					{
						monster->tryConbineAction(ACTION_BE_ATTACK);
						monster->beAttacked(100); // 以攻击100进行攻击
					}
				}
			}
			stFindTarget()
			{
			}
			std::vector<RpgObject*> objects;
		}findTarget;
		
		if (map)
		{
			map->execTargets(this->getGridPoint(),monsterData.eyeshort,&findTarget);
		}*/
		nowSkillName = "";
	}
	else if (this->isNowAction(ACTION_DEATH))
	{
		this->setVisible(false);
	}
	else this->tryConbineAction(IDLE_ACTION); // 默认执行空闲行为
//	tryMove();
}

void RpgPlayer::doVisitBuild(RpgBuild *build)
{
	RpgMap * map = (RpgMap*) this->getParent();
	if (map && map->ui)
	{
		if (build->info.id == 2)
		{
			CCDirector::sharedDirector()->pushScene( CCTransitionFade::create(1.5,RpgGame::scene()) );
		}
		else
			map->ui->visitUI->setVisible(true);
	}
}
struct stFindTargetAttack:stExecEachTarget{
		void exec(RpgObject *object)
		{
			if (objects.size() < data->getAtkNum() && object->isType(RpgObject::RPG_MONSTER) && object->isVisible())
			{
				objects.push_back(object);
				RpgMonster * monster = (RpgMonster*) object;
				if (monster)
				{
					monster->tryConbineAction(RpgMonster::ACTION_BE_ATTACK);
					int nowAtkValue = data->getAtkValue();
					monster->beAttacked(nowAtkValue); // 以攻击100进行攻击
                    CartoonInfo *info = MonsterManager::getMe().getCobineCartoon("flyattack", 0);
                    if (info)
                    player->runMoveState(monster->getMap(), *info, monster);
				}
			}
		}
		stFindTargetAttack(RpgSkillData *sdata,RpgPlayer *player)
		{
			data = sdata;
            this->player = player;
		}
		std::vector<RpgObject*> objects;
		RpgSkillData *data;
    RpgPlayer *player;
};
struct stFindMaxDistanceTarget:stExecEachTarget{
		void exec(RpgObject *object)
		{
			if (object->isType(RpgObject::RPG_MONSTER) && object->isVisible())
			{
				int dis = ((RpgMonster*)object)->calcDistance(player->getGridPoint());
				if (dis >= nowDis)
				{
					nowDis = dis;
					target = ((RpgMonster*)object);
				}
			}
		}
		stFindMaxDistanceTarget(RpgPlayer *player)
		{
			this->player = player;
			nowDis = 0;
			target = NULL;
		}
		int nowDis;
		RpgPlayer *player;
		RpgMonster * target;
};
void RpgPlayer::putAtkSkill(const std::string& skillname,const RpgSkillData &skillData)
{
	if (skillname=="speed")
	{
		// 给予10秒的加速
		monsterData.step = 128;
		speedTime = 0.4;
		this->addTime(MOVE_SPEED_UP,10);
	}
	else
	{
		RpgMap * map = (RpgMap*) this->getParent();
		stFindTargetAttack findTarget((RpgSkillData *)&skillData,this);
		
		if (map)
		{
			map->execTargets(this->getGridPoint(),((RpgSkillData*)&skillData)->getAtkDistance(),&findTarget);
		}
		RpgMonster::putAtkSkill(skillname);
	}
}
CartoonInfo* RpgPlayer::v_makeCartoon(int actionType)
{
	if (isDeath())
	{
		return NULL;
	}
	RpgMap * map = (RpgMap*) this->getParent();
	CartoonInfo *info = CartoonInfo::create();
	info->actionType = actionType;
	switch(actionType)
	{
		case ACTION_DEATH: // 死亡动画
		{
			 if (getCartoonInfo("death",0,*info))
				 return info;
		}break;
		case ACTION_BIRTH: // 出生动画
		{
			if (getCartoonInfo("birth",0,*info))
				return info;
		}break;
		case ACTION_MOVE: // 走路
		{
			theSound.play("playerwalk");
			CCPoint dest = getPixelPositionByDir(dir); // 根据方向获取下一步位置
			if (dest.x <monsterData.step || dest.x >= map->canBlockSize.width || dest.y <monsterData.step || dest.y >= map->canBlockSize.height)
			{
				switch(dir)
				{
				case CalcDir::_DIR_LEFT_UP:
					{
						dir = CalcDir::_DIR_LEFT;
					}break;
				case CalcDir::_DIR_DOWN_LEFT:
					{
						dir = CalcDir::_DIR_LEFT;
					}break;
				case CalcDir::_DIR_RIGHT_DOWN:
					{
						dir = CalcDir::_DIR_RIGHT;
					}	break;
				case CalcDir::_DIR_UP_RIGHT:
					{
						dir = CalcDir::_DIR_RIGHT;
					}break;
                case CalcDir::_DIR_DOWN:
                case CalcDir::_DIR_UP:
                    {
                        dir = faceDir;
                    }break;
				default:
					return NULL;
				}
			}
			dest = getPixelPositionByDir(dir); // 根据方向获取下一步位置
			if (dest.x <map->activeRect.origin.x || dest.x >= map->activeRect.size.width + map->activeRect.origin.x || dest.y <map->activeRect.origin.y || dest.y >= map->activeRect.size.height + activeRect.origin.y)
			{
				return NULL;
			}
			if (getCartoonInfo("move",0,*info))
			{
				info->needTime = speedTime;
				return info;
			}
		}break;
		case ACTION_ATTACK: // 攻击
		{
			//TODO 根据技能获取
			if (!useatkskills.empty())
			{
				std::string& useatkname = useatkskills.front();
				nowSkillName = useatkname;
				theSound.play(nowSkillName);
				//if (getCartoonInfo(useatkname,dir,*info))
				info = MonsterManager::getMe().getCobineCartoon(useatkname.c_str(),0);
				if (info)
				{
					info->actionType = ACTION_ATTACK;
					useatkskills.pop_front();
					return info;
				}
				return NULL;
			}
		}break;
		case ACTION_BE_ATTACK: // 被击
		{
			if (getCartoonInfo("beattack",0,*info))
				return info;
		}break;
		case IDLE_ACTION: // 空闲
		{
			
		}break;
	}
	if (getCartoonInfo("idle",0,*info)) // 默认执行空闲行为
		return info;
	return NULL; // error
}
void RpgPlayer::visit()
{
	if (this->checkTimeOut(MOVE_SPEED_UP))
	{
		speedTime = 0.6;
		monsterData.step = 96;
		this->setColor(ccc3(255,255,255));
	}
	else if (this->checkInTime(MOVE_SPEED_UP))
	{
		// 变换颜色
		this->setColor(ccc3(255,0,0));
	}
	RpgObject::visit();
	
}
/**
 * 执行动画
 */
void RpgPlayer::v_putCartoon(CartoonInfo *action)
{
	// 根据类型决定自己的展示
	if (!action) return;
	switch(action->actionType)
	{
		case ACTION_ATTACK:
		{
			// 根据技能获取
			//this->runSelfAction(*action,ccp(0,0)); // 原地动画
			stFindMaxDistanceTarget find(this);
			RpgMap * map = (RpgMap*) this->getParent();
			if (map)
			{
				map->execTargets(this->getGridPoint(),20,&find);
			}
			this->runAction(this->getParent(),action,ccp(0,0),find.target);
		}break;
		case  ACTION_MOVE:
		{
			CCPoint dest = getPixelPositionByDir(dir); // 根据方向获取下一步位置
			this->runSelfAction(*action,dest);
		}break;
		case ACTION_BIRTH:
		{
			this->runSelfAction(*action,ccp(0,0));
		}break;
		case IDLE_ACTION:
		{
			this->runSelfAction(*action,ccp(0,0));
		}break;
		case ACTION_BE_ATTACK:
		{
			this->runSelfAction(*action,ccp(0,0));
		}break;
		case ACTION_DEATH:
		{
			this->runSelfAction(*action,ccp(0,0));
		}break;
	}
}
/**
 * 持续移动
 */
void RpgPlayer::doContinueMove(int nowDir)
{
	continueMoveDir = nowDir;
	nowDestination = GridPoint(-1,-1);
}
/**
 * 取消持续移动
 */
void RpgPlayer::clearContinueMove()
{
	continueMoveDir = CalcDir::_DIR_NULL;
}
bool RpgPlayer::isContinueMove(int nowDir)
{
	return continueMoveDir == nowDir;
}
/**
 * 移动
 */
void RpgPlayer::setPosition(const CCPoint& point)
{
	RpgMap * map = getMap();
	if (map && miniMap)
	{
		CCPoint d = ccp(point.x / map->mapSize.width,point.y / map->mapSize.height);
		miniMap->updatePosition(d);
	}
	CCNode::setPosition(point);
}
void RpgPlayer::beAttacked(int atkValue)
{
	if (hp <= 0)
	{
		return;
	}
	int oldhp = hp;
	if (hp >= atkValue)
	{
		hp -= atkValue;
	}
	else
	{
		hp = 0;
	}
	
	atkValue = oldhp-hp;
	if (atkValue)
	{
		std::stringstream ss;
		ss << atkValue;
		//CCLabelAtlas *labelAtlas = CCLabelAtlas::create(ss.str().c_str(),"fps_images.png", 12, 32, '.');
		CCLabelAtlas *labelAtlas = CCLabelAtlas::create(ss.str().c_str(),"numbers_1.png", 41.28, 81, '.');
		labelAtlas->setVisible(true);
		//labelAtlas->setPosition(ccp(125,125));
		labelAtlas->setColor(ccc3(255,0,0));
		CCAction *spawn = CCSpawn::create(CCMoveBy::create(0.5f,ccp(0,64)),CCFadeOut::create(0.5),NULL);
		labelAtlas->runAction(spawn);
		this->addChild(labelAtlas,2);
		CCSprite::runAction(CCSequence::create(CCDelayTime::create(0.6),
			CCCallFuncND::create(this, callfuncND_selector(RpgMonster::actionEnd_removeLabel), (void*)labelAtlas),NULL));
		showHp();
		tryConbineAction(RpgMonster::ACTION_BE_ATTACK);
	}
	if (ui)
	{
		ui->updateHp(this);
	}
	if (hp <=0)
	{
		doDeath(); // RpgMonster 将死亡
	}
}
void RpgPlayer::doAttack(RpgMonster *monster)
{
	RpgSkillData data;
	data.atkMaxDistance = 7;
	data.atkMinDistance = 3;
	data.atkMaxNum = 3;
	data.atkMaxNum = 10;
	data.atkMaxValue = 200;
	data.atkMinValue = 20;
	monster->tryConbineAction(RpgMonster::ACTION_BE_ATTACK);
	int nowAtkValue = monster->monsterData.maxHp / 2;
	monster->beAttacked(nowAtkValue); // 以攻击100进行攻击
    CartoonInfo *info = MonsterManager::getMe().getCobineCartoon("flyattack", 0);
	int dir = CalcDir::calcDir(this->getPosition(),monster->getPosition());
	this->setDir(dir);
    if (info)
    {
        runMoveState(monster->getMap(), *info, monster);
        RpgMonster::putAtkSkill("attack");
    }
}
NS_CC_END