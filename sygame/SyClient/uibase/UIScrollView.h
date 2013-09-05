/**
 * 实现带有裁剪区域的滚动视图
 */
#pragma once
#include "cocos2d.h"
#include "UIBase.h"
NS_CC_BEGIN
class UIScrollView:public UIBase{
public:
	static UIScrollView* create(float x,float y,float w,float h);
	/**
	 * 设置大小
	 */
	void setSize(float width,float height);
	/**
	 * 设置位置
	 */
	void setPosition(float x,float y);
	/**
	 * 增加内容
	 */
	bool addContent(UIBase*node);
	/**
	 * 渲染前调用
	 */
	virtual void visit();
	/**
	 * create 函数定义
	 */
	CREATE_FUNC(UIScrollView);
	bool init(){return true;}
	/** 
	 * 检查是否在区域里
	 */
	bool touchDown(float x,float y);
	/**
	 * 更新位置
	 */
	bool touchMove(float x,float y);
	/**
	 * 停止拖动
	 */
	bool touchEnd(float x,float y);

	bool checkIn(float x,float y);
	enum{
		UP_DOWN = 1, // 上下移动
		LEFT_RIGHT = 2, // 左右移动
		SCROLL_ALL = 3, // 上下左右移动
	};
	/**
	 * 设置滚动类型
	 */
	void setScrollType(int scollType)
	{
		_scrollDir = scollType;
	}
	void setScrollAble(bool tag)
	{
		scrollTag = tag;
	}
	virtual void setEditable(bool tag){
		_editable = tag;
	}
	UIBase* child;
protected:
	bool scrollTag;
	int _scrollDir; // 设置滚动方向
	bool _touchIn; // 是否在该区域内操作
	CCPoint nowTouchPoint; // 当前touch 的点
	CCPoint startTouchPoint;
	bool _editable;
	float _x;
	float _y;
	float _width;
	float _height;
	UIScrollView()
	{
		scrollTag = true;
		_editable = true;
		_x = _y = _width = _height = 0;
		child = NULL;
		_scrollDir = LEFT_RIGHT;
		_touchIn = false;
	}
};

NS_CC_END