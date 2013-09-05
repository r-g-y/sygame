#include "UIBag.h"
NS_CC_BEGIN

/**
 * 展示
 */
void UIBag::show(bool dynamic)
{
	this->setVisible(false);
	for (ITEMS_ITER iter = _items.begin(); iter != _items.end();++iter)
	{
		UIItem *i = *iter;
		if (i)
		{
			showItem(i,dynamic);
		}
	}
	this->setVisible(true);
}
/**
 * 隐藏
 */
void UIBag::hide()
{
	this->setVisible(false);
}

void UIBag::execEachItem(stBagExecEach *exec)
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
bool UIBag::addItem(UIItem *item)
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
		return false;
	}
	item->_bag = this;
	item->bagId = _items.size();
	_items.push_back(item);
	item->setSize(_eachWidth,_eachHeight);
	this->addChild(item);
	return true;
}
CCPoint UIBag::getPixelPosition(int x,int y)
{
	CCPoint pos = ccp( _viewx + x * (_eachWidth + _eachLeftSpan) + _eachLeftSpan,
	 _viewy + getViewHeight() - (y + 1) * (_eachUpSpan + _eachHeight) + _eachUpSpan);
	return pos;
}
bool UIBag::setItem(UIItem *item,int x,int y)
{
	if (!item) return false; 
	item->setSize(_eachWidth,_eachHeight);
	int bagId = x + y * _bagWidth;
	if (bagId >= _items.size())
	{
		_items.resize(bagId + 1);
	}
	item->_bag = this;
	item->bagId = bagId;
	_items[bagId] = item;
	this->addChild(item);
	return true;
}

bool UIBag::setItem(UIItem *item,int bagId)
{
	if (!item) return false; 
	item->setSize(_eachWidth,_eachHeight);
	if (bagId >= _items.size())
	{
		_items.resize(bagId + 1);
	}
	item->_bag = this;
	item->bagId = bagId;
	_items[bagId] = item;
	this->addChild(item);
	return true;
}
/**
* 动态设置条目的位置
*/
bool UIBag::setBagDynamicPos(UIItem * item,const CCPoint& sPos,const CCPoint& dPos)
{
	if (!item) return false;
	CCPoint srcPos = ccp( _viewx + sPos.x * (_eachWidth + _eachLeftSpan) + _eachLeftSpan,
	 _viewy + getViewHeight() - (sPos.y + 1) * (_eachUpSpan + _eachHeight) + _eachUpSpan);

	CCPoint destPos = ccp( _viewx + dPos.x * (_eachWidth + _eachLeftSpan) + _eachLeftSpan,
	 _viewy + getViewHeight() - (dPos.y + 1) * (_eachUpSpan + _eachHeight) + _eachUpSpan);
	item->setPosition(srcPos.x,srcPos.y);
	if (!item->getParent())
	{
		return false;
		//this->addChild(item);
	}
	CCMoveTo *moveAction = CCMoveTo::create(0.5f,destPos);
	item->runAction(moveAction);
	return true;
}

/**
* 删除条目
*/
bool UIBag::removeItem(UIItem *item,bool wait)
{
	if (!item) return false;
	if (item->bagId < _items.size())
	{
		_items[item->bagId] = NULL;
		if (wait)
			_waitRemoveItems.insert(item);
		else
			this->removeChild(item,true);
		return true;
	}
	return false;
}
/**
* 移动条目
*/
bool UIBag::moveItem(UIItem *item,const CCPoint& dPos,bool exchange)
{
	if (!item) return false;
	UIItem *srcItem = item;
	CCPoint destPos = dPos;
	CCPoint srcPos = ccp(item->bagId % _bagWidth,item->bagId / _bagWidth);
	int did = dPos.y * _bagWidth + dPos.x;
	UIItem *destItem = NULL;
	if (did < _items.size())
	{
		destItem = _items[did];
	}
	if (!destItem && did < _items.size() && srcItem->canMove(did))
	{
		_items[did] = srcItem;
		_items[srcItem->bagId] = NULL;
		srcItem->bagId = did;
	}
	else if (destItem && did < _items.size() && exchange)
	{
		// 交换
		if (destItem && srcItem && destItem->canMove(srcItem->bagId) && srcItem->canMove(destItem->bagId))
		{
			_items[did] = srcItem;
			_items[srcItem->bagId] = destItem;
			destItem->bagId = srcItem->bagId;
			srcItem->bagId = did;
		}
	}
	//show();
	//if (srcItem)
	//	setBagDynamicPos(srcItem,srcPos,destPos);
	//if (destItem && exchange)
	//	setBagDynamicPos(destItem,destPos,srcPos);
	return true;
}
/** 
 * 检查是否在区域里
 */
