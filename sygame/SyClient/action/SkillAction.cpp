#include "SkillAction.h"

NS_CC_BEGIN

SkillAction* SkillAction::create(float duration, const CCPoint& position,const std::vector<CCSpriteFrame*> &frames,CCParticleSystemQuad *actionParticle)
{
	SkillAction *pMoveTo = new SkillAction();
    pMoveTo->initWithDuration(duration, position);
    pMoveTo->autorelease();
	pMoveTo->m_frames = frames;
	pMoveTo->actionParticle = actionParticle;
	pMoveTo->nFrames = frames.size();
    return pMoveTo;
}
SkillAction* SkillAction::create(float duration,const CCPoint &src, const CCPoint& position,const std::vector<CCSpriteFrame*> &frames,CCParticleSystemQuad *actionParticle)
{
	SkillAction *pMoveTo = new SkillAction();
    pMoveTo->initWithDuration(duration, position);
    pMoveTo->autorelease();
	pMoveTo->m_frames = frames;
	pMoveTo->actionParticle = actionParticle;
	pMoveTo->nFrames = frames.size();
	pMoveTo->m_startPosition = src;
	pMoveTo->m_delta = ccpSub(pMoveTo->m_endPosition, pMoveTo->m_startPosition);
	pMoveTo->hadFrom = true;
    return pMoveTo;
}

SkillAction* SkillAction::create(float duration,const std::vector<CCSpriteFrame*> &frames)
{
	SkillAction *pMoveTo = new SkillAction();
    pMoveTo->initWithDuration(duration, ccp(0,0));
    pMoveTo->autorelease();
	pMoveTo->m_frames = frames;
	pMoveTo->actionParticle = NULL;
	pMoveTo->nFrames = frames.size();
	pMoveTo->hadEnd = false;
    return pMoveTo;
}
SkillAction* SkillAction::create(float duration,const std::vector<CCSpriteFrame*> &frames,CCNode *target)
{
	SkillAction *pMoveTo = new SkillAction();
    pMoveTo->initWithDuration(duration, ccp(0,0));
    pMoveTo->autorelease();
	pMoveTo->m_frames = frames;
	pMoveTo->actionParticle = NULL;
	pMoveTo->nFrames = frames.size();
	pMoveTo->hadEnd = true;
	if (target)
	{
		pMoveTo->target = target;
		target->retain();
	}
	return pMoveTo;
}
void SkillAction::startWithTarget(CCNode *pTarget)
{
	if (!hadFrom)
	{
		CCActionInterval::startWithTarget(pTarget);
		m_startPosition = pTarget->getPosition();
		m_delta = ccpSub(m_endPosition, m_startPosition);
	}
}
void SkillAction::update(float time)
{
	if (!actionSprite && m_frames.size()) actionSprite = CCSprite::create();
	if (target && target->retainCount() > 1)
	{
		{
			m_endPosition = target->getPosition();
			m_delta = ccpSub(m_endPosition, m_startPosition);
		}
	}
	if (actionSprite && m_pTarget && !actionSprite->getParent())
	{
		m_pTarget->getParent()->addChild(actionSprite); // TODO 一般更改
		actionSprite->setDisplayFrame(m_frames.at(0));
		actionSprite->setAnchorPoint(ccp(0,0));
	}
	if (actionParticle && hadEnd)
		actionParticle->setPosition(ccp(m_startPosition.x + m_delta.x * time,
			m_startPosition.y + m_delta.y * time));
	if (m_pTarget)
	{
		if (hadEnd)
		actionSprite->setPosition(ccp(m_startPosition.x + m_delta.x * time,
			m_startPosition.y + m_delta.y * time));
		CCPoint nowPoint = m_pTarget->getPosition();
		float dw = ccpDistance(nowPoint,m_startPosition);
		float da = ccpDistance(m_endPosition,m_startPosition);
		int nowFrameId = this->nFrames * (dw / da);
		if (_modelData)
		{
			CCTexture2D *texture = _modelData->getTextureById(nowFrameId);
			if (texture)
				((CCSprite*)actionSprite)->initWithTexture(texture);
		}
		else if (nowFrameId < m_frames.size() )
		{
			CCSpriteFrame *frame = m_frames[nowFrameId];
			if (frame)
			{
				((CCSprite*)actionSprite)->setDisplayFrame(frame);
			}
		}
	}
}

void SkillAction::stop(void)
{
	if (actionSprite)
	{
		actionSprite->setVisible(false);
		actionSprite->removeFromParentAndCleanup(true);
		actionSprite = NULL;
	}
	if (actionParticle)
		actionParticle->stopSystem();
	CCActionInterval::stop();
	if (target)
	{
		target->release();
	}
}

