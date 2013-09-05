#include "RpgPlayerBag.h"
NS_CC_BEGIN
//////////////////////////// 道具 /////////////////////////////////////
RpgItem*RpgItem::create(int dataid)
{
	RpgItem *node = new RpgItem();
	if (node && node->init(dataid))
	{
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}
bool RpgItem::init(int dataid)
{
	if (theItemData->get(dataid,info))
	{
		content = CCSprite::create(info.spriteName.c_str());
		this->addChild(content);
		content->setAnchorPoint(ccp(0,0));
		showNum();
		return true;
	}
	return false;
}

bool RpgItem::canMove(int bagId)
{
	return true;
	if (bagId == this->bagId) return true;
	return false;
}
void RpgItem::setSize(float w,float h)
{
	content->setScaleX(w / content->getContentSize().width);
	content->setScaleY(h / content->getContentSize().height);
}
void RpgItem::showNum()
{
	std::stringstream ss;
	ss<< num;
	if (!numberLbl)
	{
		numberLbl = CCLabelTTF::create(ss.str().c_str(),"Arial",12);
		numberLbl->setPositionX(64);
		this->addChild(numberLbl);
	}
	numberLbl->setString(ss.str().c_str());
}
////////////////////////////道具数据//////////////////////////////////
void RpgItemInfo::takeNode(script::tixmlCodeNode *node)
{
	/**
	int itemId; // 道具编号
	std::string name; // 道具名字
	int itemType; // 道具类型
	int itemValue; // 道具值
	int equipPos; // 在装备栏上的位置
	int buildId; // 对应的建筑的位置
	int functionType; // 对应的功能
	int level; // 品级
	std::string spriteName; // 精灵名字
	*/
	name = node->getAttr("name");
	node->getAttr("id",itemId);
	node->getAttr("type",itemType);
	node->getAttr("quickpos",equipPos);
	node->getAttr("buildid",buildId);
	node->getAttr("level",level);
	desc = node->getAttr("desc");
	spriteName = node->getAttr("spritename");
}

////////////////////////////道具管理//////////////////////////////////
void RpgItemDatas::init()
{
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData("itemdata.xml","rb",&nSize);
	if (!nSize)return ;
	script::tixmlCode::initFromString((char*) buffer);
}

void RpgItemDatas::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
		script::tixmlCodeNode itemDataNode = node->getFirstChildNode("itemdata");
		while(itemDataNode.isValid())
		{
			RpgItemInfo info;
			info.takeNode(&itemDataNode);
			infos[info.itemId] = info;
			itemDataNode = itemDataNode.getNextNode("itemdata");
		}
	}
}
bool RpgItemDatas::get(int itemId,RpgItemInfo &info)
{
	std::map<int,RpgItemInfo>::iterator iter = infos.find(itemId);
	if (iter != infos.end())
	{
		info = iter->second;
		return true;
	}
	return false;
}
//////////////////////////// 包裹 /////////////////////////////////////
RpgPlayerBag* RpgPlayerBag::create()
{
	RpgPlayerBag *node = new RpgPlayerBag();
	if (node && node->init())
	{
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}
bool RpgPlayerBag::init()
{
	this->_eachHeight = 64;
	this->_eachWidth = 64;
	this->_width = 4;
	this->_height = 4;
	_items.resize(this->_width* this->_height);
	_eachLeftSpan = 1;
	_eachUpSpan = 1;
	CCSprite *bagBack = CCSprite::create("bagback.pngx");
	if (bagBack)
	{
		this->addChild(bagBack);
		bagBack->setAnchorPoint(ccp(0,0));
		bagBack->setScaleX((65*4) / bagBack->getContentSize().width);
		bagBack->setScaleY((65*4)/ bagBack->getContentSize().height);
	}
	for (int i = 0;i < 16;i++)
	{
		CCSprite *bagItem = CCSprite::create("itemback.pngx");
        if (!bagItem) continue;
		bagItem->setAnchorPoint(ccp(0,0));
		bagItem->setPosition(this->getPixelPosition(i));
		bagItem->setScaleX(this->_eachWidth / bagItem->getContentSize().width);
		bagItem->setScaleY(this->_eachHeight / bagItem->getContentSize().height);
		this->addChild(bagItem);
	}
	return true;
}
bool RpgPlayerBag::setItem(UIItem *item,int id)
{
	if (!item) return false;
//	int x = id % (int)_width ;
//	int y = id / (int)_height;
//	id =  (this->_height - y -1) * _width + x;
	return UISuperBag::setItem(item,id);
}
bool RpgPlayerBag::addItem(int itemId,int itemNum)
{
	int index = 0;
	for (ITEMS_ITER iter = _items.begin(); iter != _items.end();++iter,++index)
	{
		RpgItem* item = (RpgItem*)(*iter);
		if (!item || item->info.itemId == itemId)
		{
			if (!item)
			{
				item = RpgItem::create(itemId);
				this->addChild(item);
				_items[index] = item;
			}
			item->num += itemNum;
			item->bagId = index;
			item->setSize(_eachWidth,_eachHeight);
			item->showNum();
			if (this->isVisible())
				show();
			return true;
		}
	}
	return false;
}
/**
* 获取当前id 的像素位置
*/
CCPoint RpgPlayerBag::getPixelPosition(int id)
{
	int x = id % ((int)_width);
	int y = id / _width;
	return getPixelPosition(x,y);
}
/**
* 通过bagid获取zpos
*/
#if (0)
zPos RpgPlayerBag::getZPosByBagId(int bagId)
{
	int x = bagId % ((int)_width);
	int y =  bagId / _width;
	return zPos(x,y);
}
#endif
CCPoint RpgPlayerBag::getPixelPosition(int x,int y)
{
	CCPoint pos = ccp( x * (_eachWidth + _eachLeftSpan) + _eachLeftSpan,
	(_height - y - 1) * (_eachUpSpan + _eachHeight) + _eachUpSpan);
	return pos;
}
const CCSize & RpgPlayerBag::getContentSize()
{
	return CCSizeMake(4 * 65,4*65);
}
int RpgPlayerBag::getNowTouchBagIdByCursorPosition(const CCPoint& point)
{
	CCPoint pos = this->convertToNodeSpace(point);
	if ( pos.x >= 0 && pos.y >= 0 && pos.x <= getViewWidth() && pos.y <= getViewHeight())
	{
		int dx = (pos.x) / (_eachWidth + _eachLeftSpan);
		int dy = (pos.y) / (_eachUpSpan + _eachHeight);

		return (_height - dy -1) * _width + dx;
	}
	return -1;
}
float RpgPlayerBag::getViewWidth()
{
	return _width * (_eachWidth + _eachLeftSpan);
}
float RpgPlayerBag::getViewHeight()
{
	return _height * (_eachUpSpan + _eachHeight) ;
}
/**
* 检查坐标是否在区域内
*/
bool RpgPlayerBag::checkIn(int x,int y)
{
	if ( x >= 0 && y >= 0 && x <= getViewWidth() && y <= getViewHeight())
	{
		int dx = (x) / (_eachWidth + _eachLeftSpan);
		int dy = (y) / (_eachUpSpan + _eachHeight);
		int id = (_height - dy) * _width + dx;
		//if (id < _items.size() && _items.at(id))
			return true;
	}
	return false;
}

float RpgPlayerBag::getPixelWidth()
{
	return (_eachWidth + _eachLeftSpan) * (_items.size() > _width?_width: _items.size());
}
float RpgPlayerBag::getPixelHeight()
{ 
	return (_eachHeight + _eachUpSpan) * ((_items.size() / (int)_width ) +1);
}
RpgShop *RpgShop::create()
{
	RpgShop *node = new RpgShop();
	if (node && node->init())
	{
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}
bool RpgShop::init()
{
	CCSprite *back = CCSprite::create("info.png");
	if (back)
	{
		this->addChild(back);
	}
	this->_eachHeight = 64;
	this->_eachWidth = 64;
	this->_width = 4;
	this->_height = 4;
	_items.resize(this->_width* this->_height);
	_eachLeftSpan = 1;
	_eachUpSpan = 1;
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData("localshop.xml","rb",&nSize);
	if (!nSize)return false;
	script::tixmlCode::initFromString((char*) buffer);
	show();
	
	return true;
}
void RpgShop::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
		script::tixmlCodeNode viewNode = node->getFirstChildNode("face");
		if (viewNode.isValid())
		{
			float viewx = viewNode.getFloat("viewx");
			float viewy = viewNode.getFloat("viewy");
			float vieww = viewNode.getFloat("vieww");
			float viewh = viewNode.getFloat("viewh");
			view = UIScrollView::create(0,0,vieww,viewh);
			if (view)
			{
				((UIScrollView*)view)->addContent(this);
				view->setEditable(false);
			}
		}
		//view = this;
		script::tixmlCodeNode itemNode = node->getFirstChildNode("item");
		int count = 0;
		while(itemNode.isValid())
		{
			int itemid = itemNode.getInt("id");
			RpgItem *item = RpgItem::create(itemid);
			if (item)
			{
				this->setItem(item,count++);
			}
			itemNode = itemNode.getNextNode("item");
		}
        this->show();
	}
}

