#include "RpgUI.h"
#include "RpgPlayer.h"
#include "CalcDir.h"
#include "RpgMap.h"
#include "RpgGuide.h"
#include "SimpleAudioEngine.h"
NS_CC_BEGIN
/////////////////////////移动操作盘////////////////////////////////////////
RpgMoveOPUI* RpgMoveOPUI::create(RpgPlayer *player)
{
	RpgMoveOPUI * pRet = new RpgMoveOPUI();
	if (pRet && pRet->init(player))
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool RpgMoveOPUI::init(RpgPlayer *player)
{
	this->player = player;
	CCSprite *opmove = CCSprite::create("moveop.png");
	if (opmove)
	{
		this->addChild(opmove);
		opMoveCenter = ccp(136,136);
		this->setPosition(opMoveCenter);
	}
	return true;
}
/**
 * 处理点击
 * \parma touchType 点击类型
 * \param touchPoint 点击点
 */
bool RpgMoveOPUI::doTouch(int touchType,CCTouch *touch)
{
	if (!player) return false;
	
	int opMoveTap = 32;
	int OP_SIZE = 256;
	CCPoint dest = touch->getLocation();
	switch(touchType)
	{
		case RpgUI::TOUCH_DOWN:
		{
			if (nowTouch) return false;
			if (ccpDistance(dest,opMoveCenter) > OP_SIZE / 2) // 只在圆盘内操作有效
			{ 
				player->clearContinueMove();
				return false;
			}
			CCPoint tsrc = ccp((int)(opMoveCenter.x / opMoveTap),(int)(opMoveCenter.y / opMoveTap)); 
			CCPoint tdest = ccp((int)(dest.x / opMoveTap),(int)(dest.y / opMoveTap)); 
			int nowDir = CalcDir::calcDir(tsrc,tdest);
			player->setDir(nowDir);
			player->doContinueMove(nowDir);
			player->tryConbineAction(RpgMonster::ACTION_MOVE);
			showTouch(nowDir);
			nowTouchIn = true;
			nowTouch = touch;
			theGuide.doNextStep(this->m_uID,this->m_nLuaID);
			return true;
		}break;
		case RpgUI::TOUCH_MOVE:
		{
			if (nowTouch != touch) return false;
			if (nowTouchIn)
			{
				CCPoint tsrc = ccp((int)(opMoveCenter.x / opMoveTap),(int)(opMoveCenter.y / opMoveTap)); 
				CCPoint tdest = ccp((int)(dest.x / opMoveTap),(int)(dest.y / opMoveTap)); 
				int nowDir = CalcDir::calcDir(tsrc,tdest);
				player->setDir(nowDir);
				player->doContinueMove(nowDir);
                player->tryConbineAction(RpgMonster::ACTION_MOVE);
				showTouch(nowDir);
				return true;
			}
		}
		break;
		case RpgUI::TOUCH_END:
		{	
			if (nowTouch != touch) return false;
			nowTouch = NULL;
			player->clearContinueMove();
			nowTouchIn = false;
			showTouch(-1);
		}break;
	}
	return false;
}
void RpgMoveOPUI::showTouch(int dirType)
{
	if (!dirSprite)
	{
		dirSprite = CCSprite::create("downjiantou.png");
		dirSprite->setScale(0.5);
		dirSprite->setVisible(false);
		this->addChild(dirSprite);
	}
	if (!tagSprite)
	{
		tagSprite = CCSprite::create("movetag.png");
		this->addChild(tagSprite);
	}
	CCPoint dest = ccp(0,0);
	int dirValue = 112;
	int dValue = 76;
	switch(dirType)
	{
		
		case CalcDir::_DIR_DOWN:
			{
				dirSprite->initWithFile("upjiantou.png");
				dest = ccp(0,dirValue);
			}break;
		case CalcDir::_DIR_LEFT:
			{
				dirSprite->initWithFile("leftjiantou.png");
				dest = ccp(-dirValue,0);
			}break;
		case CalcDir::_DIR_RIGHT:
			{
				dirSprite->initWithFile("rightjiantou.png");
				dest = ccp(dirValue,0);
			}break;
		case CalcDir::_DIR_UP:
			{
				dirSprite->initWithFile("downjiantou.png");
				dest = ccp(0,-dirValue);
			}break;
		case CalcDir::_DIR_DOWN_LEFT:
			{
				dirSprite->initWithFile("leftupjiantou.png");
				dest = ccp(-dValue,dValue);
			}break;
		case CalcDir::_DIR_RIGHT_DOWN:
			{
				dirSprite->initWithFile("uprightjiantou.png");
				dest = ccp(dValue,dValue);
			}break;
		case CalcDir::_DIR_UP_RIGHT:
			{
				dirSprite->initWithFile("rightdownjiantou.png");
				dest = ccp(dValue,-dValue);
			}break;
		case CalcDir::_DIR_LEFT_UP:
			{
				dirSprite->initWithFile("downleftjiantou.png");
				dest = ccp(-dValue,-dValue);
			}break;
	}
	tagSprite->setPosition(dest);
}
///////////////////////////////////////////////////////////////////////////////////////////
RpgQuickItem* RpgQuickItem::create(int quickId)
{
	RpgQuickItem * pRet = new RpgQuickItem();
	if (pRet)
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
RpgQuickItem * RpgQuickItem::create(const char *upName,const char * downName,const CCSize & rect)
{
	RpgQuickItem *pRet = new RpgQuickItem();
    if (pRet && pRet->init(upName, downName,rect))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
/**
 * 展示CD 特效
 */
void RpgQuickItem::startShowCD()
{
	invalid = true;
	this->setDisable(true);
	this->setEventOff();
	CartoonInfo info = *MonsterManager::getMe().getCobineCartoon("cdshow", 0);
	info.needTime = cdtime;
	CCAnimate* animation = CCAnimate::create(info.createAnimation());
	if (animation)
	{
		CCSprite *tempSprite = CCSprite::create();
		this->getParent()->addChild(tempSprite);
		tempSprite->setPosition(this->getPosition());
		CCAction *action = CCSequence::create(animation,
			CCCallFuncND::create(this, callfuncND_selector(RpgQuickItem::cdTimeEnd), (void*)tempSprite),NULL);	
		tempSprite->runAction(action);
	}
}
void RpgQuickItem::cdTimeEnd(CCNode *pSender,void *arg)
{
	this->setDisable(false);
	invalid = false;
	this->setEventOn();
	CCSprite * tempSprite = (CCSprite*) arg;
	tempSprite->removeFromParentAndCleanup(true);
}
////////////////////////////////快速攻击栏////////////////////////////////////////////////

// 初始就一个基本攻击 后面可以点击道具到快捷栏 可以是药品 或者 技能 条目之间可以相互移动
RpgQuickOPUI * RpgQuickOPUI::create(RpgPlayer *player)
{
	RpgQuickOPUI * pRet = new RpgQuickOPUI();
	if (pRet && pRet->init(player))
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
struct stDoPutPlayerSkill:public UIEventCallback<stDoPutPlayerSkill>{
	stDoPutPlayerSkill()
	{
		_player = NULL;
		dirType = CalcDir::_DIR_LEFT;
	}
	void doClick(UICenterItem *item)
	{
		RpgQuickItem *qitem = (RpgQuickItem*) item;
		// 尝试攻击
		if (_player && qitem && !qitem->invalid)
		{
			RpgSkillData data;
			data.atkMaxDistance = 7;
			data.atkMinDistance = 3;
			data.atkMaxNum = 3;
			data.atkMaxNum = 10;
			data.atkMaxValue = 200;
			data.atkMinValue = 20;
			_player->putAtkSkill(atkName.c_str(),data);
			qitem->startShowCD();
		}
		return;
	}
	int dirType;
	std::string atkName;
	RpgPlayer *_player;
};
bool RpgQuickOPUI::init(RpgPlayer *player)
{
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData("quickop.xml","rb",&nSize);
	if (nSize)
	{
		script::tixmlCode::initFromString((char*)buffer);
	}
	this->player = player;
	// 初始化逻辑 从配置文件中获取相应的数据
	//this->setExchange();
	std::string quickInfo = CCUserDefault::sharedUserDefault()->getStringForKey("quickopinfo");
	int qucikInfoLen = CCUserDefault::sharedUserDefault()->getIntegerForKey("quickopinfolen");
	UICenterBag::init("putskill_up.png","putskill_down.png",CCSizeMake(125,125));
	setMovable(false);
	{
		do{
			static const char * skills[] = {"baoattack","attack","speed"};
			static const char * skillpngs[3][2] = {
				{"putskill_baoji.png","putskill_baoji.png"},
				{"normal_atk_show.png","normal_atk_show.png"},
				{"speed_up_skill.png","speed_up_skill.png"}
			};
			static const int cdtimes[] = {2,4,10};
			
			for (int i = 0; i < 3;i++)
			{
				RpgQuickItem *item = RpgQuickItem::create(skillpngs[i][0],skillpngs[i][1],CCSizeMake(64,64));
				//item->setText(moves[i]);
				stDoPutPlayerSkill *doSkill = stDoPutPlayerSkill::create();
				doSkill->_player = player;
				doSkill->atkName = skills[i];
				item->cdtime = cdtimes[i];
				item->bind(doSkill);
				this->addItem(UICenterBag::LEFT_SET,item);
			}
		}while(false);
	}
	show();
	this->setMovable(false);
	return true;
}
void RpgQuickOPUI::onClick()
{
	// 处理基本攻击
	if (player)
	{
		RpgSkillData data;
		data.atkMaxDistance = 15;
		data.atkMinDistance = 10;
		data.atkMaxNum = 10;
		data.atkMaxNum = 15;
		data.atkMaxValue = 5;
		data.atkMinValue = 2;
		player->putAtkSkill("normal_atk",data);
		theGuide.doNextStep(this->m_uID,this->m_nLuaID);
		startProcess();
	}
}
/**
 * 展示条目
 * \param item 待展示的item 没有延时直接展示
 */
void RpgQuickOPUI::showItemWithXml(int dirType,UICenterItem *item)
{
	if (item->itemId < pointList.size())
	{
		item->setPosition(pointList[item->itemId]);
	}
}
/**
 * 从配置文件中加载配置
 * \param node 配置根节点
 */
void RpgQuickOPUI::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
		script::tixmlCodeNode posNode = node->getFirstChildNode("position");
		while(posNode.isValid())
		{
			int id = 0;
			CCPoint pos;
			posNode.getAttr("id",id);
			posNode.getAttr("x",pos.x);
			posNode.getAttr("y",pos.y);
			if (id >= pointList.size())
			{
				pointList.resize(id + 1);
			}
			pointList[id] = pos;
			posNode = posNode.getNextNode("position");
		}
	}
}
/**
* 展示条目
* \param item 待展示的item 没有延时直接展示
*/
void RpgQuickOPUI::showItem(int dirType,UICenterItem *item)
{
	showItemWithXml(dirType,item);
	return;
	const float PI = 3.1415926;
	CCPoint dir[4] = {
		CCPoint(PI/2,PI/2), // 右边设置
		CCPoint(0,0),// 左边设置
		CCPoint(PI,PI), // 向下设置
		CCPoint(3*PI/2,3*PI/2),// 向上设置
	};
	int loops[] = {3,1,1,1};
	int loopCount = 0;
	int tap = loops[0];
	int count = item->itemId;
	while (count >= 0 )
	{
		if (count - tap < 0)
		{
			break;
		}
		else
		{
			count = count - tap;
			loopCount++;
			if (loopCount >= sizeof(loops) / sizeof(int)) return; // 多余的不展示
			tap += loops[loopCount];
		}
	}
	int radius[]={130,200,265,330};
	int r = radius[loopCount]; //(eachSpan + eachWidth+16)* (loopCount + 1);
	int index = count;
	CCPoint dest = 
		ccp(
		cos(dir[dirType].x + (PI/2) * index / (tap - 1)) * (r) + 32,
		sin(dir[dirType].y + (PI/2) * index / (tap - 1)) * (r) + 32
		);
	dest = ccpAdd(dest,ccp(32,0));
	item->setPosition(dest);
	return;
}
////////////////////// 角色血量值 ////////////////////////

RpgHpValueUI* RpgHpValueUI::create(float maxValue,float defaultValue,const char *upName,const  char *backName)
{
	RpgHpValueUI * pRet = new RpgHpValueUI();
	if (pRet && pRet->init(maxValue,defaultValue,upName,backName))
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool RpgHpValueUI::init(float maxValue,float defaultValue,const char *upName,const  char *backName)
{
	this->maxValue = maxValue;
	this->value = defaultValue;
	this->backSprite = CCSprite::create(backName);
	this->backSprite->setAnchorPoint(ccp(0,0.5));
	this->addChild(this->backSprite);
	this->valueSprite = CCSprite::create(upName);
	this->valueSprite->setAnchorPoint(ccp(0,0.5));
	this->addChild(this->valueSprite);
	width = valueSprite->getContentSize().width;
	setValue(value);
	return true;
}

void RpgHpValueUI::setValue(float value)
{
	this->value = value;
	valueSprite->setTextureRect(CCRectMake(0,0,
	 width * value / maxValue,valueSprite->getContentSize().height));
}

///////////////////////////角色头像////////////////////////////////////////////////////
RpgPlayerSimpleInfoUI* RpgPlayerSimpleInfoUI::create(RpgPlayer *player)
{
	RpgPlayerSimpleInfoUI * pRet = new RpgPlayerSimpleInfoUI();
	if (pRet && pRet->init(player))
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool RpgPlayerSimpleInfoUI::init(RpgPlayer *player)
{
	this->player = player;
	
	hpValue = RpgHpValueUI::create(500,500,"hp_line_up.png","hp_line_back.png");
	this->addChild(hpValue);
	hpValue->setPosition(ccp(124,0));
	CCPoint offset = ccp(124,0);
	for (int i = 0; i < 5;i++)
	{
		RpgHpValueUI* hpValue = RpgHpValueUI::create(100,100);
		this->addChild(hpValue);
		hpValue->setPosition(ccp(48*i + offset.x,offset.y + 64 - 22 ));
		hpValues.push_back(hpValue);
	}
	tag = hpValues.size();
	CCSprite *sprite = CCSprite::create("playerinfo.png");
	this->addChild(sprite);
	sprite->setPosition(ccp(64,0));
	return true;
}
void RpgPlayerSimpleInfoUI::update()
{
	if (!player) return;
	do{
		int hp = player->hp;
		if (hp <=0 && tag <= hpValues.size() && tag >= 1)
		{
			tag--;
			hpValues[tag]->setValue(0);
			player->hp = 500;
		}
		hpValue->setValue(player->hp);
	}while(false);
	return;
	int size = hpValues.size();
	int hp = player->hp;
	// 每格 100点血
	int tag = hp / 100;
	int vtag = hp % 100;
	if (tag < hpValues.size())
		hpValues[tag]->setValue(vtag);
	hpValue->setValue(hp);
}
void RpgPlayerSimpleInfoUI::resume()
{
	if (!player) return;
	do{
		int hp = player->hp;
		tag = hpValues.size();
		for (int i = 0; i  < hpValues.size() ; i++)
		{
			hpValues[i]->setValue(500);
		}
		hpValue->setValue(player->hp);
	}while(false);
	return;
}
//////////////////////////小地图//////////////////////////////////////////////////////
MiniMap* MiniMap::create()
{
	MiniMap * pRet = new MiniMap();
	if (pRet && pRet->init())
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool MiniMap::init()
{
	CCSprite::initWithFile("minimap.png");
	CCTime::gettimeofdayCocos2d(&lastTime,NULL);
	return true;
}
bool MiniMap::checkTimeOut()
{
	cc_timeval nowTime;
	CCTime::gettimeofdayCocos2d(&nowTime,NULL);
	if (CCTime::timersubCocos2d(&lastTime,&nowTime) >= 1000)
	{
		lastTime = nowTime;
		return true;
	}
	return false;
}
void MiniMap::visit()
{
	CCSprite::visit();	
}
RpgMinMapUI* RpgMinMapUI::create(RpgMap *map)
{
	RpgMinMapUI * pRet = new RpgMinMapUI();
	if (pRet && pRet->init(map))
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool RpgMinMapUI::init(RpgMap *map)
{
	// 创建一个底图 然后放置在右上角
	miniMap = MiniMap::create();
	mask = CCSprite::create("minimapback.png");
	miniMap->retain();
	mask->retain();
	target = CCSprite::create();
	this->addChild(target);
	miniPlayer = CCSprite::create("test.png");
	miniPlayer->setScale(0.2f);
	this->addChild(miniPlayer);
	// 会尝试在其上现实怪物 和 主角的位置
	CCSize srcContent = miniMap->getContentSize();  
	miniMap->setPosition(ccp(srcContent.width - (400 / map->mapSize.width) * srcContent.width, 400 / map->mapSize.height * srcContent.height));
	return true;
}
void RpgMinMapUI::setSize(const CCSize &size)
{
	
}
/**
 * 更新点
 */
void RpgMinMapUI::updatePosition(const CCPoint &point)
{
	CCPoint nowPoint = ccp(minMapSize.width * point.x,minMapSize.height * point.y);
	static CCPoint oldPoint = nowPoint;
	
	CCPoint dest = miniMap->getPosition();
	miniMap->setPosition(ccp(dest.x -( nowPoint.x - oldPoint.x),
				  dest.y - (nowPoint.y - oldPoint.y)));
	oldPoint = nowPoint;
	
}
void  RpgMinMapUI::visit()
{     
	CCSprite *src = miniMap;
    assert(src);  
    assert(mask);  
   
    CCSize srcContent = src->getContentSize();  
    CCSize maskContent = mask->getContentSize();  
   
    CCRenderTexture * rt = CCRenderTexture::create(srcContent.width, srcContent.height, kTexture2DPixelFormat_RGBA8888);  
    minMapSize = srcContent; 
    float ratiow = srcContent.width / maskContent.width;  
    float ratioh = srcContent.height / maskContent.height;  
  //  mask->setScaleX(ratiow);  
   // mask->setScaleY(ratioh);  
   
    mask->setPosition(ccp(srcContent.width / 2, srcContent.height / 2));  

    ccBlendFunc blendFunc2 = { GL_ONE, GL_ZERO };  
    mask->setBlendFunc(blendFunc2);  
    ccBlendFunc blendFunc3 = { GL_DST_ALPHA, GL_ZERO };  
    src->setBlendFunc(blendFunc3);  
   
    rt->begin();  
    mask->visit();  
    src->visit();  
    rt->end();  
   
    target->initWithTexture(rt->getSprite()->getTexture());  
    target->setFlipY(true);  
	CCNode::visit();
}


//////////////////////////系统操作盘/////////////////////////////////////////////////////
RpgSystemOPUI* RpgSystemOPUI::create(RpgUI *uiSys)
{
	RpgSystemOPUI *pRet = new RpgSystemOPUI();
    if (pRet && pRet->init(uiSys))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
struct stDoClose:public UIEventCallback<stDoClose>{
	void doClick(UICenterItem *item)
	{
	//	CocosDenshion::SimpleAudioEngine::sharedEngine()->end();
		CCDirector::sharedDirector()->end();
        exit(0);
	}
};
struct stDealMusic:public UIEventCallback<stDealMusic>{
	void doClick(UICenterItem *item)
	{
		tag = !tag;
		if (!tag)
			CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
		else
			CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
	}
	bool tag;
	stDealMusic()
	{
		tag = true;
	}
};
struct stShowQuickOp:public UIEventCallback<stShowQuickOp>
{
	void doClick(UICenterItem *item)
	{
		uiSys->showUI(showType);
	}
	stShowQuickOp()
	{
		this->uiSys = NULL;
		showType = 0;
	}
	RpgUI *uiSys;
	int showType;
};
bool RpgSystemOPUI::init(RpgUI *uiSys)
{
	this->choiceAble = true;
	UICenterBag::init("sys_btn_up.png","sys_btn_down.png",CCSizeMake(64,64));
	{
		do{
			
			UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCSizeMake(64,64));
			item->bind(stDoClose::create());
			item->setText("exit");
			this->addItem(UICenterBag::UP_SET,item);
		}while(false);
		do{
			
			UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCSizeMake(64,64));
			item->bind(stDealMusic::create());
			item->setText("music");
			this->addItem(UICenterBag::UP_SET,item);
		}while(false);
		/*do{
			struct stShowBag:public UIEventCallback<stShowBag>{
				void doClick()
				{
					if (uiSys)
					{
						if (uiSys->playerBag->isVisible())
							uiSys->playerBag->setVisible(false);
						else
							uiSys->playerBag->setVisible(true);
					}
				}
				stShowBag()
				{
					this->uiSys = NULL;
				}
				RpgUI *uiSys;
			};
			UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCRectZero);
			stShowBag *showLogic = stShowBag::create();
			showLogic->uiSys = uiSys;
			item->bind(showLogic);
			item->setText("bag");
			this->addItem(UICenterBag::UP_SET,item);
		}while(false);
		do{
			struct stShowPlayer:public UIEventCallback<stShowPlayer>{
				void doClick()
				{
					if (uiSys)
					{
						if (uiSys->playerInfo->isVisible())
							uiSys->playerInfo->setVisible(false);
						else
							uiSys->playerInfo->setVisible(true);
					}
				}
				stShowPlayer()
				{
					this->uiSys = NULL;
				}
				RpgUI *uiSys;
			};
			UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCRectZero);
			stShowPlayer *showLogic = stShowPlayer::create();
			showLogic->uiSys = uiSys;
			item->bind(showLogic);
			item->setText("player");
			this->addItem(UICenterBag::LEFT_SET,item);
		}while(false);
		*/
		static char * names[] = {"shop","attack","player","bag"};
		static int types[]={RpgUI::UI_TYPE_SHOP,RpgUI::UI_TYPE_QUICK,RpgUI::UI_TYPE_PLAYER_INFO,RpgUI::UI_TYPE_BAG};
		
		for (int i = 0; i < 4;i ++)
		{
			UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCSizeMake(64,64));
			item->setText(names[i]);
			stShowQuickOp *showOp = stShowQuickOp::create();
			showOp->uiSys = uiSys;
			showOp->showType = types[i];
			item->bind(showOp);
			this->addItem(UICenterBag::LEFT_SET,item);
		}
	}
	return true;
}
void RpgSystemOPUI::showItem(int dirType,UICenterItem *item)
{
	CCPoint dir[4] = {
		CCPoint(-1,0),// 左边设置
		CCPoint(1,0), // 右边设置
		CCPoint(0,-1), // 向下设置
		CCPoint(0,1),// 向上设置
	};

	CCPoint dest = //ccpAdd(this->getPosition(),
		ccp(dir[dirType].x * (item->itemId + 1)* (eachSpan + eachWidth) + 32,dir[dirType].y * (item->itemId + 1)* (eachSpan + eachHeight) + 32);
		//);
	item->setPosition(ccp(32,32));
	int index = _items[dirType].size() - item->itemId;
	float tap = 0.2f;
	CCFiniteTimeAction *action1 = CCSequence::create(CCDelayTime::create(tap*index),CCMoveTo::create(tap,dest),NULL);
	//item->setPosition(dest);
	CCFiniteTimeAction * action2 = CCSequence::create(CCHide::create(),CCDelayTime::create(tap*index),CCShow::create(),CCFadeIn::create(0.5),NULL);
	CCFiniteTimeAction *spawn = CCSpawn::create(action1,action2,NULL);
	item->runAction(spawn);
}
///////////////////////////////////////横轴方向滚动////////////////////////////////////
/**
 * 横轴方向滚动展示信息
 */

RpgAdSysUI* RpgAdSysUI::create()
{
	RpgAdSysUI *pRet = new RpgAdSysUI();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool RpgAdSysUI::init()
{
	return true;
}
/**
* 定时器
*/
void RpgAdSysUI::tick()
{
	// 做次剔除
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	for (std::list<CCLabelTTF*>::iterator iter = infos.begin(); iter != infos.end();)
	{
		CCLabelTTF *info = *iter;
		if (info)
		{
			int maxX = info->getPositionX() + info->getContentSize().width / 2;
			if (maxX < 0)
			{
				temps.push_back(info);
				//info->removeFromParentAndCleanup(true);
				info->setVisible(true);
				iter = infos.erase(iter);
				continue;
			}
		}
		++iter;
	}

	if (infos.empty())
	{
		for (std::vector<CCLabelTTF*>::iterator iter = temps.begin(); iter != temps.end(); ++iter)
		{
		//	(*iter)->setPositionX(visibleSize.width+200);
		//	(*iter)->setVisible(true);
		//	infos.push_back(*iter);
		}
		temps.clear();
	}
	// 获取首个节点的位置
	if (infos.empty()) return;
	CCLabelTTF *info = *infos.begin();
	float offset = 0;
	if (info)
	{
		offset =  info->getPositionX()- info->getContentSize().width / 2 - 32;
	}
	for (std::list<CCLabelTTF*>::iterator iter = infos.begin(); iter != infos.end();++iter)
	{
		CCLabelTTF *info = *iter;
		if (info)
		{
			info->setPositionX(offset + info->getContentSize().width / 2);
			offset += info->getContentSize().width + 32;
		}
	}
	// 展示位置
}

void RpgAdSysUI::addContent(const std::string& content)
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	
	CCLabelTTF *info = CCLabelTTF::create(content.c_str(),"Arail",32);
	if (info)
	{
		info->setPositionX(visibleSize.width+200);
		this->addChild(info);
		infos.push_back(info);
		info->setColor(ccc3(30,30,255));
	}
}
/////////////////////////////////角色装备信息展示/////////////////////////////////////
/**
 * 角色装备信息 和 属性值
 */

RpgPlayerInfoUI* RpgPlayerInfoUI::create()
{
	RpgPlayerInfoUI *pRet = new RpgPlayerInfoUI();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool RpgPlayerInfoUI::init()
{
	// 初始化位置 可放置道具格子 可更新值位置
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData("playerinfo.xml","rb",&nSize);
	if (!nSize)return false;
	if (script::tixmlCode::initFromString((char*) buffer))
	{
		_eachWidth = 64;
		_eachHeight = 64;
		return true;
	}
	return false;
}
/**
 * 获取当前可视的宽
 */
float RpgPlayerInfoUI::getViewWidth()
{
	return 195;
}
/**
 * 获取当前可视的高
 **/
float RpgPlayerInfoUI::getViewHeight()
{
	return 192;
}
/**
 * 更新数值
 */
void RpgPlayerInfoUI::updateValue(const std::string& content,float value)
{
	VALUES_POS_ITER pos_iter = valuePositions.find(content);
	if (pos_iter != valuePositions.end())
	{
		VALUES_ITER iter = values.find(content);
		std::stringstream ss;
		ss << value;
		if (iter != values.end())
		{
			CCLabelTTF* valueLbl = iter->second;
			if (valueLbl)
			{
				valueLbl->setString(ss.str().c_str());
			}
		}

		CCLabelTTF* valueLbl = CCLabelTTF::create(ss.str().c_str(),"Arial",16);
		if (valueLbl)
		{
			values[content] = valueLbl;
			this->addChild(valueLbl); // 信息的位置
			valueLbl->setPosition(pos_iter->second);
		}
	}
}
/**
 * 或者值的位置
 */
CCPoint RpgPlayerInfoUI::getValuePosition(const std::string& content)
{
	VALUES_POS_ITER pos_iter = valuePositions.find(content);
	if (pos_iter != valuePositions.end())
	{
		return pos_iter->second;
	}
	return ccp(0,0);
}
/**
* 获取当前id 的像素位置
*/
CCPoint RpgPlayerInfoUI::getPixelPosition(int id)
{
	if (id < - 1 && id >= positions.size()) return ccp(-1,-1);
	return positions.at(id);
}

int RpgPlayerInfoUI::getNowTouchBagIdByCursorPosition(const CCPoint& point)
{
	CCPoint pos = this->convertToNodeSpace(point);
	// 检查是否在6个区域内
	if (checkIn(pos.x,pos.y))
	{	
		for ( int i = 0; i < positions.size();i++)
		{
			CCRect rect = CCRectMake(positions[i].x,positions[i].y,_eachWidth,_eachHeight);
			if (rect.containsPoint(pos))
			{
				return i;
			}
		}
	}
	return -1;
}
/**
* 检查坐标是否在区域内
*/
bool RpgPlayerInfoUI::checkIn(int x,int y)
{
	CCPoint pos = ccp(x,y);
	for ( int i = 0; i < positions.size();i++)
	{
		CCRect rect = CCRectMake(positions[i].x,positions[i].y,_eachWidth,_eachHeight);
		if (rect.containsPoint(pos))
		{
			return true;
		}
	}
	return false;
}
/**
* 从配置文件中加载配置
* \param node 配置根节点
*/
void RpgPlayerInfoUI::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
		/**
		 * <Config>
		 *		<equips>
		 *			<position id="" x="" y=""/>
		 *			<position id="" x="" y=""/>
		 *		</equips>
		 *		<values>
		 *			<value name="" x="" y=""/>
		 *			<value name="" x="" y=""/>
		 *			<value name="" x="" y=""/>
		 *		</values>
		 * </Config>
		 **/
		script::tixmlCodeNode equipsNode = node->getFirstChildNode("equips");
		if(equipsNode.isValid())
		{
			script::tixmlCodeNode positionNode = equipsNode.getFirstChildNode("position");
			while (positionNode.isValid())
			{
				int id = 0;
				float x = 0;
				float y = 0;
				positionNode.getAttr("id",id);
				positionNode.getAttr("x",x);
				positionNode.getAttr("y",y);
				if (id >= positions.size()) positions.resize(id + 1);
				positions[id] = ccp(x,y);
				positionNode = positionNode.getNextNode("position");
			}
		}
		script::tixmlCodeNode valuesNode = node->getFirstChildNode("values");
		if (valuesNode.isValid())
		{
			script::tixmlCodeNode valueNode = valuesNode.getFirstChildNode("value");
			while(valueNode.isValid())
			{
				std::string name = valueNode.getAttr("name");
				float x = 0;
				float y = 0;
				valueNode.getAttr("x",x);
				valueNode.getAttr("y",y);
				valuePositions[name] = ccp(x,y);
				valueNode = valueNode.getNextNode("value");
			}
		}
	}
}
/////////////////////地图编辑器//////////////////////////////

RpgMapOPUI* RpgMapOPUI::create(RpgUI *uiSys)
{
	RpgMapOPUI *pRet = new RpgMapOPUI();
    if (pRet && pRet->init(uiSys))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
struct stDoPutSkill:public UIEventCallback<stDoPutSkill>{
	stDoPutSkill()
	{
		uiSys = NULL;
		item = NULL;
		opId = -1;
	}
	void doClick(UICenterItem *item)
	{
		uiSys->map->nowSetBlockType = opId;
		uiSys->map->doTouchType = RpgMap::DO_TOUCH_SET;
	}
	RpgUI * uiSys;
	UICenterItem *item;
	int opId;
};
struct stDoMap:public UIEventCallback<stDoMap>{
	stDoMap()
	{
		uiSys = NULL;
		opId = -1;
	}
	void doClick(UICenterItem *item)
	{
		if (opId == 10)
		{
			uiSys->map->saveBlock("block.b64");
		}
		else
			uiSys->map->doTouchType = opId;
	}
	RpgUI * uiSys;
	int opId;
};
bool RpgMapOPUI::init(RpgUI *uiSys)
{
	UICenterBag::init("test.png","bule_big.png",CCSizeMake(64,64));
	
	
		
	static const char* names[] ={"TB","TC","BB","BC","PB","MB"};
	for (int i = 0; i < 6;i++)
	{
		UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCSizeMake(64,64));
		
		stDoPutSkill *doPut = stDoPutSkill::create();
		doPut->uiSys = uiSys;
		doPut->item = item;
		item->setText(names[i]);
		doPut->opId = 1 << i;
		item->bind(doPut);
		this->addItem(UICenterBag::LEFT_SET,item);
	}
	
	static const char* movetypenames[] = {"MOVE","SAVE"};
	static int movetypes[] = {RpgMap::DO_TOUCH_MOVE,10};
	for( int i = 0; i < 2;i++)
	{
		UICenterItem *item = UICenterItem::create("bule_big.png","test.png",CCSizeMake(64,64));
		
		stDoMap *doMap = stDoMap::create();
		doMap->uiSys = uiSys;
		item->setText(movetypenames[i]);
		doMap->opId = movetypes[i];
		item->bind(doMap);
		this->addItem(UICenterBag::LEFT_SET,item);
	}
	
	show();
	this->setMovable(false);
	return true;
}

void RpgMapOPUI::showItem(int dirType,UICenterItem *item)\
{
	const float PI = 3.1415926;
	CCPoint dir[4] = {
		CCPoint(PI/2,PI/2), // 右边设置
		CCPoint(0,0),// 左边设置
		CCPoint(PI,PI), // 向下设置
		CCPoint(3*PI/2,3*PI/2),// 向上设置
	};
	int tap = 3;
	int count = item->itemId;
	while (count >= 0 )
	{
		if (count - tap < 0)
		{
			break;
		}
		else
		{
			count = count - tap;
			tap ++;
		}
	}
	int r = (eachSpan + 64+16)* ((tap - 3) + 1);
	int index = count;
	CCPoint dest = 
		ccp(
		cos(dir[dirType].x + (PI/2) * index / (tap - 1)) * (r) + 32,
		sin(dir[dirType].y + (PI/2) * index / (tap - 1)) * (r) + 32
		);
	item->setPosition(dest);
	return;
}
//////////////////////////轮次结束信息预告///////////////////////////////////////

RpgText *RpgText::create(const char *str,float width,float fontSize,const ccColor3B& color)
{
	RpgText *pRet = new RpgText();
    if (pRet && pRet->init(str,width,fontSize,color))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool RpgText::init(const char *str,float width,float fontSize,const ccColor3B& color)
{
	infoLabel = UIMultiLineArea::create(str,width,fontSize,color);
	this->addChild(infoLabel);
	return true;
}
void RpgText::update(const std::string &content)
{
	if (infoLabel)
	{
		infoLabel->update(content);
	}
}
RpgText * RpgText::create(script::tixmlCodeNode *node)
{
	if (node)
	{
		int x = node->getInt("x");
		int y = node->getInt("y");
		int r = node->getInt("r");
		int g = node->getInt("g");
		int b = node->getInt("b");
		RpgText *rpgText = RpgText::create(node->getAttr("content"),node->getFloat("width"),node->getInt("fontsize"),ccc3(r,g,b));
		rpgText->setPosition(ccp(x,y));
		return rpgText;
	}
	return NULL;
}


/**
 * 初始化
 */
bool RpgInfoUI::init(const char *fileName)
{
	unsigned long nSize = 0;
    std::string file= CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(fileName);
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData(file.c_str(),"rb",&nSize);
	if (nSize)
	{
		return script::tixmlCode::initFromString((char*)buffer);
	}
	return false;
}
struct stDoClick:public UIEventCallback<stDoClick>{
	stDoClick()
	{
		endUI = NULL;
	}
	void doClick(UICenterItem *item)
	{
		if (endUI)
			endUI->setVisible(false);
	}
	RpgInfoUI *endUI;
};
const CCSize & RpgInfoUI::getContentSize()
{
	return backSize;
}
/**
 * 处理节点
 */
void RpgInfoUI::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
		this->setAnchorPoint(ccp(0,0));
		script::tixmlCodeNode backNode = node->getFirstChildNode("back");
		if (backNode.isValid())
		{
			std::string backName = backNode.getAttr("src");
			int backw= backNode.getInt("w");
			int backh = backNode.getInt("h");
			backSize = CCSizeMake(backw,backh);
			if (backName != "")
			{
				CCSprite *back = CCSprite::create(backName.c_str());
				if (back)
				{
					back->setScaleX(backw / back->getContentSize().width);
					back->setScaleY(backh / back->getContentSize().height);
					this->addChild(back,-1);
					back->setAnchorPoint(ccp(0,0));
				}
			}
		}
		script::tixmlCodeNode textNode = node->getFirstChildNode("text");
		while(textNode.isValid())
		{
			RpgText * text = RpgText::create(&textNode);
			if (text)
			{
				std::string name = textNode.getAttr("name");
				this->updateContents[name] = text;
				this->addChild(text);
				text->setAnchorPoint(ccp(0,0));
			}
			textNode = textNode.getNextNode("text");
		}
		script::tixmlCodeNode imgNode = node->getFirstChildNode("image");
		while (imgNode.isValid())
		{
			std::string imgName = imgNode.getAttr("name");
			int x = imgNode.getInt("x");
			int y = imgNode.getInt("y");
			int w = imgNode.getInt("w");
			int h = imgNode.getInt("h");
			if (imgName != "")
			{
				CCSprite *sprite = CCSprite::create(imgName.c_str());
				if (sprite)
				{
					sprite->setPosition(ccp(x,y));
					sprite->setScaleX(w / sprite->getContentSize().width);
					sprite->setScaleY(h / sprite->getContentSize().height);
					this->addChild(sprite);
					sprite->setAnchorPoint(ccp(0,0));
				}
			}
			imgNode = imgNode.getNextNode("image");
		}
        script::tixmlCodeNode btnNode = node->getFirstChildNode("button");
        while (btnNode.isValid())
        {
            UICenterItem *btn = UICenterItem::create(btnNode.getAttr("uppng"), btnNode.getAttr("downpng"),
                                                     CCSizeMake(btnNode.getFloat("width"), btnNode.getFloat("height")));
            if (btn)
            {
            this->addChild(btn);
            btn->setPosition(ccp(btnNode.getFloat("x"),btnNode.getFloat("y")));
            buttons[btnNode.getAttr("name")] = btn;
            }
            btnNode = btnNode.getNextNode("button");
        }
		script::tixmlCodeNode bagNode = node->getFirstChildNode("bag");
		while(bagNode.isValid())
		{
			RpgXmlBag *xmlBag = RpgXmlBag::create(&bagNode);
			if (xmlBag->view)
				xmlBag->view->setPosition(bagNode.getInt("x"),bagNode.getInt("y"));
			xmlBag->addToParent(this);
			bags.push_back(xmlBag);
			bagNode  = bagNode.getNextNode("bag");
		}
        UICenterItem *close = getBtnByName("close");
        if (close)
        {
            stDoClick *doClick = stDoClick::create();
            doClick->endUI = this;
            close->bind(doClick);
        }
        /*
		close = UICenterItem::create("close.png","close_back.png",CCSizeMake(64,64));
		if (close)
		{
			this->addChild(close);
			close->setPosition(ccp(156,156));
		}
		
		stDoClick *doClick = stDoClick::create();
		doClick->endUI = this;
		close->bind(doClick);
         */
	}
}
UICenterItem * RpgInfoUI::getBtnByName(const std::string & name)
{
    std::map<std::string, UICenterItem*>::iterator iter = buttons.find(name);
    if ( iter != buttons.end())
    {
        return iter->second;
    }
    return NULL;
}
/**
 * 处理点击
 **/
bool RpgInfoUI::doTouch(int touchType,const CCPoint &touchPoint)
{
	if (!this->isVisible()) return false;
    for (std::map<std::string, UICenterItem*>::iterator iter = buttons.begin(); iter!=buttons.end();++iter)
    {
        bool tag = iter->second->onTouch(touchType,touchPoint);
        if (tag)
        {
            return true;
        }
    }
	for (std::vector<RpgXmlBag*>::iterator iter = bags.begin(); iter != bags.end();++iter)
	{
		if ((*iter)->doTouch(touchType,touchPoint)) return true;
	}
	//if (close) return close->onTouch(touchType,touchPoint);
	return false;
}
/**
 * 更新name 指定的数值
 */
void RpgInfoUI::updateNumber(const std::string& name,int number)
{
	UPDATE_CONTENTS_ITER iter = this->updateContents.find(name);
	if (iter != this->updateContents.end())
	{
		std::stringstream ss;
		ss << number;
		iter->second->update(ss.str().c_str());
	}
}

void RpgInfoUI::updateString(const std::string &name,const std::string& content)
{
	UPDATE_CONTENTS_ITER iter = this->updateContents.find(name);
	if (iter != this->updateContents.end())
	{
		iter->second->update(content);
	}
}
/**
 * 控制每个tag的展示
 */
void RpgInfoUI::show(const std::string &name,bool showTag)
{

}

/**
 * 展示
 */
void RpgInfoUI::show()
{
	this->setVisible(true);
}
/**
 * 隐藏
 */
void RpgInfoUI::hide()
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	setPositionX(visibleSize.width/2);
	this->setVisible(false);
}

/**
 * 创建结束
 */
RpgStepEndUI* RpgStepEndUI::create()
{
	RpgStepEndUI *pRet = new RpgStepEndUI();
    if (pRet && pRet->init("stependinfo.xml"))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

/**
 * 创建访问界面
 */
RpgVisitUI* RpgVisitUI::create()
{
	RpgVisitUI *pRet = new RpgVisitUI();
    if (pRet && pRet->init("visitnpc.xml"))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
RpgScoreUI*RpgScoreUI::create()
{
	RpgScoreUI *pRet = new RpgScoreUI();
    if (pRet && pRet->init("score.xml"))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
/**
 * 创建道具的描述
 */
RpgItemDescUI *RpgItemDescUI::create()
{
	RpgItemDescUI *pRet = new RpgItemDescUI();
    if (pRet && pRet->init("itemdesc.xml"))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
/**
 * 展示道具
 */
void RpgItemDescUI::showItem(RpgItem *item)
{
	this->setVisible(true);
	if (item->info.desc != "")
	{
		this->updateString("desc",item->info.desc);
	}
	if (item->info.name != "")
	{
		this->updateString("name",item->info.name);
	}
}
//////////////////////////RpgUI /////////////////////////////////////////////////
/**
* 初始化 设定初始布局
**/
bool RpgUI::init(RpgMap *map,RpgPlayer *player,CCNode *parent)
{
	map->ui = this;
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	this->player = player;
	this->map = map;
	// 移动盘的展示
	moveOP = RpgMoveOPUI::create(player);
	
	parent->addChild(moveOP,5);
	// 人物简介信息的展示 在左上角
	simplePlayerInfo = RpgPlayerSimpleInfoUI::create(player);
	if (simplePlayerInfo)
	{
		this->addChild(simplePlayerInfo);
		simplePlayerInfo->setPosition(ccp(0,visibleSize.height-64));
	}
	// 人物信息的展示 存盘 TODO
	playerInfo = RpgPlayerInfoUI::create();
	if (playerInfo)
	{
		this->addChild(playerInfo);
		for (int i = 0; i < 6;i++)
		{
			RpgItem * item = RpgItem::create(0);
			if (item)
			{
				std::stringstream idStr;
				idStr << i;
				playerInfo->setItem(item,i);
				CCLabelTTF * idLbl = CCLabelTTF::create(idStr.str().c_str(),"Arail",16);
				if (idLbl)
				{
					idLbl->setAnchorPoint(ccp(0,0));
					item->addChild(idLbl);
				}
			}
		}
		playerInfo->hide();
		playerInfo->updateValue("hp",20);
		playerInfo->setPosition(visibleSize.width/2 - playerInfo->getViewWidth()/2,visibleSize.height/2 - playerInfo->getViewHeight()/2);
		//playerInfo->setPosition(0,0);
	}
	// 快捷操作盘的展示 存盘
	quickOP = RpgQuickOPUI::create(player);
	parent->addChild(quickOP,5);
	quickOP->setPosition(ccp(visibleSize.width - 64,64));
	// 小地图的展示
	minMapInfo = RpgMinMapUI::create(map);
	if (minMapInfo)
	{
		this->addChild(minMapInfo);
		minMapInfo->setSize(CCSizeMake(160,160));
		minMapInfo->setPosition(ccp(visibleSize.width - 80,visibleSize.height - 80));
	}
	player->miniMap = minMapInfo;
	player->ui = this;
	// 系统操作的展示
	systemOP = RpgSystemOPUI::create(this);
	if (systemOP)
	{
		this->addChild(systemOP);
		systemOP->setPosition(ccp(visibleSize.width - 32,visibleSize.height/2 + 32));
		systemOP->hideWithOutMe();
	}
	// 地图操作盘
	mapOpUI = RpgMapOPUI::create(this);
	if (mapOpUI)
	{
		parent->addChild(mapOpUI);
		mapOpUI->setPosition(ccp(visibleSize.width - 32,32));
		mapOpUI->hideWithOutMe();
		mapOpUI->hide();
	}
	// 召唤盘 TODO

	// 包裹界面
	playerBag = RpgPlayerBag::create();
	if (playerBag)
	{
		this->addChild(playerBag);
		playerBag->setPosition(visibleSize.width/2 - playerBag->getViewWidth()/2,visibleSize.height/2 - playerBag->getViewHeight()/2);
		// TEST
		/*for (int i = 0; i < 10;i++)
		{
			RpgItem * item = RpgItem::create(0);
			if (item && i != 5)
			{
				playerBag->setItem(item,i);
			}
		}*/
		playerBag->hide();
		playerBag->bind(UIBase::EVENT_CLICK_DOWN,ui_function(RpgUI::doItemDownPlayerBag));
	}
	// 当前NPC TODO
	
	// 广告栏
	adUI = RpgAdSysUI::create();
	if (adUI)
	{
		this->addChild(adUI,-1);
		adUI->setPosition(ccp(0,visibleSize.height - 84));
		// test ui
		adUI->addContent(map->adContent);
	}

	shop = RpgShop::create();
	if (shop)
	{
		shop->view->setPosition(visibleSize.width/2 - shop->getViewWidth()/2,visibleSize.height/2 - shop->getViewHeight()/2);
		shop->view->setVisible(false);
		this->addChild(shop->view);
	}
	stepEnd = RpgStepEndUI::create();
	if (stepEnd)
	{
		stepEnd->setPosition(ccp(visibleSize.width/2 - stepEnd->getContentSize().width/2,visibleSize.height/2 - stepEnd->getContentSize().height /2));
		this->addChild(stepEnd);
        stepEnd->hide();
		
	}
	visitUI = RpgVisitUI::create();
	if (visitUI)
	{
		visitUI->setPosition(ccp(visibleSize.width/2 - visitUI->getContentSize().width/2,visibleSize.height/2 - visitUI->getContentSize().height/2));
		this->addChild(visitUI);
		visitUI->hide();
	}
	scoreUI = RpgScoreUI::create();
	if (scoreUI)
	{
		scoreUI->setPosition(ccp(visibleSize.width/2,visibleSize.height-64));
		this->addChild(scoreUI);
	}
	descUI = RpgItemDescUI::create();
	if (descUI)
	{
		this->addChild(descUI);
		descUI->setPosition(ccp(visibleSize.width/2,visibleSize.height/2 - descUI->getContentSize().height/2));
		descUI->hide();
	}
	return true;
}
/**
 * 根据uiType 展示不同的UI
 */
void RpgUI::showUI(int uiType)
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	std::vector<CCNode*> showUIs;
	switch(uiType)
	{
		case UI_TYPE_QUICK:
		{
			quickOP->show();
			mapOpUI->hide();
		}break;
		case UI_TYPE_MAP_OP:
		{
			quickOP->hide();
			mapOpUI->show();
		
		}break;
		case UI_TYPE_BAG:
		{
			if (this->playerBag->isVisible())
			{
				CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
				this->playerBag->setPositionX(visibleSize.width/2);
				this->playerBag->hide();
				this->descUI->hide();
			}
			else
			{
				this->playerBag->show();
				this->descUI->show();
			}
		}break;
		case UI_TYPE_PLAYER_INFO:
		{
			if (this->playerInfo->isVisible())
				this->playerInfo->hide();
			else
			{
				this->playerInfo->show();
			}
		}break;
		case UI_TYPE_SHOP:
		{
			if (this->shop->isVisible())
			{
				this->shop->hide();
			}
			else
			{
				this->shop->show();
			}
		}break;
	}
	int allWidthSize = 0;
	if (this->playerInfo->isVisible())
	{
		showUIs.push_back(this->playerInfo);	
		allWidthSize += this->playerInfo->getContentSize().width;
	}
	if (this->playerBag->isVisible())
	{
		showUIs.push_back(this->playerBag);	
		allWidthSize += this->playerBag->getContentSize().width;
	}
	if (this->shop->isVisible())
	{
		showUIs.push_back(this->shop);	
		allWidthSize += this->shop->getContentSize().width;
	}
	if (this->descUI->isVisible())
	{
		showUIs.push_back(this->descUI);
		allWidthSize += this->descUI->getContentSize().width;
	}
	int startx = (visibleSize.width - allWidthSize)/2;
	for (int i = 0; i < showUIs.size();i++)
	{
		CCNode *node = showUIs.at(i);
		if (node)
		{
			CCMoveTo * moveTo = CCMoveTo::create(0.2f,ccp(startx,node->getPositionY()));
			//node->setPositionX(startx);
			node->runAction(moveTo);
			startx += node->getContentSize().width;
		}
	}
	// 设置所有界面的x 轴平铺
}
//RpgPlayer * RpgUI::gloabPlayer = NULL;
#if (0)
static void doKeyDown( UINT message,WPARAM wParam,LPARAM lParam )
{
	if (!RpgUI::gloabPlayer) return;
    switch( message )
    {
    case WM_KEYDOWN:
		{
			int dirType = -1;
			switch(wParam)
			{
			case 32: // 攻击
				{
				//	RpgUI::gloabPlayer->putAtkSkill("attack");
				}break;
			case 65: // 向左
				{
					dirType = CalcDir::_DIR_LEFT;
					CCLOG("a");
				}break;
			case 87: // 向上
				{
					dirType = CalcDir::_DIR_DOWN;
					CCLOG("w");
				}break;
			case 68: // 向右
				{
					dirType = CalcDir::_DIR_RIGHT;
					CCLOG("d");
				}break;
			case 83: // 向下
				{
					dirType = CalcDir::_DIR_UP;
					CCLOG("s");
				}break;
			}
			if (dirType != -1)
			{
				RpgUI::gloabPlayer->setDir(dirType);
				RpgUI::gloabPlayer->doContinueMove(dirType);
				RpgUI::gloabPlayer->tryConbineAction(RpgMonster::ACTION_MOVE);
			}
			break;
		}
    case WM_KEYUP:
		{
			RpgUI::gloabPlayer->clearContinueMove();
		}
        break;
    case WM_CHAR:
        // Deliberately empty - all handled through key up and down events
        break;
    default:
        // Not expected to get here!!
        CC_ASSERT( false );
        break;
    }
}
#endif
RpgUI* RpgUI::create(RpgMap *map,RpgPlayer *player,CCNode *parent)
{
	RpgUI * pRet = new RpgUI();
	if (pRet && pRet->init(map,player,parent))
	{
        	pRet->autorelease();
	//	gloabPlayer = player;
	//	CCEGLView::sharedOpenGLView()->setAccelerometerKeyHook( &doKeyDown);
        	return pRet;
    	}
    	CC_SAFE_DELETE(pRet);
    return NULL;
}

/**
 * 处理点击
 * \parma touchType 点击类型
 * \param touchPoint 点击点
 */
bool RpgUI::doTouch(int touchType,CCTouch *touch)
{
	CCPoint touchPoint = touch->getLocation();
	if (playerInfo && playerInfo->doTouch(touchType,touchPoint))
	{
		return true;
	}

	if (moveOP && moveOP->doTouch(touchType,touch)){
		return true;
	}
	if (quickOP && quickOP->onTouch(touchType,touch)){
		return true;
	}
	
	if (systemOP && systemOP->onTouch(touchType,touchPoint)){
		return true;
	}
	if (playerBag && playerBag->doTouch(touchType,touchPoint))
	{
		return true;
	}
	if (mapOpUI && mapOpUI->onTouch(touchType,touchPoint))
	{
		return true;
	}
	if (shop->view && shop->view->doTouch(touchType,touchPoint))
	{
		return true;
	}
	if (stepEnd && stepEnd->doTouch(touchType,touchPoint))
	{
		return true;
	}
	if (visitUI && visitUI->doTouch(touchType,touchPoint))
	{
		return true;
	}
	if (descUI && descUI->doTouch(touchType,touchPoint))
	{
		return true;
	}
	return false;
}

/**
 * 执行定时行为
 */
void RpgUI::tick()
{
	if (adUI)
	{
		adUI->tick();
	}
}
/**
 * 显示出道具详细信息列表
 **/
void RpgUI::doItemDownPlayerBag(UIBase *base)
{
	if (!base) return;
	RpgPlayerBag * bag = (RpgPlayerBag*) base;
	if (bag)
	{
		RpgItem *item = (RpgItem*)bag->getNowChoiceItem();
		if (item)
		{
			showItemDescInfo(item);
			// 创建建筑物在场景
			RpgBuildInfo buildInfo = map->findFarmInfoById(item->info.buildId);
			RpgBuild * build = RpgBuild::createFromBuild(&buildInfo);
			if (build)
			{
				GridPoint point = player->getValidCirclePoint();
				if (!map->placeBuild(build,point.x,point.y))
				{
					build->preSetPosition(point.x*32,point.y*32);
					build->showInvalidGridBack();
					map->addBuild(build);
					map->showBlocks();
				}
				build->showHp();
			}
			if (player->isDeath())
			{
				player->setVisible(true);
				player->deathTag = false;
				player->start(RpgMonster::IDLE_ACTION);
				player->hp = 500;
				player->showHp();
				this->simplePlayerInfo->resume();
			}
			item->num --;
			if (item->num <= 0)
			{
				bag->removeItem(item);
				item->removeFromParentAndCleanup(true);
			}
			else item->showNum();
		}
	}
}

void RpgUI::updateHp(RpgPlayer *player)
{
	if (!player) return;
	if (simplePlayerInfo)
	{
		simplePlayerInfo->update();
	}
}
void RpgUI::showItemDescInfo(RpgItem *item)
{
	if (descUI)
	{
		descUI->showItem(item);
	}
}
NS_CC_END
