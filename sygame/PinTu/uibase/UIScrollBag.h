#pragma once
#include "cocos2d.h"
#include "UIItem.h"
#include <vector>
#include "UIUpBag.h"
#include "UIScrollView.h"
NS_CC_BEGIN
/**
 * 实现上下 滚动的Bag
 */

class UIScrollBag:public UIScrollView{
public:
	/**
	 * 创建包裹
	 */
	static	UIScrollBag *create(int bagWidth,int bagHeight,int eachWidth,int eachHeight,int eachLeftSpan,int eachUpSpan);
	bool init(int bagWidth,int bagHeight,int eachWidth,int eachHeight,int eachLeftSpan,int eachUpSpan);
	/**
	 * 增加条目
	 */
	bool addItem(UIItem *item);
	/**
	 * 展示
	 */
	void showEach();

	/**
	 * 隐藏
	 */
	void hideEach();

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

	UIItem * getNowChoiceItem();
	bool bind(int uiEvent,const FunctionInfo& function);
protected:
	UIScrollBag()
	{
		slider = NULL;
		bag = NULL;
	}
	CCSprite * slider; // 滑块 根据offset 来更改当前位置
	void updateSlider();
	UIUpBag *bag;
};

NS_CC_END