#pragma once
#include <vector>
#include "cocos2d.h"
#include "UIItem.h"
#include "UIBase.h"
NS_CC_BEGIN
struct stExecListItem{
	virtual void exec(UIItem *item) = 0;
};
/**
 * List空间 容纳竖向的物体
 */
class UIList:public UIBase{
public:
	static UIList* create(const CCRect & viewRect,int eachWidth,int eachHeight);
	/**
	 * 静止的隐藏每个条目
	 */
	void hideEach();
	/**
	 * 展示每个条目
	 */
	void showEach();
	/**
	 * 动态的隐藏每个条目
	 */
	void hideEachDynamic();
	/**
	 * 可以使每个物体动态的展示出来
	 */
	void showEachDynamic();
	/**
	 * 设置位置
	 */
	virtual void setPosition(float x,float y);
	
	/**
	 * 设置大小
	 */
	virtual void setSize(float w,float h) {}
	/**
	 * 在尾巴增加一个条目
	 */
	bool pushItem(UIItem *item);
	/**
	 * 删除一个条目
	 */
	bool removeItem(UIItem *item);

	void setStartId(int id){
		startId = id;
	}
	/** 
	 * 检查是否在区域里
	 */
	virtual bool touchDown(float x,float y);
	/**
	 * 更新位置
	 */
	virtual bool touchMove(float x,float y);
	/**
	 * 停止拖动
	 */
	virtual bool touchEnd(float x,float y);
	
	void setEditable(bool tag){
		editable = tag;
	}
	void execEach(stExecListItem *exec);
	UIItem * getNowItem();
protected:
	/**
	 * 处理条目按下
	 */
	virtual void doItemDown(UIItem *item);
	int nowDragItemId; // 当前拖动的物体ID

	bool editable; // 是否在编辑状态

	CCPoint nowTouchPoint;
	/**
	 * 内容集合
	 */
	std::vector<UIItem*> _items;
	typedef std::vector<UIItem*>::iterator ITEMS_ITER;
	int _viewx; // 起始横坐标
	int _viewy; // 起始纵坐标
	int _viewWidth; // 视图宽
	int _viewHeight; // 视图高

	int _contentx; // 内容的初始横坐标
	int _contenty;  // 内容的初始纵坐标
	int _contentWidth; // 内容宽
	int _contentHeight; // 内容高

	int _leftItemSpan; // 左边边框的距离
	int _downItemSpan; // 每个边框的距离
	int _eachItemWidth; // 每个条目的宽
	int _eachItemHeight; // 每个条目的高
	/**
	 * 展示当前条目
	 */
	void showItem(UIItem *item);
	/**
	 * 获取横向能容纳的条目
	 */
	int getWidthItemCount();
	int getHeightItemCount();
	CCPoint getRightHidenPosition(int itemId);
	CCPoint getShowPosition(int itemId);
	CCPoint getShowMidPosition(int itemId);
	CCPoint getLeftHidenPosition();
	CCPoint getUpHidenPosition();
	void moveEnded();
	UIList()
	{
		_leftItemSpan = _downItemSpan = _eachItemWidth = _eachItemHeight = 0;
		nowId = startId = 0;
		touchIn = false;
		editable = false;
		uiType = UIBase::UI_LIST;
	}
	int nowId;
	int startId;
	bool touchIn;
};

NS_CC_END