void SkillParticleAction::update(float time)
{
	if (actionParticle)
    {
        actionParticle->setPosition(ccp(m_startPosition.x + m_delta.x * time,
            m_startPosition.y + m_delta.y * time));
    }
}
SkillParticleAction* SkillParticleAction::create(CCParticleSystemQuad *actionParticle,float duration, const CCPoint& dest)
{
	SkillParticleAction *pMoveTo = new SkillParticleAction();
    pMoveTo->initWithDuration(duration, dest);
    pMoveTo->autorelease();
	pMoveTo->actionParticle = actionParticle;
    return pMoveTo;
}
SkillParticleAction* SkillParticleAction::create(CCParticleSystemQuad *actionParticle,float duration, const CCPoint& src,const CCPoint &dest)
{
	SkillParticleAction *pMoveTo = new SkillParticleAction();
    pMoveTo->initWithDuration(duration, dest);
    pMoveTo->autorelease();
	pMoveTo->actionParticle = actionParticle;
	pMoveTo->m_startPosition = src;
	pMoveTo->m_delta = ccpSub(pMoveTo->m_endPosition, pMoveTo->m_startPosition);
	pMoveTo->hadFrom = true;
    return pMoveTo;
}
void SkillParticleAction::startWithTarget(CCNode *pTarget)
{
	if (!hadFrom)
	{
		CCActionInterval::startWithTarget(pTarget);
		m_startPosition = pTarget->getPosition();
		m_delta = ccpSub(m_endPosition, m_startPosition);
		if (!actionParticle->getParent())
			pTarget->addChild(actionParticle);
	}
}
void SkillParticleAction::stop(void)
{
	if (actionParticle)
	{
		actionParticle->stopSystem();
		actionParticle->removeFromParentAndCleanup(true);
	}
	CCActionInterval::stop();
}

SkillMoveAction* SkillMoveAction::create(float duration, const CCPoint& position,const std::vector<CCSpriteFrame*> &frames,float distance,int moveType)
{
	SkillMoveAction *pMoveTo = new SkillMoveAction();
    pMoveTo->initWithDuration(duration, position);
    pMoveTo->autorelease();
	pMoveTo->m_frames = frames;
	pMoveTo->nFrames = frames.size();
	pMoveTo->_dirType = moveType;
	pMoveTo->_distance = distance;
    return pMoveTo;
}

void SkillMoveAction::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	m_startPosition = pTarget->getPosition();
	// 根据方向 distance 计算终点
	if (_dirType == TO_DIR)
	{
		float distance = ccpDistance(m_startPosition,m_endPosition);
		if (distance)
		{
			CCPoint dp = ccpSub(m_endPosition,m_startPosition);
			m_endPosition = ccp(m_startPosition.x + dp.x *  _distance  / distance,m_startPosition.y + dp.y *  _distance  / distance);
		}
	}
	if (_dirType == TO_REVERSE_DIR)
	{
		float distance = ccpDistance(m_startPosition,m_endPosition);
		if (distance)
		{
			CCPoint dp = ccpSub(m_endPosition,m_startPosition);
			m_endPosition = ccp(m_startPosition.x - dp.x *  _distance  / distance,m_startPosition.y - dp.y *  _distance  / distance);
		}
	}
	m_delta = ccpSub(m_endPosition, m_startPosition);
}

// 旋转自身的动作
SkillRotateBy* SkillRotateBy::create(float duration, float fDeltaAngle,const CCPoint& newAnchorPoint,const CCPoint &offsetPoint)
{
	SkillRotateBy *pRotateBy = new SkillRotateBy();
    pRotateBy->initWithDuration(duration, fDeltaAngle);
    pRotateBy->autorelease();
	pRotateBy->nowPoint = offsetPoint;
	pRotateBy->oldAnchorPoint = newAnchorPoint;
    return pRotateBy;
}
void SkillRotateBy::startWithTarget(CCNode *pTarget)
{
	CCRotateBy::startWithTarget(pTarget);
	CCPoint nowAnchorPoint = pTarget->getAnchorPoint();
	pTarget->setAnchorPoint(oldAnchorPoint);
	oldAnchorPoint = nowAnchorPoint;

	CCPoint pNow = pTarget->getPosition();
	pTarget->setPosition(ccpAdd(pNow,nowPoint));
	nowPoint = pNow;
}
void SkillRotateBy::update(float time)
{
	CCRotateBy::update(time);
}
void SkillRotateBy::stop(void)
{
	if (m_pTarget)
	{
		m_pTarget->setAnchorPoint(oldAnchorPoint);
		m_pTarget->setPosition(nowPoint);
	}
	CCRotateBy::stop();
	
}
// 自身缩放动作
SkillScaleTo* SkillScaleTo::create(float duration, float scale,const CCPoint& newAnchorPoint,const CCPoint &offsetPoint)
{
	SkillScaleTo *pScaleTo = new SkillScaleTo();
    pScaleTo->initWithDuration(duration, scale);
    pScaleTo->autorelease();
	pScaleTo->nowPoint = offsetPoint;
	pScaleTo->oldAnchorPoint = newAnchorPoint;
    return pScaleTo;
}
void SkillScaleTo::startWithTarget(CCNode *pTarget)
{
	CCScaleTo::startWithTarget(pTarget);
	CCPoint nowAnchorPoint = pTarget->getAnchorPoint();
	pTarget->setAnchorPoint(oldAnchorPoint);
	oldAnchorPoint = nowAnchorPoint;

	CCPoint pNow = pTarget->getPosition();
	pTarget->setPosition(ccpAdd(pNow,nowPoint));
	nowPoint = pNow;
}
void SkillScaleTo::update(float time)
{
	CCScaleTo::update(time);
}
void SkillScaleTo::stop(void)
{
	if (m_pTarget)
	{
		m_pTarget->setAnchorPoint(oldAnchorPoint);
		m_pTarget->setPosition(nowPoint);
	}
	CCScaleTo::stop();
}
NS_CC_END