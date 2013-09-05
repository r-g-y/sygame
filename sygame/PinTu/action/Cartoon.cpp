#include "Cartoon.h"
NS_CC_BEGIN

/**
* 创建动画信息 并且autorelease
*/
CartoonInfo* CartoonInfo::create(const CartoonInfo &info)
{
	CartoonInfo * pRet = new CartoonInfo();
	*pRet = info;
    pRet->autorelease();
    return pRet;
}
CartoonInfo* CartoonInfo::create()
{
	CartoonInfo * pRet = new CartoonInfo();
    pRet->autorelease();
    return pRet;
}
/**
 * 根据动画创建自身动作
 */
CCFiniteTimeAction * CartoonInfo::createAction(CCNode * self,CCNode *parent,const cocos2d::CCPoint &point,CCNode *target)
{
	// 判定是移动还是静止
	CCFiniteTimeAction *action = NULL;
	if (!point.equals(CCPointZero) && frameType != TIME_FRAMES)
	{
		// 移动动画
		CartoonMoveAction * moveAction = CartoonMoveAction::create(*this,point);
		action = moveAction;
		if (cartoonType == OTHER)
		{
			moveAction->isTempTarget = true;
		}
	}
	else
	{
		// 静止动画
		CartoonAction* cartoonAction = CartoonAction::create(*this);
		action = cartoonAction;
		if (cartoonType == OTHER)
		{
			cartoonAction->isTempTarget = true;
		}
	}
	if (!action) return NULL;

	switch(cartoonType)
	{
		case OTHER:
		{
			CCSprite * temp = CCSprite::create();
			if (temp)
			{
				parent->addChild(temp);
				//temp->setAnchorPoint(ccp(0,0));
				temp->setPosition(self->getPosition());
				action->setTarget(temp);
			}
		}break;
	}
	return action;
}
/**
 * 增加下一个动画序列
 */
CartoonInfo* CartoonInfo::addNextCartoonInfo(int nextConbineType,const CartoonInfo& info)
{
	this->nextConbineType = nextConbineType;
	this->nextCartoon = CartoonInfo::create(info);
	return nextCartoon;
}
/**
 * 释放自身
 */
void CartoonInfo::release()
{
	for (std::vector<CCSpriteFrame*>::iterator iter = frames.begin(); iter != frames.end();++iter)
	{
		if (*iter)
		{
			(*iter)->release();
		}
	}
}
CCAnimation * CartoonInfo::createAnimation()
{
	CCAnimation* animation = CCAnimation::create();
	for (int i = 0; i < frames.size(); i++)
	{
		animation->addSpriteFrame(frames[i]);
	}
	if (frames.empty()) return NULL;
	animation->setDelayPerUnit(needTime / frames.size());
	animation->setRestoreOriginalFrame(true);
	return animation;
}
void CartoonMoveAction::update(float time)
{
	if (m_pTarget)
	{
		m_pTarget->setPosition(ccp(m_startPosition.x + m_delta.x * time,
			m_startPosition.y + m_delta.y * time));
		CCPoint nowPoint = m_pTarget->getPosition();
		float dw = ccpDistance(nowPoint,m_startPosition);
		float da = ccpDistance(m_endPosition,m_startPosition);
		int frameSize = this->cartoonInfo.frames.size();
		int nowFrameId = frameSize - 1;
		if (da) nowFrameId = frameSize * (dw / da);
		if (nowFrameId >= 0 && nowFrameId < frameSize)
		{
			CCSpriteFrame *frame = this->cartoonInfo.frames[nowFrameId];
			if (frame)
			{
				CCSprite *sprite = static_cast<CCSprite*>(m_pTarget);
				if (sprite)
					sprite->setDisplayFrame(frame);
			}
		}
	}
}
	
