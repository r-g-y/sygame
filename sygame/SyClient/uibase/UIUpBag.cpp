#include "UIUpBag.h"
NS_CC_BEGIN

/**
 * 展示
 */
void UIUpBag::show()
{
	this->setVisible(false);
	for (ITEMS_ITER iter = _items.begin(); iter != _items.end();++iter)
	{
		UIItem *i = *iter;
		if (i)
		{
			showItem(i);
		}
	}
	this->setVisible(true);
}
/**
 * 隐藏
 */
void UIUpBag::hide()
{
	this->setVisible(false);
}

void UIUpBag::execEachItem(stBagExecEach *exec)
{
	if (!exec) return;
	for (ITEMS_ITER iter = _items.begin(); iter != _items.end(); ++iter)
	{
		exec->exec(*iter);
	}
}
/**
 * 增加条目
 */
bool UIUpBag::addItem(UIItem *item)
{
	if (!item) return false;
	unsigned int index = 0;
	for (ITEMS_ITER iter = _items.begin(); iter != _items.end();++iter,++index)
	{
		UIItem *i = *iter;
		if (!i)
		{
			_items[index] = item;
			item->bagId = index;
			item->setSize(_eachWidth,_eachHeight);
			return true;
		}
	}
	if (_items.size() >= _bagWidth * _bagHeight )
	{
	//	return false;
	}
	item->bagId = _items.size();
	_items.push_back(item);
	item->setSize(_eachWidth,_eachHeight);
	item->setAnchorPoint(ccp(0,0));
	this->addChild(item);
	//item->show(CCRectMake(_viewx,_viewy,getViewWidth(),getViewHeight()));
	return true;
}

/** 
 * 检查是否在区域里
 */
bool UIUpBag::touchDown(float x,float y)
{
	if (!this->isVisible()) return false;
	CCPoint pos = ccp(x,y);
	pos = this->convertToNodeSpace(pos);
	startPoint = ccp(x,y);
	_touchIn = false;
	_nowTouchItemId = -1;
	nowTouchPoint = ccp(x,y);
	if ( checkIn(pos))
	{
		if (!_editable)
		{
			int dx = (pos.x - _viewx) / (_eachWidth + _eachLeftSpan);
			int dy = (pos.y - _viewy) / (_eachUpSpan + _eachHeight);

			_nowTouchItemId = dy * _bagWidth + dx;
		}
		_touchIn = true;
		return true;
	}
	return false;
}
/**
 * 更新位置
 */
bool UIUpBag::touchMove(float x,float y)
{
	if (!this->isVisible()) return false;
	if (!_touchIn) return false;
	if (!_exchange) return false;
	CCPoint pos = ccp(x,y);
	if (_editable)
	{
		CCPoint nowPoint = getPosition();
        setPosition(nowPoint.x + pos.x - nowTouchPoint.x,
                                      nowPoint.y + pos.y - nowTouchPoint.y);
		nowTouchPoint = pos;
	}
	else
	{
		if (_nowTouchItemId != -1)
		{
			// 移动当前选择的包裹物体
			if (_nowTouchItemId < _items.size() && _exchange)
			{
				UIItem * item = _items.at(_nowTouchItemId);
				if (item)
				{
					CCPoint nowPoint = item->getPosition();
					item->setPosition(nowPoint.x + pos.x - nowTouchPoint.x,
                                      nowPoint.y + pos.y - nowTouchPoint.y);
				}
			}
		}
	}
	nowTouchPoint = pos;
	return true;
}
/**
 * 停止拖动
 */
