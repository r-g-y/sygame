#include "UIWindow.h"

NS_CC_BEGIN
UIWindow * UIWindow::window = NULL;
FunctionInfo exitFunction; // 退出函数
UIWindow * UIWindow::create()
{
	UIWindow *pRet = new UIWindow();
    if (pRet)
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}
void UIWindow::setCursor(CCSprite *cursor)
{
	if (_cursor)
	{
		this->removeChild(_cursor,true);
	}
	_cursor = cursor;
	if (cursor)
		this->addChild(cursor);
}
/**
 * 增加一个容器
 */
void UIWindow::addPanel(UIPanel *panel)
{
	if (!panel) return;
	insertBase(panel);
	this->addChild(panel);
}
/**
 * 增加ui
 */
void UIWindow::addUI(UIBase *base)
{
	for (BASES_ITER iter = _bases.begin() ; iter != _bases.end();++iter)
	{
		if (base == *iter)
		{
			return;
		}
	}
	insertBase(base);
	this->addChild(base);
}
/** 
 * 检查是否在区域里
 */
bool UIWindow::touchDown(float x,float y)
{
	nowCursorPoint = ccp(x,y);
	if (_nowTouchPanel && _nowTouchPanel->isModel())
	{
		_nowTouchPanel->touchDown(x,y);
		return true;
	}
	_nowTouchUI = NULL;
	for (BASES_ITER iter = _bases.begin() ; iter != _bases.end();++iter)
	{
		UIBase *base = *iter;
		if (base && base->touchDown(x,y))
		{
			_nowTouchUI = base;
			return true;
		}
	}
	_nowTouchPanel = NULL;
	PANELS temps = _panels;
	for (PANELS_ITER iter = temps.begin(); iter != temps.end(); ++iter)
	{
		UIPanel * panel = *iter;
		if (panel && panel->touchDown(x,y))
		{
			setNowTouchPanel(panel);
			return true;
		}
	}
	
	return false;
}
void UIWindow::release()
{
	if (window)
	{
		window->_bases.clear();
		window->_panels.clear();
		
		window->_nowTouchPanel = NULL;
		window->_nowTouchUI = NULL;
		window->removeAllChildrenWithCleanup(true);
		FunctionInfo& func = exitFunction;
		if (func.handle && func.object)
		{
			UIBase *o = func.object;
			
			o->release();
			func.object = NULL;
			(o->*func.handle)(NULL);
		}
	}
	window = NULL;
}

void UIWindow::setEndFunction(const FunctionInfo &function)
{
	function.object->retain();
	exitFunction = function;
	endFlag = true;
}
/**
 * 更新位置
 */
bool UIWindow::touchMove(float x,float y)
{
	nowCursorPoint = ccp(x,y);
	if (_cursor)
	{
		_cursor->setPosition(ccp(x,y));
	}
	if (_nowTouchPanel && _nowTouchPanel->isModel())
	{
		//return true;
	}
	if (_nowTouchPanel && _nowTouchPanel->touchMove(x,y))
		return true;
	if (_nowTouchUI)
	{
		if (_nowTouchUI->touchMove(x,y))
		{
			return true;
		}
	}
	PANELS temps = _panels;
	for (PANELS_ITER iter = temps.begin(); iter != temps.end(); ++iter)
	{
		UIPanel * panel = *iter;
		if (panel && panel->touchMove(x,y))
		{
			return true;
		}
	}
	return false;
}
/**
 * 停止拖动
 */
bool UIWindow::touchEnd(float x,float y)
{
	nowCursorPoint = ccp(x,y);
	bool tag = false;
	if (_nowTouchPanel && _nowTouchPanel->isModel())
	{
		_nowTouchPanel->touchEnd(x,y);
		if (endFlag)
		{
			FunctionInfo& func = exitFunction;
			if (func.handle && func.object)
			{
				UIBase *o = func.object;
				(o->*func.handle)(NULL);
				o->release();
				func.object = NULL;
			}
		}
		return false;
	}
	if (_nowTouchPanel)_nowTouchPanel->setZOrder(0);
	_nowTouchUI = NULL;
	BASES tbases = _bases;
	for (BASES_ITER iter = tbases.begin() ; iter != tbases.end();++iter)
	{
		UIBase *base = *iter;
		if (base && base->touchEnd(x,y))
		{
			tag = true;
		}
	}
	_nowTouchPanel = NULL;
	PANELS temps = _panels;
	for (PANELS_ITER iter = temps.begin(); iter != temps.end(); ++iter)
	{
		UIPanel * panel = *iter;
		if (panel && panel->touchEnd(x,y))
		{
			tag = true;
		}
	}
	
	if (endFlag)
	{
		FunctionInfo& func = exitFunction;
		if (func.handle && func.object)
		{
			UIBase *o = func.object;
			(o->*func.handle)(NULL);
			o->release();
			func.object = NULL;
		}
	}
	return tag;
}
UIWindow& UIWindow::getMe()
{
	if (!window) window= UIWindow::create();
	return *window;
}
void UIWindow::attachParent(CCNode *node)
{	
	node->addChild(window);
}
CCPoint UIWindow::getCursorPoint()
{
	return nowCursorPoint;
}
/**
 * 删除当前活动Panel
 */
void UIWindow::removeNowPanel()
{
	if (_nowTouchPanel)
	{
		PANELS temps = _panels;
		for (PANELS_ITER iter = temps.begin(); iter != temps.end(); ++iter)
		{
			if (_nowTouchPanel == *iter)
			{
				temps.erase(iter);
				break;
			}
		}
		this->removeChild(_nowTouchPanel,true);
	}
}

void  UIWindow::insertBase(UIBase *base)
{
	for (BASES_ITER iter = _bases.begin() ; iter != _bases.end();++iter)
	{
		UIBase *src = *iter;
		if (src && src->eventPriority <= base->eventPriority)
		{
			_bases.insert(iter,base);
			return;
		}
	}
	_bases.push_back(base);//push_back(base);
}
void UIWindow::showErr(const std::string& info)
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	if (!_errLbl)
	{
		_errLbl = CCLabelTTF::create(info.c_str(),"Arial",64);
		this->addChild(_errLbl,102);
	}
	if (_errLbl)
	{
		_errLbl->setPosition(ccp(visibleSize.width / 2,visibleSize.height / 2));
		_errLbl->runAction(CCMoveBy::create(1.5f,ccp(0,125)));
		_errLbl->runAction(CCFadeOut::create(1.5));
		_errLbl->setColor(ccc3(255,0,0));
	}
}
NS_CC_END