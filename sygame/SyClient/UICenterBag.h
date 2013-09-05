#pragma once
#include "cocos2d.h"
NS_CC_BEGIN
/**
 * 事件处理器
 */
class UICenterItem;
class UIEvent:public CCObject{
public:
	virtual void doClick(UICenterItem *item) = 0;
};
template<class CHILD_EVENT>
class UIEventCallback:public UIEvent{
public:
	static CHILD_EVENT*create()
	{
		CHILD_EVENT *pRet = new CHILD_EVENT();
		pRet->autorelease();
		return pRet;
	}
};
/**
 * 收纳包裹的条目
 */
class UICenterItem:public CCSprite{
public:
	bool disable;
	bool up;
	UICenterItem *parent;
	/**
	 * 创建一个条目
	 */
	static UICenterItem * create(const char *upName,const char * downName,const CCSize & rect);
	bool init(const char *upName,const char * downName,const CCSize & rect);
	enum{
		BAG_ITEM, // 可以弹出子菜单
		FUNC_ITEM, // 具有功能的子菜单
	};
	UIEvent* eventHandle; // 事件集合
	void bind(UIEvent *eventHandle)
	{
		this->eventHandle = eventHandle;
		eventHandle->retain();
	}
	void setDisable(bool tag){
		this->disable = tag;
	}
	void setEventOn(){eventOn = true;}
	void setEventOff(){eventOn = false;}
	void setText(const std::string& textContent);

	CCSpriteFrame* downShow;
	CCSpriteFrame* upShow;
	int itemId;
	int itemType;
	int dirType;
	bool inTouch;
	bool eventOn;
	CCSize itemSize;
	CCPoint nowTouchPoint;
	bool canMove; // 标识是否可移动
	bool choiceAble; // 选择标识
	UICenterItem()
	{
		choiceAble = false;
		disable = false;
		up = true;
		canMove = false;
		dirType = -1;
		inTouch = false;
		downShow = NULL;
		upShow = NULL;
		itemType = FUNC_ITEM;
		parent = NULL;
		eventHandle = NULL;
		eventOn = true;
		valid = true;
	}
	~UICenterItem()
	{
		if (upShow) upShow->release();
		if (downShow) downShow->release();
		if (eventHandle) eventHandle->release();
	}

	/**
	 * 展示
	 */
	virtual void show();
	/**
	 * 展示
	 */
	virtual void showOnlyMe();
	/**
	 * 隐藏
	 */
	virtual void hide();
	enum{
		TOUCH_DOWN = 0,
		TOUCH_MOVE = 1,
		TOUCH_END = 2,
	};
	/**
	 * 检查是否命中
	 * \param touchPoint 点击点
	 * \return true 命中 false 非命中
	 */
	virtual bool checkIn(const CCPoint & touchPoint);
	/**
	 * 处理touch 事件
	 * \param touchType 点击类型
	 * \param touchPoint 点击的点
	 */
	virtual bool onTouch(int touchType,const CCPoint &touchPoint);
	/**
	 * 可以移动的操作
	 **/
	bool doTouchCanMove(int touchType,const CCPoint &touchPoint);
	/**
	 * 处理点击
	 */
	virtual void onClick();

	CCSpriteFrame * getFrameByFileName(const char *pngName);
	
	bool valid; // 是否有效
	void startProcess();
	void actionEnd_setValid(CCNode *pSender,void *arg);
};
/**
 * 包裹
 */
class UICenterBag:public UICenterItem{
public:
	static UICenterBag * create(const char *upName,const char * downName,const CCSize & rect);
	bool init(const char *upName,const char * downName,const CCSize & rect);
	std::vector<std::vector<UICenterItem*> > _items;
	typedef std::vector<UICenterItem*>::iterator DIR_ITEMS_ITER;
	typedef std::vector<std::vector<UICenterItem*> >::iterator ITEMS_ITER;
	UICenterItem *nowTouchItem;
	bool nowMove; // 标识当前是否移动条目
	bool nowItemMove; // 标识当前是否移动条目
	bool canExchange; // 是否可以交换
	bool canItemMove; // 标识当前是否可以移动条目
	CCTouch *nowTouch;
	enum{
		LEFT_SET = 0, // 左边设置
		RIGHT_SET = 1, // 右边设置
		DOWN_SET = 2, // 向下设置
		UP_SET = 3, // 向上设置
	};
	UICenterBag()
	{
		nowTouch = NULL;
		canMove = false;
		canExchange = false;
		canItemMove = false;
		nowItemMove = false;
		nowMove = false;
		nowTouchItem = NULL;
		itemType = BAG_ITEM;
		_items.resize(4);
		eachWidth = 64;
		eachHeight = 64;
		eachSpan = 1;
		openTag = false;
	}
	void setMovable(bool tag)
	{
		canMove = tag;
	}
	void setExchange()
	{
		canExchange = true;
		canItemMove = true;
	}
	/**
	 * 增加道具
	 * \param 方向
	 * \param item 条目
	 */
	void addItem(int dirType,UICenterItem *item);
	/**
	 * 展示
	 */
	void show();
	/**
	 * 隐藏
	 */
	void hide();
	bool openTag;
	bool isOpen(){return openTag;}
	void hideWithOutMe();
	int eachWidth; // 每个宽
	int eachHeight; // 每个高
	int eachSpan; // 每个格子的空隙

	/**
	 * 展示条目
	 * \param item 待展示的item
	 */
	virtual void showItem(int dirType,UICenterItem *item);
	/**
	 * 处理touch 事件
	 * \param touchType 点击类型
	 * \param touchPoint 点击的点
	 */
	virtual bool onTouch(int touchType,const CCPoint &touchPoint);
	virtual bool onTouch(int touchType,CCTouch* touch);
	/**
	 * 处理点击
	 */
	virtual void onClick();
	/**
	 * 检查是否命中
	 * \param touchPoint 点击点
	 * \return true 命中 false 非命中
	 */
	virtual bool checkIn(const CCPoint & touchPoint);
};

/**
 * 技能界面
 */
class UISkillBag:public UICenterBag{
public:
	static UISkillBag * create(const char *upName,const char * downName,const CCSize & rect);
	/**
	 * 展示条目
	 * \param item 待展示的item
	 */
	virtual void showItem(int dirType,UICenterItem *item);
};
NS_CC_END