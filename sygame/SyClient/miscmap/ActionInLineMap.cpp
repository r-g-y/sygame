#include "ActionInLineMap.h"
NS_CC_BEGIN

ActionInLineMap* ActionInLineMap::actionWithDuration(float duration, const CCPoint& position)
{
    return ActionInLineMap::create(duration, position);
}

ActionInLineMap* ActionInLineMap::create(float duration, const CCPoint& position)
{
    ActionInLineMap *pMoveTo = new ActionInLineMap();
    pMoveTo->initWithDuration(duration, position);
    pMoveTo->autorelease();

    return pMoveTo;
}

bool ActionInLineMap::initWithDuration(float duration, const CCPoint& position)
{
    if (CCActionInterval::initWithDuration(duration))
    {
        m_endPosition = position;
        return true;
    }

    return false;
}

CCObject* ActionInLineMap::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    ActionInLineMap* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (ActionInLineMap*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new ActionInLineMap();
        pZone = pNewZone = new CCZone(pCopy);
    }

    CCActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_endPosition);

    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void ActionInLineMap::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);
    m_startPosition = pTarget->getPosition();
    m_delta = ccpSub(m_endPosition, m_startPosition);
}
/**
 * 在该类中根据action 处理位置的变换
 */
void ActionInLineMap::update(float time)
{
    if (m_pTarget)
    {
        m_pTarget->setPosition(ccp(m_startPosition.x + m_delta.x * time,
            m_startPosition.y + m_delta.y * time));
    }
}

NS_CC_END