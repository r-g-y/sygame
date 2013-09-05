#pragma once
#include "cocos2d.h"
#include "UIBase.h"
NS_CC_BEGIN
/**
 * 选择框
 */
class UIChoice:public UIBase{
public:
	/**
	 * 创建选择框
	 */
	static UIChoice*create(const char *upSprite,const char *downSprite);

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
	
	void setEditable(bool tag){
		_editable = tag;
	}
	/**
	 * 设置位置
	 */
	virtual void setPosition(float x,float y);
	
	/**
	 * 设置大小
	 */
	virtual void setSize(float w,float h);
	bool isChoiced(){return _choiced;}
private:
	bool _choiced; // 选择中了
	bool _editable; // 是否可选
	CCSprite *_up; 
	CCSprite *_down;
	bool _touchIn;
	UIChoice()
	{
		_touchIn = false;
		_choiced = false;
		_editable = false;
		_up = false;
		_down = false;
		uiType = UIBase::UI_CHOICE;
	}
	CCPoint nowTouchPoint;
};

NS_CC_END