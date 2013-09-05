#include "UINumber.h"

NS_CC_BEGIN
/**
 *  设置展示内容
 */
void UINumber::setNumber(float number)
{
	if (labelAtlas)
	{
		std::stringstream ss;
		ss << number;
		labelAtlas->setString(ss.str().c_str());
	}
}


void UINumber::setColor(const ccColor3B &color)
{
	if (labelAtlas)
	{
		labelAtlas->setColor(color);
	}
}
UINumber * UINumber::create(float number,const char *pngName,float width,float height,char tap)
{
	UINumber *node = new UINumber();
	if (node)
	{
		if (node)
		{
			std::stringstream ss;
			ss << number;
			node->labelAtlas = CCLabelAtlas::create(ss.str().c_str(),pngName, width, height, tap);
			node->addChild(node->labelAtlas);
			node->autorelease();
			return node;
		}
	}
	CC_SAFE_DELETE(node);
	return NULL;

}

void UINumber::setPosition(float x,float y)
{
	CCNode::setPosition(ccp(x,y));
}

void UINumber::setSize(float x,float y)
{
	
}
NS_CC_END