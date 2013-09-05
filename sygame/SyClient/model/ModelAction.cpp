#include "Model.h"
#include "ModelAction.h"
NS_CC_BEGIN
ModelMoveAction* ModelMoveAction::create(float duration, const CCPoint& position)
{
    ModelMoveAction *pMoveTo = new ModelMoveAction();
    pMoveTo->initWithDuration(duration, position);
    pMoveTo->autorelease();

    return pMoveTo;
}
void ModelMoveAction::update(float time)
{
	if (m_pTarget)
	{
		m_pTarget->setPosition(ccp(m_startPosition.x + m_delta.x * time,
			m_startPosition.y + m_delta.y * time));
		CCPoint nowPoint = m_pTarget->getPosition();
		float dw = ccpDistance(nowPoint,m_startPosition);
		float da = ccpDistance(m_endPosition,m_startPosition);
		int nowFrameId = this->nFrames * (dw / da);
		Model* model = (Model*)(m_pTarget);
		if (model)
		{
			int dir = clacDir(m_startPosition,nowPoint);
			CCTexture2D *texture = model->getMoveTexutueByDirAndFrameId(dir,nowFrameId);
			if (texture)
				model->initWithTexture(texture);
		}
	}
}
int ModelMoveAction::clacDir(const CCPoint &startPoint,const CCPoint &dest)
{
	if (!m_pTarget) return _DIR_NULL;
	CCPoint nowPos = startPoint;
	int dir = _DIR_NULL;
	int x = nowPos.x;
	int y = nowPos.y;
	if (x == dest.y && y > dest.y)
	{
		return _DIR_UP;
	}
	else if ( x < dest.x && y > dest.y)
	{
		return _DIR_UP_RIGHT;
	}
	else if ( x < dest.x && y == dest.y)
	{
		return _DIR_RIGHT;
	}
	else if ( x < dest.x && y < dest.y)
	{
		return _DIR_RIGHT_DOWN;
	}
	else if ( x == dest.x && y < dest.y)
	{
		return _DIR_DOWN;
	}
	else if ( x > dest.x && y < dest.y)
	{
		return _DIR_DOWN_LEFT;
	}
	else if ( x > dest.x && y == dest.y)
	{
		return _DIR_LEFT;
	}
	else if ( x > dest.x && y > dest.y)
	{
		return _DIR_LEFT_UP;
	}
	return _DIR_NULL;
}
NS_CC_END