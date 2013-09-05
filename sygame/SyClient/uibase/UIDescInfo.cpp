#include "UIDescInfo.h"
NS_CC_BEGIN
RpgText *RpgText::create(const char *str,float width,float fontSize,const ccColor3B& color)
{
	RpgText *pRet = new RpgText();
    if (pRet && pRet->init(str,width,fontSize,color))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool RpgText::init(const char *str,float width,float fontSize,const ccColor3B& color)
{
	infoLabel = UIMultiLineArea::create(str,width,fontSize,color);
	this->addChild(infoLabel);
	return true;
}
void RpgText::update(const std::string &content)
{
	if (infoLabel)
	{
		infoLabel->update(content);
	}
}
RpgText * RpgText::create(script::tixmlCodeNode *node)
{
	if (node)
	{
		int x = node->getInt("x");
		int y = node->getInt("y");
		int r = node->getInt("r");
		int g = node->getInt("g");
		int b = node->getInt("b");
		RpgText *rpgText = RpgText::create(node->getAttr("content"),node->getFloat("width"),node->getInt("fontsize"),ccc3(r,g,b));
		rpgText->setPosition(ccp(x,y));
		return rpgText;
	}
	return NULL;
}


/**
 * 初始化
 */
bool RpgInfoUI::init(const char *fileName)
{
	unsigned long nSize = 0;
    std::string file= CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(fileName);
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData(file.c_str(),"rb",&nSize);
	if (nSize)
	{
		return script::tixmlCode::initFromString((char*)buffer);
	}
	return false;
}
struct stDoClick:public UIEventCallback<stDoClick>{
	stDoClick()
	{
		endUI = NULL;
	}
	void doClick(UICenterItem *item)
	{
		if (endUI)
			endUI->setVisible(false);
	}
	RpgInfoUI *endUI;
};
const CCSize & RpgInfoUI::getContentSize()
{
	return backSize;
}
/**
 * 处理节点
 */
void RpgInfoUI::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
		this->setAnchorPoint(ccp(0,0));
		script::tixmlCodeNode backNode = node->getFirstChildNode("back");
		if (backNode.isValid())
		{
			std::string backName = backNode.getAttr("src");
			int backw= backNode.getInt("w");
			int backh = backNode.getInt("h");
			backSize = CCSizeMake(backw,backh);
			if (backName != "")
			{
				CCSprite *back = CCSprite::create(backName.c_str());
				if (back)
				{
					back->setScaleX(backw / back->getContentSize().width);
					back->setScaleY(backh / back->getContentSize().height);
					this->addChild(back,-1);
					back->setAnchorPoint(ccp(0,0));
				}
			}
		}
		script::tixmlCodeNode textNode = node->getFirstChildNode("text");
		while(textNode.isValid())
		{
			RpgText * text = RpgText::create(&textNode);
			if (text)
			{
				std::string name = textNode.getAttr("name");
				this->updateContents[name] = text;
				this->addChild(text);
				text->setAnchorPoint(ccp(0,0));
			}
			textNode = textNode.getNextNode("text");
		}
		script::tixmlCodeNode imgNode = node->getFirstChildNode("image");
		while (imgNode.isValid())
		{
			std::string imgName = imgNode.getAttr("name");
			int x = imgNode.getInt("x");
			int y = imgNode.getInt("y");
			int w = imgNode.getInt("w");
			int h = imgNode.getInt("h");
			if (imgName != "")
			{
				CCSprite *sprite = CCSprite::create(imgName.c_str());
				if (sprite)
				{
					sprite->setPosition(ccp(x,y));
					sprite->setScaleX(w / sprite->getContentSize().width);
					sprite->setScaleY(h / sprite->getContentSize().height);
					this->addChild(sprite);
					sprite->setAnchorPoint(ccp(0,0));
				}
			}
			imgNode = imgNode.getNextNode("image");
		}
        script::tixmlCodeNode btnNode = node->getFirstChildNode("button");
        while (btnNode.isValid())
        {
            UICenterItem *btn = UICenterItem::create(btnNode.getAttr("uppng"), btnNode.getAttr("downpng"),
                                                     CCSizeMake(btnNode.getFloat("width"), btnNode.getFloat("height")));
            this->addChild(btn);
            btn->setPosition(ccp(btnNode.getFloat("x"),btnNode.getFloat("y")));
            buttons[btnNode.getAttr("name")] = btn;
            btnNode = btnNode.getNextNode("button");
        }
		script::tixmlCodeNode bagNode = node->getFirstChildNode("bag");
		while(bagNode.isValid())
		{
			UIViewBag *xmlBag = UIViewBag::create(&bagNode);
			if (xmlBag->view)
				xmlBag->view->setPosition(bagNode.getInt("x"),bagNode.getInt("y"));
			xmlBag->addToParent(this);
			bags.push_back(xmlBag);
			bagNode  = bagNode.getNextNode("bag");
		}
        UICenterItem *close = getBtnByName("close");
        if (close)
        {
            stDoClick *doClick = stDoClick::create();
            doClick->endUI = this;
            close->bind(doClick);
        }
        /*
		close = UICenterItem::create("close.png","close_back.png",CCSizeMake(64,64));
		if (close)
		{
			this->addChild(close);
			close->setPosition(ccp(156,156));
		}
		
		stDoClick *doClick = stDoClick::create();
		doClick->endUI = this;
		close->bind(doClick);
         */
	}
}
UICenterItem * RpgInfoUI::getBtnByName(const std::string & name)
{
    std::map<std::string, UICenterItem*>::iterator iter = buttons.find(name);
    if ( iter != buttons.end())
    {
        return iter->second;
    }
    return NULL;
}
/**
 * 处理点击
 **/
bool RpgInfoUI::doTouch(int touchType,const CCPoint &touchPoint)
{
	if (!this->isVisible()) return false;
    for (std::map<std::string, UICenterItem*>::iterator iter = buttons.begin(); iter!=buttons.end();++iter)
    {
        bool tag = iter->second->onTouch(touchType,touchPoint);
        if (tag)
        {
            return true;
        }
    }
	for (std::vector<UIViewBag*>::iterator iter = bags.begin(); iter != bags.end();++iter)
	{
		if ((*iter)->doTouch(touchType,touchPoint)) return true;
	}
	//if (close) return close->onTouch(touchType,touchPoint);
	return false;
}
/**
 * 更新name 指定的数值
 */
void RpgInfoUI::updateNumber(const std::string& name,int number)
{
	UPDATE_CONTENTS_ITER iter = this->updateContents.find(name);
	if (iter != this->updateContents.end())
	{
		std::stringstream ss;
		ss << number;
		iter->second->update(ss.str().c_str());
	}
}

void RpgInfoUI::updateString(const std::string &name,const std::string& content)
{
	UPDATE_CONTENTS_ITER iter = this->updateContents.find(name);
	if (iter != this->updateContents.end())
	{
		iter->second->update(content);
	}
}
/**
 * 控制每个tag的展示
 */
void RpgInfoUI::show(const std::string &name,bool showTag)
{

}

/**
 * 展示
 */
void RpgInfoUI::show()
{
	this->setVisible(true);
}
/**
 * 隐藏
 */
void RpgInfoUI::hide()
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	setPositionX(visibleSize.width/2);
	this->setVisible(false);
}

NS_CC_END