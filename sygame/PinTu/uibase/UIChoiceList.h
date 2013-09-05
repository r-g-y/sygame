#pragma once
#include "cocos2d.h"
#include "UIList.h"
NS_CC_BEGIN
/**
 * 实现下拉列表框
 */

class UIChoiceList:public UIList{
public:
	static UIChoiceList* create(const CCRect & viewRect,int eachWidth,int eachHeight);
	/**
	 * 可以使每个物体动态的展示出来
	 */
	void showEachDynamic();
	void showEach();
	/**
	 * 处理条目选中
	 **/
	 void doItemDown(UIItem *item);

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

	/**
	 * 设置位置
	 */
	virtual void setPosition(float x,float y);
	
	/**
	 * 设置大小
	 */
	virtual void setSize(float w,float h) ;

	virtual void setEditable(bool tag);
	int getNowChoiceId(){if (_nowChoiceItem) return _nowChoiceItem->bagId; return -1;}
	UIItem *getNowChoiceItem(){return _nowChoiceItem;}
	UIItem * getItemByBagId(int bagId);

	void setChoiceId(int id);
private:
	UIItem * _nowChoiceItem; // 当前选中的条目
	bool _touchIn;
	bool _editable;
	bool showTag;
	UIChoiceList()
	{
		showTag = true;
		_touchIn = false;
		_editable = false;
		_nowChoiceItem = NULL;
	}
};

NS_CC_END