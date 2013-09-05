#pragma once
#include <vector>
#include "cocos2d.h"
NS_CC_BEGIN
/**
 * 实现容纳固定大小的sprite 且空间可以大于展示的空间 可以拖动内容，并且放置
 */
class UIBag;
class UIItem:public CCNode{
public:
	/**
	 * 在包裹中的位置
	 */
	int bagId;
	virtual void setSize(float width,float height);
	void hide();
	void show(const CCRect& viewRect);
	void setPosition(float x,float y);
	void show();
	virtual void visit();
	CREATE_FUNC(UIItem);
	virtual bool init(){return true;}
	/**
	 * 检查点是否在Item 中
	 */
	bool checkIn(float x,float y);
	virtual UIItem *clone(); // 复制自己一份
	/**
	 * 检查自身能移动到该位置否
	 */
	virtual bool canMove(int bagId){return true;}
	virtual void setOpacity(float o){}
	/**
	 * 处理点击
	 * \parma touchType 点击类型
	 * \param touchPoint 点击点
	 */
	virtual bool doTouch(int touchType,const CCPoint &touchPoint){return false;}
protected:
	friend class UIBag;
	UIBag * _bag;
	CCRect _viewRect;
	CCRect _oldRect;
	bool downTag; // 标识是否按下按钮
	bool showInView;
	UIItem()
	{
		showInView = false;
		downTag = false;
		bagId = 0;
		_bag = NULL;
	}
};

NS_CC_END
