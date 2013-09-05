#pragma once
#include "cocos2d.h"
#include "UILabel.h"
#include "UIBase.h"
NS_CC_BEGIN
/**
 * 通过滚动栏 设置数值
 */
class UILineValue:public UIBase{
public:
	/**
	 * 创建
	 */
	static UILineValue* create(const char *backname,const char *valuename,float minValue,float maxValue,float defaultValue);
	/**
	 * 初始化
	 */
	bool init(const char *backname,const char *valuename,float minValue,float maxValue,float defaultValue);
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
		//_editable = tag;
	}
	/**
	 * 设置位置
	 */
	virtual void setPosition(float x,float y);
	
	/**
	 * 设置大小
	 */
	virtual void setSize(float w,float h);

	float getNowValue(){return nowValue;}
protected:
	UILineValue()
	{
		minValue = 0;
		maxValue =100;
		nowValue = 0;
		_back = NULL;
		_span = NULL;
		width = height = 0;
		_touchIn = false;
		_info = NULL;
	}
	CCPoint nowTouchPoint;
	bool _touchIn;
	float width;
	float height;
	float minValue;
	float maxValue;
	float nowValue;
	CCSprite * _back; // 背景
	CCSprite *_span; // 标示块
	UILabel * _info;
	void update();
};

NS_CC_END