CartoonMoveAction* CartoonMoveAction::create(const CartoonInfo &cartoonInfo, const CCPoint& position)
{
	CartoonMoveAction *pMove = new CartoonMoveAction();
	pMove->init(cartoonInfo,position);
    pMove->autorelease();
    return pMove;
}
/**
* 初始化cartoon 移动行为
*/
bool CartoonMoveAction::init(const CartoonInfo &cartoonInfo, const CCPoint& position)
{
	this->cartoonInfo = cartoonInfo;
	initWithDuration(cartoonInfo.needTime, position);
	return true;
}

void CartoonMoveAction::stop(void)
{
	CCNode *tempTarget = m_pTarget; 
	CCMoveTo::stop();
	cartoonInfo.release();
	if (isTempTarget)
	{
		tempTarget->removeFromParentAndCleanup(true);
	}
}
CartoonFollowAction* CartoonFollowAction::create(const CartoonInfo &cartoonInfo,CCNode *target)
{
	CartoonFollowAction *pMoveTo = new CartoonFollowAction();
	pMoveTo->initWithDuration(cartoonInfo.needTime, ccp(0,0));
    pMoveTo->autorelease();
	pMoveTo->cartoonInfo = cartoonInfo;
	pMoveTo->initWithAnimation(pMoveTo->cartoonInfo.createAnimation());
	if (target)
	{
		pMoveTo->target = target;
		target->retain();
	}
	return pMoveTo;
}
bool CartoonFollowAction::initWithAnimation(CCAnimation *pAnimation)
{
	float singleDuration = pAnimation->getDuration();

    if ( CCActionInterval::initWithDuration(singleDuration * pAnimation->getLoops() ) ) 
    {
        m_nNextFrame = 0;
        setAnimation(pAnimation);
        m_pOrigFrame = NULL;
        m_uExecutedLoops = 0;

        m_pSplitTimes->reserve(pAnimation->getFrames()->count());

        float accumUnitsOfTime = 0;
        float newUnitOfTimeValue = singleDuration / pAnimation->getTotalDelayUnits();

        CCArray* pFrames = pAnimation->getFrames();
        CCARRAY_VERIFY_TYPE(pFrames, CCAnimationFrame*);

        CCObject* pObj = NULL;
        CCARRAY_FOREACH(pFrames, pObj)
        {
            CCAnimationFrame* frame = (CCAnimationFrame*)pObj;
            float value = (accumUnitsOfTime * newUnitOfTimeValue) / singleDuration;
            accumUnitsOfTime += frame->getDelayUnits();
            m_pSplitTimes->push_back(value);
        }    
        return true;
    }
    return false;
}
void CartoonFollowAction::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
    CCSprite *pSprite = (CCSprite*)(pTarget);

    CC_SAFE_RELEASE(m_pOrigFrame);

    if (m_pAnimation->getRestoreOriginalFrame())
    {
        m_pOrigFrame = pSprite->displayFrame();
        m_pOrigFrame->retain();
    }
    m_nNextFrame = 0;
    m_uExecutedLoops = 0;
}
/**
* 更新
*/
void CartoonFollowAction::update(float t)
{
	if (target && target->retainCount() > 1)
	{
		m_endPosition = target->getPosition();
		m_delta = ccpSub(m_endPosition, m_startPosition);
	}
	if (m_pTarget)
	{
		m_pTarget->setPosition(ccp(m_startPosition.x + m_delta.x * t,
			m_startPosition.y + m_delta.y * t));
		// 根据时间计算帧数
		updateAnimate(t);
	}
}
void CartoonFollowAction::updateAnimate(float t)
{
	// if t==1, ignore. Animation should finish with t==1
    if( t < 1.0f ) {
        t *= m_pAnimation->getLoops();

        // new loop?  If so, reset frame counter
        unsigned int loopNumber = (unsigned int)t;
        if( loopNumber > m_uExecutedLoops ) {
            m_nNextFrame = 0;
            m_uExecutedLoops++;
        }

        // new t for animations
        t = fmodf(t, 1.0f);
    }

    CCArray* frames = m_pAnimation->getFrames();
    unsigned int numberOfFrames = frames->count();
    CCSpriteFrame *frameToDisplay = NULL;

    for( unsigned int i=m_nNextFrame; i < numberOfFrames; i++ ) {
        float splitTime = m_pSplitTimes->at(i);

        if( splitTime <= t ) {
            CCAnimationFrame* frame = (CCAnimationFrame*)frames->objectAtIndex(i);
            frameToDisplay = frame->getSpriteFrame();
            ((CCSprite*)m_pTarget)->setDisplayFrame(frameToDisplay);
            m_nNextFrame = i+1;

            break;
        }
    }
}
/**
* 为使动作更加连续
*/
void CartoonFollowAction::stop(void)
{
	if (target)
		target->release();
	cartoonInfo.release();
	if (m_pAnimation->getRestoreOriginalFrame() && m_pTarget)
    {
        ((CCSprite*)(m_pTarget))->setDisplayFrame(m_pOrigFrame);
    }

    CCActionInterval::stop();
}
/**
 * 创建卡通动作
 */
 CartoonAction* CartoonAction::create(const CartoonInfo &cartoonInfo)
 {
	CartoonAction *pAnimate = new CartoonAction();
	if (pAnimate->init(cartoonInfo))
	{
		pAnimate->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pAnimate);
	}
	return pAnimate;
 
}
/**
 * 初始化动作
 */
 bool CartoonAction::init(const CartoonInfo &cartoonInfo)
 {
	 CartoonInfo c = cartoonInfo;
	 CCAnimation * pAnimation = c.createAnimation();
	 if (!pAnimation) return false;
	 initWithAnimation(pAnimation);
	 c.release();
	 return true;
 }
 /**
 * 释放自己所占有的帧
 */
