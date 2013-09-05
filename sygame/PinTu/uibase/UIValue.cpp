#include "UIValue.h"
NS_CC_BEGIN
/**
 * 创建数值条
 */
UIValue * UIValue::create(const char *backPng,const char *valuePng,float maxvalue)
{
	UIValue *item = new UIValue();
	if (item)
	{
		item->init(backPng,valuePng,maxvalue);
		item->autorelease();
		return item;
	}
	CC_SAFE_DELETE(item);
	return NULL;
}
/**
 * 设置数值
 */
void UIValue::setValue(float value)
{
	if (this->_valuePng)
	{
		_value = value;
		this->_valuePng->setScaleX(value / (this->_maxvalue + 1));
		CCPoint nowPoint = this->_backPng->getPosition();
		float width = ( this->_backPng->getContentSize().width - this->_valuePng->getContentSize().width * this->_valuePng->getScaleX() )/ 2.0;
		// 主要调整 x轴坐标
		this->_valuePng->setPosition(ccp(nowPoint.x - width,nowPoint.y));
	
		std::stringstream ss;
		ss << _value<<"/"<<_maxvalue;
		if (numberText)
		{
			numberText->setString(ss.str().c_str());
		}
	}
}

void UIValue::init(const char *backPng,const char *valuePng,float maxvalue)
{
	this->_maxvalue = maxvalue;
	this->_backPng = CCSprite::create(backPng);
	this->_valuePng = CCSprite::create(valuePng);
	this->addChild(_backPng);
	this->addChild(_valuePng);
	numberText = CCLabelTTF::create("","Arial", this->_backPng->getContentSize().height);
	numberText->setColor(ccc3(0,255,0));
	this->addChild(numberText,2);
}
NS_CC_END