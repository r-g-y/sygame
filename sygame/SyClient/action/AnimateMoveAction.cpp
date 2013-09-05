#include "Model.h"
#include "AnimateMoveAction.h"
NS_CC_BEGIN
AnimateMoveAction* AnimateMoveAction::create(float duration, const CCPoint& position,ModelData *modelData)
{
    AnimateMoveAction *pMoveTo = new AnimateMoveAction();
    pMoveTo->initWithDuration(duration, position);
    pMoveTo->autorelease();
	pMoveTo->_modelData = modelData;
    return pMoveTo;
}
void AnimateMoveAction::update(float time)
{
	if (m_pTarget)
	{
		m_pTarget->setPosition(ccp(m_startPosition.x + m_delta.x * time,
			m_startPosition.y + m_delta.y * time));
		CCPoint nowPoint = m_pTarget->getPosition();
		float dw = ccpDistance(nowPoint,m_startPosition);
		float da = ccpDistance(m_endPosition,m_startPosition);
		int nowFrameId = this->nFrames * (dw / da);
		if (_modelData)
		{
			CCTexture2D *texture = _modelData->getTextureById(nowFrameId);
			if (texture)
				((CCSprite*)m_pTarget)->initWithTexture(texture);
		}
		else if (nowFrameId < m_frames.size() )
		{
			CCSpriteFrame *frame = m_frames[nowFrameId];
			if (frame)
			{
				CCSprite *sprite = static_cast<CCSprite*>(m_pTarget);
				if (sprite)
					sprite->setDisplayFrame(frame);
			}
		}
	}
}
void AnimateMoveAction::stop(void)
{
	
	for (std::vector<CCSpriteFrame*>::iterator iter = m_frames.begin(); iter != m_frames.end();++iter)
	{
		(*iter)->release();
	}
	CCMoveTo::stop();
}
AnimateMoveAction* AnimateMoveAction::create(float duration, const CCPoint& position,const std::vector<CCSpriteFrame*> &frames)
{
	AnimateMoveAction *pMoveTo = new AnimateMoveAction();
    pMoveTo->initWithDuration(duration, position);
    pMoveTo->autorelease();
    pMoveTo->m_frames = frames;
	pMoveTo->nFrames = frames.size();
	return pMoveTo;
}
ActionMoveDirAction* ActionMoveDirAction::create(float duration, const CCPoint& position,const std::vector<CCSpriteFrame*> &frames,float distance,int dirType)
{
	ActionMoveDirAction *pMoveTo = new ActionMoveDirAction();
    pMoveTo->initWithDuration(duration, position);
    pMoveTo->autorelease();
    pMoveTo->m_frames = frames;
	pMoveTo->nFrames = frames.size();
	pMoveTo->_distance = distance;
	pMoveTo->_dirType = dirType;
	return pMoveTo;
}

void ActionMoveDirAction::startWithTarget(CCNode *pTarget)
{
	 CCActionInterval::startWithTarget(pTarget);
     m_startPosition = pTarget->getPosition();
	 // 根据dir distance 确定终点
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
NS_CC_END