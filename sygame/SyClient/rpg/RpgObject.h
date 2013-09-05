#pragma once
#include "cocos2d.h"
#include "Cartoon.h"
#include "quickidmanager.h"
NS_CC_BEGIN
/**
 * 格子店
 */
class GridPoint{
public:
	int x;
	int y;
	GridPoint()
	{
		x = y = 0;
	}
	GridPoint(int x,int y)
	{
		this->x = x;
		this->y = y;
	}
	GridPoint & operator=(const GridPoint &point)
	{
		this->x = point.x;
		this->y = point.y;
		return *this;
	}
	bool isInvalid() const
	{
		return (x == -1 || y == -1);
	}
	bool equal(const GridPoint &point)
	{
		return (x == point.x && y == point.y);
	}
	bool equal(const GridPoint &point,int range)
	{
		return (abs(x - point.x) < range && abs(y - point.y) < range);
	}
};
/**
 * rpg 对象
 */
class RpgObject:public Cartoon,public QuickObjectBase{
public:
	int objectType; // 物体类型
	int quickId; //快速引索号
	enum{
		RPG_BUILD = 0, // 建筑物
		RPG_MONSTER = 1, // 怪物
		RPG_PLAYER = 2, // 玩家
	};
	RpgObject(int objectType):objectType(objectType)
	{
		
	}
	virtual void nextStep(){}
	virtual CCFiniteTimeAction* v_makeAction(int actionType)
	{
		return NULL;
	}

	virtual void v_putAction(CCFiniteTimeAction *action){}
	/**
	 * 设置快速唯一索引
	 * \param uniqueQuickId 设置快速唯一索引
	 */
	virtual void setUniqueQuickId(int uniqueQuickId)
	{
		this->quickId = uniqueQuickId;
	}
	/**
	 * 获取快速唯一索引
	 * \return 快速唯一索引
	 */
	virtual int getUniqueQuickId() 
	{
		return quickId;
	}
	bool isType(int type)
	{
		return objectType == type;
	}
};

class RpgObjectManager:public QuickIdManager{
public:
	/**
	 * 单例模式 
	 * 获取Rpg对象管理器
	 */
	static RpgObjectManager & getMe(){static RpgObjectManager rom; return rom;}
};

#define theObjectM RpgObjectManager::getMe()
NS_CC_END