bool UIBag::touchDown(float x,float y)
{
	if (!this->isVisible()) return false;
	CCPoint pos = ccp(x,y);
	pos = this->convertToNodeSpace(pos);
	_touchIn = false;
	_nowTouchItemId = -1;
	nowTouchPoint = ccp(x,y);
	if ( pos.x >= _viewx && pos.y >= _viewy && pos.x <= _viewx + getViewWidth() && pos.y <= _viewy + getViewHeight())
	{
		if (!_editable)
		{
			int dx = (pos.x - _viewx) / (_eachWidth + _eachLeftSpan);
			int dy = (_viewy +getViewHeight() -  pos.y) / (_eachUpSpan + _eachHeight);

			_nowTouchItemId = dy * _bagWidth + dx;
			if (_nowTouchItemId < _items.size())
			{
				UIItem * item = _items.at(_nowTouchItemId);
				if (!item || !item->canMove(_nowTouchItemId))
				{
					_nowTouchItemId = -1;
					return false;
				}
				if (item)
				{
					item->setZOrder(2);
				}
			}
		}
		_touchIn = true;
		return true;
	}
	return false;
}

int UIBag::getNowTouchBagIdByCursorPosition(const CCPoint& point)
{
	CCPoint pos = this->convertToNodeSpace(point);
	if ( pos.x >= _viewx && pos.y >= _viewy && pos.x <= _viewx + getViewWidth() && pos.y <= _viewy + getViewHeight())
	{
		int dx = (pos.x - _viewx) / (_eachWidth + _eachLeftSpan);
		int dy = (_viewy +getViewHeight() -  pos.y) / (_eachUpSpan + _eachHeight);

		return dy * _bagWidth + dx;
	}
	return -1;
}
/**
 * 更新位置
 */
