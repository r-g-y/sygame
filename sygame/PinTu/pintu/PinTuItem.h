#pragma once 
#include "cocos2d.h"
#include "UISuperBag.h"
#include "xmlScript.h"
#include "AboutData.h"
/**
 * 和谐的条目
 */
NS_CC_BEGIN
/**
 * 包裹中的条目
 */
class PinTuItem:public UIItem{
public:
	/**
	 * 创建条目 展示的是道具图标 点击后根据类型做不同的处理 包裹额定大小 6 * 6 每个条目 64 * 64
	 */
	static PinTuItem*create(CCTexture2D * texture,const CCRect &rect);
	bool init(CCTexture2D * texture,const CCRect &rect);
	
	bool canMove(int bagId);
	virtual void setSize(float w,float h);
	CCSprite *content;
	PinTuItem()
	{
		content = NULL;
		positionId = 0;
	}
	int positionId; // 位置信息
};

/**
 * 包裹 创建一个BAG 可以乱序图片 并承载
 **/
class PinTuBag:public UISuperBag{
public:
	static PinTuBag* create(const CCSize &size,const char *pngName);
	bool init(const CCSize &size,const char *pngName);
	int nowItemId;
	virtual bool setItem(UIItem *item,int id);
	
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
	CCSize eachSize;
	void setPosition(const CCPoint &point);
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
	PinTuBag()
	{
		_width = _height = 0;
		_eachUpSpan = _eachLeftSpan = 0;
		nowItemId = -1;
	}
	float getViewWidth();
	float getViewHeight();
};
NS_CC_END