
#pragma once
#include <vector>
#include "cocos2d.h"
#include "UIItem.h"
#include "UIBase.h"
NS_CC_BEGIN
/**
 * 超级包裹 可以灵活的使用id-pos 的关系 默认是个横向发展的BAG
 */
class UISuperBag :public UIBase{
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

	virtual bool setItem(UIItem *item,int id);
	/**
	 * 设置位置
	 */
	virtual void setPosition(float x,float y);
	
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
	virtual bool removeItem(UIItem *item);
	/**
	 * 移动条目
	 */
	virtual bool moveItem(UIItem *item,int destId);
	/**
	 * 设置可编辑
	 */
	void setEditable(bool tag){
		_editable = tag;
	}
	/**
	 * 创建包裹
	 */
	static	UISuperBag *create(int viewx,int viewy,int eachWidth,int eachHeight);
	/**
	 * 执行包裹中的每个元素
	 */
	void execEachItem(stBagExecEach *exec);

	UIItem * getNowChoiceItem();

	int oldTouchItemId;

	UIItem * getItemById(int id)
	{
		if (id >= 0 && id < _items.size()) return _items[id];
		return NULL;
	}
// 子类实现函数
	/**
	 * 获取当前id 的像素位置
	 */
	virtual CCPoint getPixelPosition(int id);

	virtual int getNowTouchBagIdByCursorPosition(const CCPoint& pos);
	/**
	 * 检查坐标是否在区域内
	 */
	virtual bool checkIn(int x,int y);
	const CCSize & getContentSize()
	{
		return CCSizeMake(0,0);
	}
protected:
	bool _editable;
	int _nowTouchItemId;
	int _viewx; // 视图起始横坐标
	int _viewy; // 视图起始纵坐标
	
	int _eachWidth; // 每个高
	int _eachHeight;  // 每个宽

	std::vector<UIItem*> _items; // 每个道具列表
	typedef std::vector<UIItem*>::iterator ITEMS_ITER;

	bool _touchIn;
	UISuperBag()
	{
		oldTouchItemId = -1;
		_touchIn = false;
		_editable = false;
		_viewx = 0;
		_viewy = 0;
	
		_eachWidth = 0;
		_eachHeight = 0;
	
		_nowTouchItemId = -1;
	}
	CCPoint nowTouchPoint; 
	virtual void showItem(UIItem *item,bool dynamic =false);
};
/**
 * 圆形的包裹
 */
class UICircleBag:public UISuperBag{
public:
	/**
	 * 创建包裹
	 */
	static	UICircleBag *create(int viewx,int viewy,int eachWidth,int eachHeight,int minRadius,int maxRadius,int itemCount);

	/**
	 * 获取当前id 的像素位置 相对位置
	 */
	virtual CCPoint getPixelPosition(int id);
	/**
	 * 根据绝对位置寻找id
	 */
	virtual int getNowTouchBagIdByCursorPosition(const CCPoint& pos);
	/**
	 * 检查坐标是否在区域内 在中心区域 相对位置
	 */
	virtual bool checkIn(int x,int y);


	bool init(int,int,int);
protected:
	std::vector<CCPoint> positions; // 计算的是相对位置
	int minRadius;
	int maxRadius;
};
NS_CC_END