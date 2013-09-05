#pragma once
#include "cocos2d.h"
class CalcDir{
public:
	static int calcDir(const cocos2d::CCPoint &src,const cocos2d::CCPoint &dest)
	{
		int x = src.x;
		int y = src.y;
		if (x == dest.x && y > dest.y)
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
	enum{
		_DIR_NULL = -1,
		_DIR_LEFT = 4,
		_DIR_UP = 2,
		_DIR_RIGHT = 0,
		_DIR_DOWN = 6,
		_DIR_UP_RIGHT = 1,
		_DIR_RIGHT_DOWN = 7,
		_DIR_DOWN_LEFT = 5,
		_DIR_LEFT_UP = 3,
	};
};