const CCSize &  RpgShop::getContentSize()
{
	return CCSizeMake(this->_eachWidth * this->_width,this->_eachHeight * this->_height);
}
void  RpgShop::show()
{
    RpgPlayerBag::show();
	if (view)
	{
		view->setVisible(true);
	}
}
void  RpgShop::hide()
{
	if (view)
	{
		view->setVisible(false);
	}
}
bool  RpgShop::isVisible()
{
	if (view)
	{
		return view->isVisible();
	}
	return false;
}
/**
* 创建UI对象
*/
 RpgXmlBag *RpgXmlBag::create(const char*bagName)
 {
	RpgXmlBag *node = new RpgXmlBag();
	if (node && node->init(bagName))
	{
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
 }
RpgXmlBag *RpgXmlBag::create(script::tixmlCodeNode *snode)
{
	RpgXmlBag *node = new RpgXmlBag();
	if (node && node->initWithNode(snode))
	{
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}

bool  RpgXmlBag::doTouch(int touchType,const CCPoint &touchPoint)
{
	if (view)
	{
		return view->doTouch(touchType,touchPoint);
	}
	else
	{
		return this->doTouch(touchType,touchPoint);
	}
	return false;
}
bool RpgXmlBag::initWithNode(script::tixmlCodeNode *node)
{
	if (node->equal("bag"))
	{
		CCSprite *back = CCSprite::create(node->getAttr("back"));
		if (back)
		{
			this->addChild(back);
		}
		this->_eachHeight = node->getInt("eachheight");
		this->_eachWidth = node->getInt("eachwidth");
		this->_width = node->getInt("width");
		this->_height = node->getInt("height");
		_items.resize(this->_width* this->_height);
		_eachLeftSpan =  node->getInt("leftspan");
		_eachUpSpan =  node->getInt("upspan");
		script::tixmlCodeNode viewNode = node->getFirstChildNode("face");
		if (viewNode.isValid())
		{
			float viewx = viewNode.getFloat("viewx");
			float viewy = viewNode.getFloat("viewy");
			float vieww = viewNode.getFloat("vieww");
			float viewh = viewNode.getFloat("viewh");
			view = UIScrollView::create(0,0,vieww,viewh);
			if (view)
			{
				((UIScrollView*)view)->addContent(this);
				view->setEditable(false);
			}
		}
		script::tixmlCodeNode itemNode = node->getFirstChildNode("item");
		int count = 0;
		while(itemNode.isValid())
		{
			int itemid = itemNode.getInt("id");
			RpgItem *item = RpgItem::create(itemid);
			if (item)
			{
				this->setItem(item,count++);
			}
			itemNode = itemNode.getNextNode("item");
		}
	}
	show();
	return true;
}
bool RpgXmlBag::init(const char*bagName)
{
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData("localshop.xml","rb",&nSize);
	if (!nSize)return false;
	script::tixmlCode::initFromString((char*) buffer);
	show();
	return true;
}
const CCSize & RpgXmlBag::getContentSize()
{
	return CCSizeMake(this->_eachWidth * this->_width,this->_eachHeight * this->_height);
}
void RpgXmlBag::show()
{
	RpgPlayerBag::show();
	if (view)
	{
		view->setVisible(true);
	}
}
void RpgXmlBag::hide()
{
	if (view)
	{
		view->setVisible(false);
	}
}
bool RpgXmlBag::isVisible()
{
	if (view)
	{
		return view->isVisible();
	}
	return false;
}
void RpgXmlBag::addToParent(CCNode *node)
{
	if (view)
	{
		node->addChild(view);
	}
	else
	{
		node->addChild(this);
	}
}
void RpgXmlBag::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
        script::tixmlCodeNode bagNode = node->getFirstChildNode("bag");
		initWithNode(&bagNode);
	}
}
NS_CC_END