bool UIBag::touchMove(float x,float y)
{
	if (!this->isVisible()) return false;
	if (!_touchIn) return false;
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
			if (_nowTouchItemId < _items.size())
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
UIItem * UIBag::getNowChoiceItem()
{
	UIItem *srcItem = NULL;
	if (oldTouchItemId < _items.size() && oldTouchItemId >= 0)
	{
		srcItem = _items.at(oldTouchItemId);
		return srcItem;
	}
	
	return NULL;
}
/**
 * 停止拖动
 */
bool UIBag::touchEnd(float x,float y)
{
	if (!this->isVisible()) return false;
	CCPoint pos = ccp(x,y);
	pos = this->convertToNodeSpace(pos);
	UIItem *srcItem = NULL;
	if (_nowTouchItemId < _items.size() && _nowTouchItemId >= 0)
	{
		srcItem = _items.at(_nowTouchItemId);
		oldTouchItemId = _nowTouchItemId;
	}
	if (!srcItem){
		_nowTouchItemId = -1;
		_touchIn = false;
		return false;
	}
	if (srcItem) srcItem->setZOrder(0);
	if (!_editable)
	{
		nowTouchPoint = ccp(x,y);
		if ( pos.x >= _viewx && pos.y >= _viewy && pos.x <= _viewx + getViewWidth() && pos.y <= _viewy + getViewHeight())
		{
			if (!_editable)
			{
				int dx = (pos.x - _viewx) / (_eachWidth + _eachLeftSpan);
				int dy = (_viewy +getViewHeight() -  pos.y) / (_eachUpSpan + _eachHeight);

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
				if (srcItem == destItem)
				{
					this->doEvent(UIBase::EVENT_CLICK_DOWN,this);
				}
				
				{
					if (!destItem && nowTouchId < _items.size() && srcItem->canMove(nowTouchId))
					{
						_items[nowTouchId] = srcItem;
						_items[_nowTouchItemId] = NULL;
						srcItem->bagId = nowTouchId;
					}
					else if (destItem && nowTouchId < _items.size())
					{
						// 交换
						if (destItem && srcItem && srcItem->canMove(destItem->bagId))
						{
							_items[nowTouchId] = srcItem;
							_items[_nowTouchItemId] = destItem;
							destItem->bagId = _nowTouchItemId;
							srcItem->bagId = nowTouchId;
						}
					}
					else if (!destItem && (nowTouchId < 0 || nowTouchId >= _items.size()))
					{
						this->doEvent(UIBase::EVENT_DRAG_OUT,this); // 拖出包裹
					}
				}
			}
			
			//return true;
		}
		else
				this->doEvent(UIBase::EVENT_DRAG_OUT,this); // 拖出包裹
	}
	
	show();
	_nowTouchItemId = -1;
	_touchIn = false;
	for (std::set<UIItem*>::iterator iter = _waitRemoveItems.begin(); iter !=  _waitRemoveItems.end();++iter)
	{
		UIItem *item = *iter;
		if (item)
		{
			item->removeAllChildrenWithCleanup(true);
			this->removeChild(item,true);
			int index = 0;
			for (ITEMS_ITER pos = _items.begin(); pos != _items.end();++pos,++index)
			{
				if (*pos == item)
				{				
					_items[index] = NULL;
				}
			}
		}
	}
	_waitRemoveItems.clear();
	return true;
}
	
/**
* 设置位置
*/
void UIBag::setPosition(float x,float y)
{
	CCNode::setPosition(ccp(x,y));
	this->x = x;
	this->y = y;
}
	
/**
* 设置大小
*/
void UIBag::setSize(float w,float h)
{
	_bagWidth = w;
	_bagHeight = h;
	this->w = w;
	this->h = h;
}
void UIBag::setBackgroud(const char *backpngName)
{
	if (back)
	{
		back->initWithFile(backpngName);
	}
	else
	{
		back = CCSprite::create(backpngName);
		if (back)
		{
			this->addChild(back,-1);
			back->setAnchorPoint(ccp(0,0));
		}
	}
	if(back)
	{
//		back->setScaleX(this->getViewWidth()/ back->getContentSize().width);
//		back->setScaleY(this->getViewHeight()/ back->getContentSize().height);
	}
}
/**
 * 创建包裹
 */
UIBag *UIBag::create(int viewx,int viewy, int bagWidth,int bagHeight,int eachWidth,int eachHeight,int eachLeftSpan,int eachUpSpan)
{
	UIBag *bag = new UIBag();
	bag->_viewx= viewx;
	bag->_viewy = viewy;
	bag->_bagWidth = bagWidth;
	bag->_bagHeight = bagHeight;
	bag->_eachWidth = eachWidth;
	bag->_eachHeight = eachHeight;
	bag->_eachLeftSpan = eachLeftSpan;
	bag->_eachUpSpan = eachUpSpan;
    if (bag)
    {
        bag->autorelease();
        return bag;
    }
    CC_SAFE_DELETE(bag);
    return NULL;
}

void UIBag::showItem(UIItem *item,bool dynamic)
{
	if (!item) return;
	int bagId = item->bagId;
	if (bagId < 0) return;
	float x = bagId % _bagWidth;
	float y = bagId / _bagWidth;
	if (dynamic)
	{
		setBagDynamicPos(item,ccp(x,0),ccp(x,y));
	}
	else
	{
		x = _viewx + x * (_eachWidth + _eachLeftSpan);
		y = _viewy + getViewHeight() - (y + 1) * (_eachUpSpan + _eachHeight);
		
		item->setPosition(x + _eachLeftSpan ,y + _eachUpSpan);
	}
}
/**
 * 获取视图的宽
 */
int UIBag::getViewWidth()
{
	return _bagWidth * (_eachWidth + _eachLeftSpan);
}
/**
 * 获取视图的高
 */
int UIBag::getViewHeight()
{
	return _bagHeight * (_eachUpSpan + _eachHeight);
}

NS_CC_END