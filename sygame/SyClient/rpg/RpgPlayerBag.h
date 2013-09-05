#pragma once
#include "cocos2d.h"
#include "UISuperBag.h"
#include "xmlScript.h"
#include "UIScrollView.h"
NS_CC_BEGIN
/**
 * 角色的包裹 存放召唤道具 和 装备类道具 药水瓶子
 */
class RpgItemInfo{
public:
	int itemId; // 道具编号
	std::string name; // 道具名字
	int itemType; // 道具类型
	int equipPos; // 在装备栏上的位置
	int buildId; // 对应的建筑的位置
	int functionType; // 对应的功能
	int level; // 品级
	std::string spriteName; // 精灵名字
	std::string desc; // 对应的描述
	int addSpeed; // 增加速度
	int addLife; // 增加生命
	int addHp; // 增加血量
	int addAtkValue; // 增加攻击值
	int addAtkValuePercent; // 增加攻击值的百分比
	int maxNum;
	enum{
		ITEM_TYPE_SUMMON, // 召唤
		ITEM_TYPE_EQUIP, // 装备
		ITEM_TYPE_HP, // 血瓶
		ITEM_TYPE_SKILL, // 技能
	};
	RpgItemInfo()
	{
		itemId = 0;
		itemType = ITEM_TYPE_EQUIP;
	}
	void takeNode(script::tixmlCodeNode *node);
};
class RpgItemDatas:public script::tixmlCode{
public:
	static RpgItemDatas * getMe()
	{
		static RpgItemDatas data;
		return &data;
	}
	void init();
	
	bool get(int itemId,RpgItemInfo &info);

	void takeNode(script::tixmlCodeNode *node);

	std::map<int,RpgItemInfo> infos;
};
#define theItemData RpgItemDatas::getMe()
/**
 * 包裹中的条目
 */
class RpgItem:public UIItem{
public:
	/**
	 * 创建条目 展示的是道具图标 点击后根据类型做不同的处理 包裹额定大小 6 * 6 每个条目 64 * 64
	 */
	static RpgItem*create(int dataid);
	bool init(int dataid);
	RpgItemInfo info;
	bool canMove(int bagId);
	virtual void setSize(float w,float h);
	CCSprite *content;
	int num;
	RpgItem()
	{
		num = 0;
		content = NULL;
		numberLbl = NULL;
	}
	CCLabelTTF *numberLbl;
	void showNum();
};

/**
 * 包裹
 **/
class RpgPlayerBag:public UISuperBag{
public:
	static RpgPlayerBag* create();
	bool init();
	int nowItemId;
	virtual bool setItem(UIItem *item,int id);
	virtual bool addItem(int itemId,int itemNum);
	/**
	 * 获取当前可视的宽
	 */
	float getPixelWidth();
	/**
	 * 获取当前可视的高
	 **/
	float getPixelHeight();
	
	/**
	 * 通过bagid获取zpos
	 */
	//zPos getZPosByBagId(int bagId);
	CCPoint getPixelPosition(int x,int y);
	const CCSize & getContentSize();
public:
	/**
	 * 获取当前id 的像素位置
	 */
	virtual CCPoint getPixelPosition(int id);

	virtual int getNowTouchBagIdByCursorPosition(const CCPoint& pos);
	/**
	 * 检查坐标是否在区域内
	 */
	virtual bool checkIn(int x,int y);

	float _width; // 宽
	float _height; // 高
	float _eachUpSpan; // 每个高 span
	float _eachLeftSpan; // 每个左span
	RpgPlayerBag()
	{
		_width = _height = 0;
		_eachUpSpan = _eachLeftSpan = 0;
		nowItemId = -1;
	}
	float getViewWidth();
	float getViewHeight();
};

/**
 * 商城系统 可以滚动的展示系统中出售的道具
 */
class RpgShop:public RpgPlayerBag,public script::tixmlCode{
public:
	/**
	 * 创建UI对象
	 */
	static RpgShop *create();
	bool init();
	const CCSize & getContentSize();
	void show();
	void hide();
	bool isVisible();
	UIBase *view;

	void takeNode(script::tixmlCodeNode *node);
};
/**
 * RPG 包裹
 */
class RpgXmlBag:public RpgPlayerBag,public script::tixmlCode{
public:
	/**
	 * 创建UI对象
	 */
	static RpgXmlBag *create(const char*bagName);
	static RpgXmlBag *create(script::tixmlCodeNode *node);
	bool initWithNode(script::tixmlCodeNode *node);
	bool init(const char*bagName);
	const CCSize & getContentSize();
	void show();
	void hide();
	bool isVisible();
	void addToParent(CCNode *node);
	void takeNode(script::tixmlCodeNode *node);
	bool doTouch(int touchType,const CCPoint &touchPoint);
	RpgXmlBag()
	{
		view = NULL;
	}
	UIBase *view;
};
NS_CC_END