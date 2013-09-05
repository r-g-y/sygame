/**
 * 移动动作处理
 */
#pragma once
#include "cocos2d.h"
#include "ModelLoader.h"
#include <vector>
NS_CC_BEGIN
class AnimateMoveAction:public CCMoveTo{
public:
	virtual void update(float time);
	enum{
		_DIR_NULL = 0,
		_DIR_LEFT = 1,
		_DIR_UP = 2,
		_DIR_RIGHT = 3,
		_DIR_DOWN = 4,
		_DIR_UP_RIGHT = 5,
		_DIR_RIGHT_DOWN = 6,
		_DIR_DOWN_LEFT = 7,
		_DIR_LEFT_UP = 8,
	};
	AnimateMoveAction()
	{
		nFrames = 8;
		_modelData = NULL;
	}
	static AnimateMoveAction* create(float duration, const CCPoint& position,ModelData *modelData);
	static AnimateMoveAction* create(float duration, const CCPoint& position,const std::vector<CCSpriteFrame*> &frames);
	void stop(void);
protected:
	ModelData *_modelData;
	std::vector<CCSpriteFrame*> m_frames;
	int nFrames;
};
/**
 * 移动到指定方向
 */
class ActionMoveDirAction: public AnimateMoveAction
{
public:
	static ActionMoveDirAction* create(float duration, const CCPoint& position,const std::vector<CCSpriteFrame*> &frames,float distance,int dirType);
	virtual void startWithTarget(CCNode *pTarget);
	enum{
		TO_DIR, // 向正方向
		TO_REVERSE_DIR, // 向反方向
	};
private:
	float _distance;
	float _dirType;
	ActionMoveDirAction()
	{
		_distance = 0;
		_dirType = 0;
	}
};
NS_CC_END
