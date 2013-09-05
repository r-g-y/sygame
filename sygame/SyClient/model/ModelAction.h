#pragma once
#include "cocos2d.h"
NS_CC_BEGIN
class Model;
class ModelMoveAction:public CCMoveTo{
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
	ModelMoveAction()
	{
		nFrames = 8;
	}
	static ModelMoveAction* ModelMoveAction::create(float duration, const CCPoint& position);

private:
	int nFrames;
	int clacDir(const CCPoint &startPoint,const CCPoint &dest);
};

NS_CC_END