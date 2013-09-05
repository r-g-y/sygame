#include "UIPanel.h"
#include "UIBase.h"
#include "UIButton.h"
#include "UILabel.h"
#include "UIImage.h"
#include "UIEditField.h"
#include "UIChoiceList.h"
#include "UIBag.h"
#include "UIChoice.h"
#include "UILineValue.h"
#include "UIWindow.h"
#include "UIXmlBag.h"
//#include "TFActionManager.h"
NS_CC_BEGIN

UIPanel * UIPanel::createFromNode(script::tixmlCodeNode *snode)
{
	UIPanel *node = new UIPanel();
	if (node && node->initXFromNode(snode))
	{
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}

UIBase * UIPanel::createButton(const CCPoint &position,const CCSize &size,const char * upImgName,const char *downImgName,UIPanel *parent ,int uniqueId)
{
	UIButton * btn = UIButton::create(upImgName,downImgName);
	if (btn)
	{
		btn->uniqueId = uniqueId;
		btn->setPosition(position.x,position.y);
		btn->setSize(size.width,size.height);
		if (parent)
		{
			parent->addUI(btn);
			parent->addGloabUI(btn);
		}
	}
	return btn;
}
UIBase * UIPanel::createButton(const CCPoint &position,const CCSize &size,const char* buttonName,UIPanel *parent ,int uniqueId)
{
	UITextButton * btn = UITextButton::create(buttonName);
	if (btn)
	{
		btn->uniqueId = uniqueId;
		btn->setPosition(position.x,position.y);
		btn->setSize(size.width,size.height);
		if (parent)
		{
			parent->addUI(btn);
			parent->addGloabUI(btn);
		}
		
	}
	return btn;
}

/**
* 创建label
**/
UIBase *UIPanel::createLabel(const CCPoint &position,const CCSize &size,const char *content,float fontSize,UIPanel *parent,int uniqueId)
{
	UILabel *label = UILabel::create(content,fontSize);
	if (label)
	{
		label->setPosition(position.x,position.y);
		label->setSize(size.width,size.height);
		label->uniqueId = uniqueId;
		if (parent)
		{
			parent->addUI(label);
			parent->addGloabUI(label);
		}

	}
	return label;
}
UIBase *UIPanel::createLineValue(const CCPoint &position,float minValue,float maxValue,float defaultValue,UIPanel * parent,int uniqueId)
{
	UILineValue *lineValue = UILineValue::create("","",minValue,maxValue,defaultValue);
	if (lineValue)
	{
		lineValue->setPosition(position.x,position.y);
		lineValue->uniqueId = uniqueId;
		if (parent)
		{
			parent->addUI(lineValue);
			parent->addGloabUI(lineValue);
		}

	}
	return lineValue;
}
/**
* 创建textfield
*/
UIBase *UIPanel::createTextField(const CCPoint& position,const CCSize &size,const char* defaultContent,UIPanel *parent ,int uniqueId)
{
	UIEditField *field = UIEditField::create(position,size,defaultContent);
	if (field)
	{
		field->uniqueId = uniqueId;
		field->setPosition(position.x,position.y);
		field->setSize(size.width,size.height);
		if (parent)
		{
			parent->addUI(field);
			parent->addGloabUI(field);
		}

	}
	return field;
}

/**
* 创建图像
*/
UIBase *UIPanel::createImage(const CCPoint& position,const CCSize &size,const char *pngName,UIPanel *parent ,int uniqueId)
{
	UIImage *img = UIImage::create(pngName);
	if (img)
	{
		img->uniqueId = uniqueId;
		img->setPosition(position.x,position.y);
		img->setSize(size.width,size.height);
		if (parent)
		{
			parent->addUI(img);
			parent->addGloabUI(img);
		}
	}
	return img;
}

/**
* 创建ChoiceList
*/
UIBase* UIPanel::createChoiceList(const CCPoint &position,const CCRect &rect, const CCSize &eachSize,int startId,UIPanel *parent,int uniqueId )
{
	UIChoiceList *clist = UIChoiceList::create(rect,eachSize.width,eachSize.height);
	if (clist)
	{
		clist->uniqueId = uniqueId;
		clist->setPosition(position.x,position.y);
//		clist->setSize(size.width,size.height);
		clist->setStartId(startId);
		if (parent)
		{
			parent->addUI(clist);
			parent->addGloabUI(clist);
		}
		
	}

	return clist;
}
/**
* 创建List
*/
UIBase* UIPanel::createList(const CCPoint &position,const CCRect &rect,const CCSize &eachSize,int startId,UIPanel *parent ,int uniqueId )
{
	UIList *list = UIList::create(rect,eachSize.width,eachSize.height);
	if (list)
	{
		list->uniqueId = uniqueId;
		list->setPosition(position.x,position.y);
		list->setStartId(startId);
		if (parent)
		{
			parent->addUI(list);
			parent->addGloabUI(list);
		}
	}

	return list;
}
/**
* 创建Bag
*/
UIBase *UIPanel::createBag(const CCPoint &position,const CCSize & bagContent,const CCSize &eachBag,int eachLeftSpan,int eachUpSpan,UIPanel *parent ,int uniqueId)
{
	UIBag * bag = UIBag::create(position.x,position.y,bagContent.width,bagContent.height,eachBag.width,eachBag.height, eachLeftSpan, eachUpSpan);
	if (bag)
	{
		bag->uniqueId = uniqueId;
		if (parent)
		{
			parent->addUI(bag);
			parent->addGloabUI(bag);
		}
		
	}
	return bag;
}
/**
* 创建选择框
*/
UIBase *UIPanel::createChoice(const CCPoint &position,const CCSize &size,const char * upImgName,const char *downImgName,UIPanel *parent ,int uniqueId)
{
	UIChoice *choice = UIChoice::create(upImgName,downImgName);
	if (choice)
	{
		choice->uniqueId = uniqueId;
		choice->setPosition(position.x,position.y);
		choice->setSize(size.width,size.height);
		if (parent)
		{
			parent->addUI(choice);
			parent->addGloabUI(choice);
		}
		
	}
	return choice;
}

void UIPanel::addUI(UIBase *base)
{
	for (CHILD_UIS_ITER iter = childuis.begin() ; iter != childuis.end();++iter)
	{
		if (base == *iter)
		{
			return;
		}
	}
	childuis.push_back(base);
	this->addChild(base);
}
bool UIPanel::addGloabUI(UIBase *base)
{
	 if (!base) return false;
	 if (base->uniqueId == -1) return false;
	 if (base->uniqueId >= uis.size())
		 uis.resize(base->uniqueId + 1);
	 if (base->uniqueId < uis.size() && !uis[base->uniqueId])
	 {
		 uis[base->uniqueId] = base;
		 return true;
	 }
	 return false;
}

 /** 
* 检查是否在区域里
*/
bool UIPanel::touchDown(float x,float y)
{
	if (!this->isVisible()) return false;
	CCPoint pos = ccp(x,y);
	pos = this->convertToNodeSpace(pos);
	float px = back ? back->getPosition().x - _width / 2 : this->getPositionX();
	float py = back ? back->getPosition().y - _height / 2 : this->getPositionY();
	nowTouchPoint = ccp(x,y);
	_touchIn =false;
	_nowTouchUI = NULL;
	for (CHILD_UIS_ITER iter = childuis.begin() ; iter != childuis.end();++iter)
	{
		UIBase *base = *iter;
		if (base && base->touchDown(x,y))
		{
			_nowTouchUI = base;
			CCLOG("can move child");
		//	if (!_moveable) return false;
			this->doEvent(EVENT_TOUCH_IN,this);
			_touchIn = true;
			return true;
		}
	}
	CCRect rect = CCRectMake(px,py,_width,_height);
	if (rect.containsPoint(pos))
	{
		_touchIn = true;
		this->doEvent(EVENT_TOUCH_NO_CHILD_IN,this);
		if (!_moveable) return false;
		return true;
	}
	return false;
}
/**
* 更新位置
*/
bool UIPanel::touchMove(float x,float y)
{
	if (!this->isVisible()) return false;
	CCPoint pos = ccp(x,y);
	std::vector<UIBase*> temps = childuis;
	if (_nowTouchUI)
	{
		this->doEvent(EVENT_TOUCH_MOVE,this);
		if (_nowTouchUI->touchMove(x,y))
		{
			return true;
		}
	}
	if (!_moveable) return false;
	for (CHILD_UIS_ITER iter = temps.begin() ; iter != temps.end();++iter)
	{
		UIBase *base = *iter;
		if (base && base->touchMove(x,y))
		{
			this->doEvent(EVENT_TOUCH_MOVE,this);
			return true;
		}
	}
	if (_moveable && _touchIn)
	{
		CCPoint nowPoint =getPosition();
        setPosition(nowPoint.x + pos.x - nowTouchPoint.x,
                                      nowPoint.y + pos.y - nowTouchPoint.y);
		nowTouchPoint = pos;
		return true;
	}
	return false;
}
/**
* 停止拖动
*/
bool UIPanel::touchEnd(float x,float y)
{
	if (!this->isVisible()) return false;
	bool tag = false;
	if (_touchIn)
	{
		this->doEvent(EVENT_TOUCH_OUT,this);
		this->doEvent(UIBase::EVENT_EDIT_DOWN,this);
		tag = true;
	}
	_touchIn = false;
	_nowTouchUI = NULL;
	
	std::vector<UIBase*> temps = childuis;
	for (CHILD_UIS_ITER iter = temps.begin() ; iter != temps.end();++iter)
	{
		UIBase *base = *iter;
		if (base && base->touchEnd(x,y))
		{
			tag = true;
		}
	}
	return tag;
}
void UIPanel::setPosition(float x,float y)
{
	CCNode::setPosition(ccp(x,y));
	this->x = x;
	this->y = y;
}
/**
* 获取UIBase
*/
UIBase * UIPanel::getUIById(int id)
{
	if (id < uis.size())
	{
		return uis.at(id);
	}
	return NULL;
}

void UIPanel::setEditable(bool tag)
{
	for (CHILD_UIS_ITER iter = childuis.begin() ; iter != childuis.end();++iter)
	{
		UIBase *base = *iter;
		if (base)
		{
			base->setEditable(tag);
		}
	}
}

/**
* 设置大小
*/
void UIPanel::setSize(float w,float h) 
{
	 if (back)
	 {
		 back->setScaleX(w / back->getContentSize().width);
		 back->setScaleY(h / back->getContentSize().height);	 
	 }
	 this->w = w;
	 this->h = h;
	 _width = w;
	_height = h;
}

UIPanel* UIPanel::create()
{
	UIPanel *node = new UIPanel();
	if (node)
	{
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}

void UIPanel::loadFromFile(const char *fileName)
{
	serialize::Stream stream;
	stream.readFromFile(fileName);
	this->parseRecord(stream);
}

void UIPanel::saveToFile(const char *fileName)
{
	serialize::Stream stream;
	stream = toRecord();
	stream.writeToFile(fileName);
}
void UIPanel::beLoaded()
{
	for (CHILD_UIS_ITER iter = childuis.begin() ; iter != childuis.end();++iter)
	{
		if (*iter)
			this->addChild(*iter);
	}
	setEditable(true);
	this->doEvent(UIBase::EVENT_PANEL_LOAD,this);
}

void UIPanel::execEach(stExecPanelEach *exec)
{
	for (CHILD_UIS_ITER iter = uis.begin() ; iter != uis.end();++iter)
	{
		if (*iter)
		{
			exec->exec(*iter);
		}
	}
}
UIButton * UIPanel::createBtn(script::tixmlCodeNode &btnNode)
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	UIButton * btn = NULL;
	if (btnNode.isValid())
	{
		//<battle uniqueid="" offsetx="" offsety="" name="" downimg="" upimg="" moveimg="" r="" g="" b=""/>
		float x = 0;
		float y = 0;
		float fontSize = 0;
		float r = 0;
		float g = 0;
		float b = 0;
		float w = 0;
		float h = 0;
		int uniqueId = 0;
		int locper = 0;
		std::string uniquename = btnNode.getAttr("uniquename");
		btnNode.getAttr("uniqueid",uniqueId);
		btnNode.getAttr("fontsize",fontSize);
		btnNode.getAttr("x",x);
		btnNode.getAttr("y",y);
		btnNode.getAttr("w",w);
		btnNode.getAttr("h",h);
		btnNode.getAttr("r",r);
		btnNode.getAttr("loctype",locper);
		btnNode.getAttr("g",g);
		btnNode.getAttr("b",b);
		std::string name = btnNode.getAttr("name");
		std::string downimg = btnNode.getAttr("downimg") ;
		std::string upimg = btnNode.getAttr("upimg");
		std::string moveimg = btnNode.getAttr("moveimg");
		btn = UIButton::createWithPngNames(upimg.c_str(),downimg.c_str(),moveimg.c_str(),name.c_str(),fontSize);
		if (locper == 1)
		{
			x *= 	visibleSize.width / 100;
			y *= 	visibleSize.height / 100;
			w *=		visibleSize.width / 100;
			h *=		visibleSize.height / 100;
		}
		if (btn)
		{
			btn->uniqueId = uniqueId;
			this->addUI(btn);
			if (w && h)
				setSize(w,h);
			btn->setPosition(x,y);
			btn->setColor(ccc3(r,g,b));
			if (w && h)
			btn->setSize(w,h);
			addGloabUI(btn);
			nameuis[uniquename] = btn;
		}
	}
	return btn;
}
UILabel * UIPanel::createLblFromNode(script::tixmlCodeNode * node)
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	std::string content = node->getAttr("content");
	float fontSize = 0;
	node->getAttr("fontsize",fontSize);
	float x =0,y = 0,w =0,h =0;
	node->getAttr("x",x);
	node->getAttr("y",y);
	node->getAttr("w",w);
	node->getAttr("h",h);
	
	int r = 0,g = 0,b = 0;

	node->getAttr("r",r);
	node->getAttr("g",g);
	node->getAttr("b",b);
	int locper = 0;
	int uniqueId = 0;
	node->getAttr("loctype",locper);
	node->getAttr("uniqueid",uniqueId);

	UILabel *label = UILabel::create(content.c_str(),fontSize);
	
	std::string uniquename = node->getAttr("uniquename");
	if (locper == 1)
	{
		x *= 	visibleSize.width / 100;
		y *= 	visibleSize.height / 100;
		w *=	visibleSize.width / 100;
		h *=	visibleSize.height / 100;
	}
	if (label)
	{
		label->setPosition(x,y);
		if (w && h)
			label->setSize(w,h);
		label->uniqueId = uniqueId;
		label->setColor(ccc3(r,g,b));
		addGloabUI(label);
		nameuis[uniquename] = label;
	}
	return label;
}
UIEditField *UIPanel::createFieldFromNode(script::tixmlCodeNode *node)
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
    int uniqueId = node->getInt("uniqueid");
    std::string uniqueName = node->getAttr("uniquename");
    CCPoint position = ccp(node->getInt("x"),node->getInt("y"));
    int locper = node->getInt("loctype");
    CCSize size = CCSizeMake(node->getInt("width"), node->getInt("height"));
	std::string defaultContent = node->getAttr("content");
	if (locper == 1)
    {
        position.x *= 	visibleSize.width / 100;
        position.y *= 	visibleSize.height / 100;
        size.width *=	visibleSize.width / 100;
        size.height *=	visibleSize.height / 100;
    }
	UIEditField *field = UIEditField::create(position,size,defaultContent.c_str());
	if (field)
	{
		field->uniqueId = uniqueId;
		field->setPosition(position.x,position.y);
		field->setSize(size.width,size.height);
		addGloabUI(field);
		nameuis[uniqueName] = field;
	}
	return field;
}
UIImage * UIPanel::createImgFromNode(script::tixmlCodeNode *node)
{
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
    const char *pngName = node->getAttr("pngname");
    int uniqueId = node->getInt("uniqueid");
    std::string uniqueName = node->getAttr("uniquename");
    UIImage *img = UIImage::create(pngName);
    CCPoint position = ccp(node->getInt("x"),node->getInt("y"));
    int locper = node->getInt("loctype");
    CCSize size = CCSizeMake(node->getInt("width"), node->getInt("height"));
	if (img)
	{
        if (locper == 1)
        {
            position.x *= 	visibleSize.width / 100;
            position.y *= 	visibleSize.height / 100;
            size.width *=	visibleSize.width / 100;
            size.height *=	visibleSize.height / 100;
        }
        if (img)
        {
            img->uniqueId = uniqueId;
            img->setPosition(position.x,position.y);
			if (size.width && size.height)
			 img->setSize(size.width,size.height);
            addGloabUI(img);
            nameuis[uniqueName] = img;
        }
	}
    return img;
}
UINumber *UIPanel::createNumFromNode(script::tixmlCodeNode *node)
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	void setColor(const ccColor3B &color); 
	float number = 0;
	float w = 0;
	float h = 0;
	float x = 0;
	float y = 0;
	char tap = '.';
	std::string pngName = node->getAttr("pngname");
	node->getAttr("number",number);
	node->getAttr("x",x);
	node->getAttr("y",y);
	node->getAttr("w",w);
	node->getAttr("h",h);
	node->getAttr("tap",tap);
	float fontSize = 16;
	node->getAttr("fontsize",fontSize);
	int r = 0,g = 0,b = 0;
	std::string uniquename = node->getAttr("uniquename");
	node->getAttr("r",r);
	node->getAttr("g",g);
	node->getAttr("b",b);
	UINumber *numui = UINumber::create(number,pngName.c_str(),12,32,tap);
	if (numui)
	{
		if ( w && h)
			numui->setSize(w,h);
		numui->setPosition(x,y);
		numui->setColor(ccc3(r,g,b));
		numui->setScale(fontSize / 12);
		addGloabUI(numui);
		nameuis[uniquename] = numui;
	}
	return numui;
}
bool UIPanel::initXFromNode(script::tixmlCodeNode *node)
{
	// 处理界面的参数
	float width = 0;
	float height = 0;
	std::string backimg = node->getAttr("backimg");
	node->getAttr("width",width);
	node->getAttr("height",height);
	std::string moveable = node->getAttr("moveable");
	if (moveable == "false")
	{
		this->setMovable(false);
	}
	std::string visible = node->getAttr("visible");
	if (visible == "false")
	{
		this->setVisible(false);
	}
	this->setSize(width,height);
	if (backimg != "")
	{
		this->replaceBack(backimg.c_str());
	}
	else
	{
		this->removeChild(back,true);
		back = NULL;
	}
	this->setAnchorPoint(ccp(0,0));
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	origin = ccpAdd(origin,ccp(width /2,height/2));
	float x = 0;
	float y = 0;
	float w = 0;
	float h = 0;
	node->getAttr("x",x);
	node->getAttr("y",y);
	node->getAttr("w",w);
	node->getAttr("h",h);
	int localinfo = 0;
	node->getAttr("local",localinfo);
	int localtype= 0;
	node->getAttr("loctype",localtype);
	if (localtype == 1)
	{
		x *= visibleSize.width / 100 + origin.x;
		y *= visibleSize.height / 100 + origin.y ;
		w *=	visibleSize.width / 100;
		h *=	visibleSize.height / 100;
	}
	else
	{
		if ((localinfo & ALIGN_TOP))
		{
			 y = visibleSize.height - height + origin.y;
		}
		if ((localinfo & ALIGN_X_CENTER))
		{
			 x = (visibleSize.width - width) / 2 + origin.x;
		}
		if ((localinfo & ALIGN_Y_CENTER))
		{
			 y = (visibleSize.height - height) / 2 + origin.y;
		}
		if ((localinfo & ALIGN_DOWN))
		{
			 y= origin.y;
		}
		if ((localinfo & ALIGN_RIGHT))
		{
			 x = visibleSize.width - width + origin.x;
		}
	}
	if (w && h)
		this->setSize(w,h);
	this->setPosition(x,y);
    /**
     * create btn 
     **/
	script::tixmlCodeNode btnNode = node->getFirstChildNode("button");
	while (btnNode.isValid())
	{
		//<button uniqueid="" offsetx="" offsety="" name="" downimg="" upimg="" moveimg="" r="" g="" b=""/>
		UIButton * btn = createBtn(btnNode);
        if (btn)
        {
            addUI(btn);
        }
		btnNode = btnNode.getNextNode("button");
	}
    /**
     * create label
     */
    script::tixmlCodeNode lblNode = node->getFirstChildNode("label");
    while (lblNode.isValid()) {
        UILabel *lbl = createLblFromNode(&lblNode);
        if (lbl)
        {
            addUI(lbl);
        }
        lblNode = lblNode.getNextNode("label");
    }
    script::tixmlCodeNode imgNode = node->getFirstChildNode("image");
    while(imgNode.isValid())
    {
        UIImage *img = createImgFromNode(&imgNode);
        if (img)
        {
            addUI(img);
        }
        imgNode = imgNode.getNextNode("image");
    }
	script::tixmlCodeNode editNode = node->getFirstChildNode("editfield");
	while(editNode.isValid())
	{
		UIEditField *field = createFieldFromNode(&editNode);
		if (field)
		{
			addUI(field);
		}
		editNode = editNode.getNextNode("editfield");
	}
	script::tixmlCodeNode bagNode = node->getFirstChildNode("bag");
	while(bagNode.isValid())
	{
		UIViewBag *xmlBag = UIViewBag::create(&bagNode);
		if (xmlBag)
		{
			childuis.push_back(xmlBag->view);
			xmlBag->addToParent(this);
			std::string uniqueName = bagNode.getAttr("uniquename");
			nameuis[uniqueName] = xmlBag;
		}
		int count = 0;
		script::tixmlCodeNode itemNode = bagNode.getFirstChildNode("item");
		while(itemNode.isValid())
		{
			XmlBagItem *item = XmlBagItem::create(&itemNode);
			xmlBag->setItem(item,count++);
			itemNode = itemNode.getNextNode("item");
		}
		xmlBag->show();
		bagNode = bagNode.getNextNode("bag");
	}
	return true;
}

void UIPanel::replaceBack(const char *pngName)
{
	if (std::string(pngName) == "" && back)
	{
	//	this->removeChild(back,true);
	//	back = NULL;
	}
	if (!back)
	{
		back = CCSprite::create();
		this->addChild(back);
	}
	if (back)
	{
		back->initWithFile(pngName);
		if (_width && _height)
		{
			back->setScaleX(_width / back->getContentSize().width);
			back->setScaleY(_height / back->getContentSize().height);
		}
		_width = back->getContentSize().width;
		_height = back->getContentSize().height;
	}
}
UIBase* UIPanel::getUIByName(std::string name)
{
	NAME_UIS_ITER iter = nameuis.find(name);
	if (iter != nameuis.end())
	{
		return iter->second;
	}
	return NULL;
}

void UIPanel::showByAction(int actionId)
{
	//CCAction *action = TFActionManager::getMe().getAction(actionId,0,ccp(0,0),NULL);
	//if (action)
	{
	//	this->runAction(action);
	}
}
NS_CC_END