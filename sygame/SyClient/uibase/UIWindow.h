#pragma once
/**
 * 全局变量
 * 系统中承载dialog dialog限制嵌套
 */
#include "cocos2d.h"
#include "UIPanel.h"
NS_CC_BEGIN
class UIWindow:public CCNode{
public:
	static UIWindow& getMe();

	static UIWindow * create();
	/**
	 * 增加一个容器
	 */
	void addPanel(UIPanel *panel);

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

	void attachParent(CCNode *node);

	UIPanel * getNowTouchPanel(){return _nowTouchPanel;}
	/**
	 * 删除当前活动Panel
	 */
	void removeNowPanel();
	/**
	 * 设置鼠标图片
	 */
	void setCursor(CCSprite *sprite);
	/**
	 * 增加ui
	 */
	void addUI(UIBase *base);

	void setNowTouchPanel(UIPanel * pan)
	{
		if (_nowTouchPanel) _nowTouchPanel->setZOrder(0);
		_nowTouchPanel = pan;
		if (pan)
			pan->setZOrder(3);
	}
	static bool isClose()
	{
		return !window;
	}
	CCPoint getCursorPoint();
	static void release();

	void setEndFunction(const FunctionInfo &function);
	/**
	 * 展示当前出错信息
	 */
	void showErr(const std::string& info);
private:
	CCLabelTTF * _errLbl;
	void insertBase(UIBase *base);
	typedef std::list<UIPanel*> PANELS;
	PANELS _panels;
	typedef PANELS::iterator PANELS_ITER;
	typedef std::list<UIBase*> BASES;
	BASES _bases;
	typedef BASES::iterator BASES_ITER;
	UIPanel *_nowTouchPanel;
	CCSprite *_cursor; // 鼠标游标
	UIBase *_nowTouchUI;
	CCPoint nowCursorPoint;
	bool endFlag;
	UIWindow()
	{
		_errLbl = NULL;
		endFlag = false;
		_nowTouchUI = NULL;
		_cursor = NULL;
		_nowTouchPanel = NULL;
	}
	static UIWindow * window;
};
NS_CC_END