void CartoonAction::stop(void)
{
	CCNode *tempTarget = m_pTarget;
	CCAnimate::stop();
	if (isTempTarget)
	{
		tempTarget->removeFromParentAndCleanup(true);
	}
}
/** 自身做改变
 * 目标点 position
 * 动态构建Action
 */
void Cartoon::runSelfAction(const CartoonInfo &info,const CCPoint &position)
{
	if (position.x != 0 && position.y != 0)
	{
		// 创建移动动画
		CartoonMoveAction *action = CartoonMoveAction::create(info,position);
		if (action)
		{
			CCAction *seqaction = CCSequence::create(action,
					CCCallFunc::create(this, callfunc_selector(Cartoon::doCartoonEnd)),NULL);
			if (seqaction)
				sprite->runAction(seqaction);
		}
	}
	else
	{
		// 创建自身动画 相当于静止动画
		CartoonAction *action = CartoonAction::create(info);
		CCFiniteTimeAction *seqaction = CCSequence::create(action,
					CCCallFunc::create(this, callfunc_selector(Cartoon::doCartoonEnd)),NULL);
		if (seqaction)
			sprite->runAction(seqaction);
	}
}

/**
 * 附加在离身的动作 指定目的地
 **/
void Cartoon::runFlyAction(CCNode* parent,const CartoonInfo &info,const cocos2d::CCPoint &point)
{
	CCSprite *tempSprite = CCSprite::create();
	parent->addChild(tempSprite);
	if (point.x != 0 && point.y != 0)
	{
		// 创建移动动画
		CartoonMoveAction *moveAction = CartoonMoveAction::create(info,point);
		if (moveAction)
		{
			CCAction *action = CCSequence::create(moveAction,
			CCCallFuncND::create(this, callfuncND_selector(Cartoon::flyEnd), (void*)tempSprite),NULL);
	
			tempSprite->runAction(action);
		}
	}
	else
	{
		// 创建离身动画 相当于头顶特效
		CartoonAction *moveAction = CartoonAction::create(info);
		if (moveAction)
		{
			CCAction *action = CCSequence::create(moveAction,
			CCCallFuncND::create(this, callfuncND_selector(Cartoon::flyEnd), (void*)tempSprite),NULL);
	
			tempSprite->runAction(action);
		}
	}
}
/**
 * 创建跟随动画
 * 碰到对象就消失
 */
