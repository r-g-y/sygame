#include "HeXieItem.h"
NS_CC_BEGIN
//////////////////////////// 道具 /////////////////////////////////////
HeXieItem*HeXieItem::create(CCTexture2D * texture,const CCRect &rect)
{
	HeXieItem *node = new HeXieItem();
	if (node && node->init(texture,rect))
	{
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}
bool HeXieItem::init(CCTexture2D * texture,const CCRect &rect)
{
	content = CCSprite::createWithTexture(texture,rect);
	if (content)
	{
		this->addChild(content);
		content->setAnchorPoint(ccp(0,0));
		return true;
	}
	return false;
}

bool HeXieItem::canMove(int bagId)
{
	return true;
}
void HeXieItem::setSize(float w,float h)
{
	content->setScaleX(w / content->getContentSize().width);
	content->setScaleY(h / content->getContentSize().height);
}

//////////////////////////// 包裹 /////////////////////////////////////
HeXieBag* HeXieBag::create(const CCSize &size,const char *pngName)
{
	HeXieBag *node = new HeXieBag();
	if (node && node->init(size,pngName))
	{
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}
bool HeXieBag::init(const CCSize &size,const char *pngName)
{
	this->_eachHeight = 64;
	this->_eachWidth = 64;
	this->_width = size.width;
	this->_height = size.height;
	_items.resize(this->_width* this->_height);
	_eachLeftSpan = 1;
	_eachUpSpan = 1;
	// 将图片打乱放置于BAG上
	CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->addImage(pngName);
	CCSize imgSize = pTexture->getContentSize();
	eachSize = CCSizeMake(imgSize.width / this->_width,imgSize.height / this->_height);
	for (int i = 0; i < size.width ;i++)
		for (int j = 0; j < size.height;j++)
		{
			HeXieItem*item = HeXieItem::create(pTexture,
				CCRectMake(i * eachSize.width,j * eachSize.height,eachSize.width,eachSize.height));
			int id =  (size.height - j -1) * size.width + i;
			this->setItem(item,id);
			item->positionId = id;
		}
	show();
	return true;
}
void HeXieBag::setPosition(const CCPoint &point)
{
	CCNode::setPosition(ccpAdd(point,ccp(this->_eachWidth/2,_eachHeight/2)));
}
bool HeXieBag::setItem(UIItem *item,int id)
{
	if (!item) return false;
	return UISuperBag::setItem(item,id);
}
/**
* 获取当前id 的像素位置
*/
CCPoint HeXieBag::getPixelPosition(int id)
{
	int x = id % ((int)_width);
	int y =  id / _width;
	CCPoint pos = ccp( x * (_eachWidth + _eachLeftSpan) + _eachLeftSpan,
	(y) * (_eachUpSpan + _eachHeight) + _eachUpSpan);
	return pos;
}
/**
* 通过bagid获取zpos
*/

CCPoint HeXieBag::getPixelPosition(int x,int y)
{
	CCPoint pos = ccp( x * (_eachWidth + _eachLeftSpan) + _eachLeftSpan,
	(y) * (_eachUpSpan + _eachHeight) + _eachUpSpan);
	return pos;
}

int HeXieBag::getNowTouchBagIdByCursorPosition(const CCPoint& point)
{
	CCPoint pos = this->convertToNodeSpace(point);
	if ( pos.x >= 0 && pos.y >= 0 && pos.x <= getViewWidth() && pos.y <= getViewHeight())
	{
		int dx = (pos.x) / (_eachWidth + _eachLeftSpan);
		int dy = (pos.y) / (_eachUpSpan + _eachHeight);

		return ( dy) * _width + dx;
	}
	return -1;
}
float HeXieBag::getViewWidth()
{
	return _width * (_eachWidth + _eachLeftSpan);
}
float HeXieBag::getViewHeight()
{
	return _height * (_eachUpSpan + _eachHeight) ;
}
/**
* 检查坐标是否在区域内
*/
bool HeXieBag::checkIn(int x,int y)
{
	if ( x >= 0 && y >= 0 && x <= getViewWidth() && y <= getViewHeight())
	{
		int dx = (x) / (_eachWidth + _eachLeftSpan);
		int dy = (y) / (_eachUpSpan + _eachHeight);
		int id = ( dy) * _width + dx;
		//if (id < _items.size() && _items.at(id))
			return true;
	}
	return false;
}

float HeXieBag::getPixelWidth()
{
	return (_eachWidth + _eachLeftSpan) * (_items.size() > _width?_width: _items.size());
}
float HeXieBag::getPixelHeight()
{ 
	return (_eachHeight + _eachUpSpan) * ((_items.size() / (int)_width ) +1);
}
NS_CC_END