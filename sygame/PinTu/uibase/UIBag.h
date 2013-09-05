#pragma once
#include <vector>
#include "cocos2d.h"
#include "UIItem.h"
#include "UIBase.h"
NS_CC_BEGIN
/**
 *固定大小的包裹空间 
 */

class UIBag :public UIBase{
public:
	/**
	 * 展示
	 */
	void show(bool dynamic = false);
	/**
	 * 隐藏
	 */
	void hide();
	/**
	 * 增加条目
	 */
	virtual bool addItem(UIItem *item);

	bool setItem(UIItem *item,int x,int y);
	/**
	 * 设置位置
	 */
	virtual void setPosition(float x,float y);
	bool setItem(UIItem *item,int bagId);
	/**
	 * 设置大小
	 */
	virtual void setSize(float w,float h);
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
	/**
	 * 动态设置条目的位置
	 */
	virtual bool setBagDynamicPos(UIItem * item,const CCPoint& sPos,const CCPoint& dPos);
	
	/**
	 * 删除条目
	 */
	virtual bool removeItem(UIItem *item,bool wait =true);
	/**
	 * 移动条目
	 */
	virtual bool moveItem(UIItem *item,const CCPoint& dPos,bool exchange = false);
	/**
	 * 设置可编辑
	 */
	void setEditable(bool tag){
		_editable = tag;
	}
	/**
	 * 创建包裹
	 */
	static	UIBag *create(int viewx,int viewy, int bagWidth,int bagHeight,int eachWidth,int eachHeight,int eachLeftSpan,int eachUpSpan);

	void execEachItem(stBagExecEach *exec);

	UIItem * getNowChoiceItem();

	int oldTouchItemId;

	UIItem * getItemById(int id)
	{
		if (id >=0 && id < _items.size()) return _items[id];
		return NULL;
	}
	int getWidth()
	{
		return _bagWidth;
	}
	int getHeight()
	{
		return _bagHeight;
	}
	UIItem * getItemByPos(int x,int y)
	{
		int id = x + y *_bagWidth;
		return getItemById(id);
	}
	CCPoint getPixelPosition(int x,int y);

	int getNowTouchBagIdByCursorPosition(const CCPoint& pos);

	void setBackgroud(const char *backpngName);
protected:
	CCSprite *back;
	bool _editable;
	int _nowTouchItemId;
	int _viewx; // 视图起始横坐标
	int _viewy; // 视图起始纵坐标
	int getViewWidth();
	int getViewHeight();
	int _bagWidth; // 包裹宽
	int _bagHeight; // 包裹高
	
	int _eachWidth; // 每个高
	int _eachHeight;  // 每个宽
	int _eachLeftSpan; // 每个左空隙
	int _eachUpSpan; // 每个上空隙
	std::vector<UIItem*> _items; // 每个道具列表
	typedef std::vector<UIItem*>::iterator ITEMS_ITER;
	std::set<UIItem*> _waitRemoveItems;
	bool _touchIn;
	UIBag()
	{
		back = NULL;
		oldTouchItemId = -1;
		_touchIn = false;
		_editable = false;
		_viewx = 0;
		_viewy = 0;
		_bagWidth = 0;
		_bagHeight = 0;
		_eachWidth = 0;
		_eachHeight = 0;
		_eachLeftSpan = 0;
		_eachUpSpan = 0;
		_nowTouchItemId = -1;
	}
	CCPoint nowTouchPoint; 
	virtual void showItem(UIItem *item,bool dynamic =false);
};
NS_CC_END