void Cartoon::runFllowAction(CCNode *parent,const CartoonInfo&info,CCNode *target)
{
	CartoonFollowAction * moveAction = CartoonFollowAction::create(info,target);
	if (moveAction)
	{
		CCSprite *tempSprite = CCSprite::create();
		parent->addChild(tempSprite);
		CCAction *action = CCSequence::create(moveAction,
			CCCallFuncND::create(this, callfuncND_selector(Cartoon::flyEnd), (void*)tempSprite),NULL);
	
		tempSprite->runAction(action);
	}
}
/**
 * 组合动画的播放 新式动作
 * \param parent fly 承载节点
 * \param info 动画信息
 * \param point 目的地
 * \param target 跟随对象
 * CartoonInfo::create()->addNextCartoonInfo(CartoonInfo::TOGETHER,info1)->addNextCartoonInfo(CartoonInfo::TOGETHER,info2);
 */
void Cartoon::runAction(CCNode *parent,const CartoonInfo*info,const cocos2d::CCPoint &point,CCNode *target)
{
	CartoonInfo * root =(CartoonInfo*) info;
	int nextConbine = root->nextConbineType;
	CCFiniteTimeAction *preAction = NULL;
	CCFiniteTimeAction *nowAction = NULL;
	while(root)
	{
		nowAction = root->createAction(sprite,parent,point,target);
		if (!preAction)
		{
			preAction = CCSequence::create(nowAction,NULL);
		}
		else
		{
			switch(nextConbine)
			{
				case CartoonInfo::SEQUENCE:
				{
					preAction = CCSequence::create(preAction,nowAction,NULL);
				}break;
				case CartoonInfo::TOGETHER:
				{
					preAction = CCSpawn::create(preAction,nowAction,NULL);
				}break;
			}
		}
		nextConbine = root->nextConbineType;
		root = root->nextCartoon;
	}
	if (preAction)
	{
		CCFiniteTimeAction *seqaction = CCSequence::create(preAction,
					CCCallFunc::create(this, callfunc_selector(Cartoon::doCartoonEnd)),NULL);
		if (seqaction)
			sprite->runAction(seqaction);
	}
}
void Cartoon::flyEnd(CCNode *pSender,void *arg)
{
	if (pSender)
	{
		pSender->removeFromParentAndCleanup(true);
	}
	doCartoonEnd();
}
void Cartoon::doCartoonEnd()
{
	nextStep();
	this->putConbineAction();
}

 /**
* 以某个行为为初始执行状态
*/
void Cartoon::start(int id)
{
	tryNextAction(id);
	CartoonInfo *cartoonInfo = makeCartoon(id);
	if (cartoonInfo)
		putCartoon(cartoonInfo);
}
void Cartoon::tryNextAction(int id)
{
	actionType = id;
}
/**
* 并行的执行行为
*/
void Cartoon::tryConbineAction(int type)
{
	actionType |= type;
}

/**
* 检查当前行为
*/
bool Cartoon::isNowAction(int actionType)
{
	if (oldActionType == -1) return false;
	return (oldActionType & actionType);
}
/**
* 下一个动作里去除该系
*/
void Cartoon::clearAction(int type)
{
	actionType &= ~type;
}
/**
* 根据优先级 生成相应的动作
*/
void Cartoon::putConbineAction()
{

	for (int i = 0; i < 16;i++)
	{
		if (actionType & (1 << i))
		{
			CartoonInfo *cartoonInfo = makeCartoon(1<<i);
			if (cartoonInfo)
			{
				clearAction(1<<i);
				putCartoon(cartoonInfo);
				return ;
			}
		}
	}
	CartoonInfo *cartoonInfo = makeCartoon(1<<5);
	if (cartoonInfo)
	{
		clearAction(1<<5);
		putCartoon(cartoonInfo);
		return ;
	}
}

void Cartoon::setAction(int actionType)
{
	oldActionType = actionType;
}
CartoonInfo* Cartoon::makeCartoon(int actionType)
{
	oldActionType = actionType; 
	return v_makeCartoon(actionType);
}
void Cartoon::putCartoon(CartoonInfo *action)
{
	v_putCartoon(action);
}
NS_CC_END