bool UIUpBag::touchEnd(float x,float y)
{
	if (!this->isVisible()) return false;
	CCPoint pos = ccp(x,y);
	float distance = fabs(y - startPoint.y);
	pos = this->convertToNodeSpace(pos);
	UIItem *srcItem = NULL;
	if (_nowTouchItemId < _items.size() && _nowTouchItemId >= 0)
	{
		srcItem = _items.at(_nowTouchItemId);
	}

	if (!srcItem){
		_nowTouchItemId = -1;
		_touchIn = false;
		return false;
	}
	if (!_editable)
	{
		nowTouchPoint = ccp(x,y);
		if (checkIn(pos))
		{
			if (!_editable)
			{
				int dx = (pos.x - _viewx) / (_eachWidth + _eachLeftSpan);
				int dy = (pos.y - _viewy) / (_eachUpSpan + _eachHeight);

				int nowTouchId = dy * _bagWidth + dx;
				if (nowTouchId >= _items.size() && nowTouchId < _bagWidth * _bagHeight)
				{
					_items.resize(_bagWidth * _bagHeight);
				}
				UIItem *destItem = NULL;
				if (nowTouchId < _items.size() && nowTouchId >= 0)
				{
					destItem = _items.at(nowTouchId);
				}
				if (srcItem == destItem && distance < (_eachUpSpan + _eachHeight))
					this->doEvent(UIBase::EVENT_CLICK_DOWN,this);
				if (!_exchange)
				{
					show();
					_nowTouchItemId = -1;
					_touchIn = false;
					return true;
				}
				if (!destItem && nowTouchId < _items.size())
				{
					_items[nowTouchId] = srcItem;
					_items[_nowTouchItemId] = NULL;
					srcItem->bagId = nowTouchId;
				}
				else if (destItem && nowTouchId < _items.size())
				{
					// 交换
					_items[nowTouchId] = srcItem;
					_items[_nowTouchItemId] = destItem;
					destItem->bagId = _nowTouchItemId;
					srcItem->bagId = nowTouchId;
				}
			}
			//return true;
		}
	}
	show();
	_nowTouchItemId = -1;
	_touchIn = false;
	return true;
}
bool UIUpBag::checkIn(const CCPoint &point)
{
	 if (point.x >= 0 && point.x <= x + getViewWidth() && point.y >= 0)
	 {
		return true;
	 }
	 return false;
}
/**
* 设置位置
*/
void UIUpBag::setPosition(float x,float y)
{
	CCNode::setPosition(ccp(x,y));
	this->x = x;
	this->y = y;
	//this->_viewx = x;
	//this->_viewy = y;
}
	
/**
* 设置大小
*/
void UIUpBag::setSize(float w,float h)
{
	_bagWidth = w;
	_bagHeight = h;
	this->w = w;
	this->h = h;
}
/**
 * 创建包裹
 */
UIUpBag *UIUpBag::create(int viewx,int viewy, int bagWidth,int bagHeight,int eachWidth,int eachHeight,int eachLeftSpan,int eachUpSpan)
{
	UIUpBag *bag = new UIUpBag();
	bag->_viewx= viewx;
	bag->_viewy = viewy;
	bag->_bagWidth = bagWidth;
	bag->_bagHeight = bagHeight;
	bag->_eachWidth = eachWidth;
	bag->_eachHeight = eachHeight;
	bag->_eachLeftSpan = eachLeftSpan;
	bag->_eachUpSpan = eachUpSpan;
	CCSprite *test = CCSprite::create("test.png");
	bag->addChild(test);
    if (bag)
    {
        bag->autorelease();
        return bag;
    }
    CC_SAFE_DELETE(bag);
    return NULL;
}

void UIUpBag::showItem(UIItem *item)
{
	if (!item) return;
	int bagId = item->bagId;
	if (bagId < 0) return;
	float x = bagId % _bagWidth;
	float y = bagId / _bagWidth;
	x = _viewx + x * (_eachWidth + _eachLeftSpan);
	y = _viewy + y * (_eachUpSpan + _eachHeight);
	item->setPosition(x + _eachLeftSpan,y +_eachUpSpan );
}
/**
 * 获取视图的宽
 */
int UIUpBag::getViewWidth()
{
	return _bagWidth * (_eachWidth + _eachLeftSpan);
}
/**
 * 获取视图的高
 */
int UIUpBag::getViewHeight()
{
	int bagHeight = _items.size() / _bagWidth ;
	return bagHeight * (_eachUpSpan + _eachHeight);
}
UIItem * UIUpBag::getNowChoiceItem()
{
	if (_nowTouchItemId < _items.size())
	{
		UIItem * item = _items.at(_nowTouchItemId);
		return item;
	}
	return NULL;
}
NS_CC_END