#include "UILineValue.h"
#include "UIPanel.h"
NS_CC_BEGIN
/**
 * 创建
 */
UILineValue* UILineValue::create(const char *backname,const char *valuename,float minValue,float maxValue,float defaultValue)
{
	UILineValue *node = new UILineValue();
	if (node && node->init(backname,valuename,minValue,maxValue,defaultValue))
	{
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}
/**
 * 初始化
 */
bool UILineValue::init(const char *backname,const char *valuename,float minValue,float maxValue,float defaultValue)
{
	_back = CCSprite::create(backname);
	_span = CCSprite::create(valuename);

	
	this->minValue = minValue;
	this->maxValue = maxValue;
	this->setSize(100,10);
	this->nowValue = defaultValue;
	std::stringstream nows;
	nows<< nowValue;
	_info = (UILabel*)UIPanel::createLabel(ccp(width/2,0),CCSizeMake(200,10),nows.str().c_str(),15);
	if (_info)
	{
		this->addChild(_info,2);
		_info->setColor(ccc3(233,0,0));
	}
	_back->setAnchorPoint(ccp(0,0.5));
	_span->setAnchorPoint(ccp(0,0.5));
	this->addChild(_back);
	this->addChild(_span);
	update();
	return true;
}
/** 
 * 检查是否在区域里
 */
bool UILineValue::touchDown(float x,float y)
{
	CCPoint pos = ccp(x,y);
	pos = this->convertToNodeSpace(pos);
	nowTouchPoint = ccp(x,y);
	if (_back)
	{
		float width = _back->getContentSize().width * _back->getScaleX();
		float height = _back->getContentSize().height * _back->getScaleY();
		CCRect rect = CCRectMake(
			_back->getPosition().x ,
			_back->getPosition().y,
			width,
			height);
		if (rect.containsPoint(pos))
		{
			_touchIn = true;
			return true;
		}
	}
	return false;
}
/**
 * 更新位置
 */
bool UILineValue::touchMove(float x,float y)
{
	CCPoint pos = ccp(x,y);
//	pos = this->convertToNodeSpace(pos);
	/**
	 * 移动设置距离
	 */
	if (_touchIn)
	{
		CCPoint nowPoint = _span->getPosition();
		float dx = pos.x - nowTouchPoint.x + nowPoint.x;
		if (dx < 0 || dx >= width)
			return false;
        _span->setPosition(ccp(nowPoint.x + pos.x - nowTouchPoint.x,
                                      nowPoint.y));
		nowTouchPoint = pos;

		nowValue = (maxValue - minValue) * _span->getPositionX() / width;
		this->doEvent(UIBase::EVENT_CLICK_DOWN,this);
		std::stringstream nows;
		nows<< nowValue;
		_info->setContent(nows.str().c_str());
		return true;
	}
	return false;
}
/**
 * 停止拖动
 */
bool UILineValue::touchEnd(float x,float y)
{
	if (_touchIn)
	{
		nowValue = (maxValue - minValue) * _span->getPositionX() / width;
		this->doEvent(UIBase::EVENT_CLICK_DOWN,this);
		std::stringstream nows;
		nows<< nowValue;
		_info->setContent(nows.str().c_str());
	}
	_touchIn = false;
	return true;
}

/**
 * 设置位置
 */
void UILineValue::setPosition(float x,float y)
{
	CCNode::setPosition(ccp(x,y));
}

/**
 * 设置大小
 */
void UILineValue::setSize(float w,float h)
{
	if (_back)
	{
		_back->setScaleX( w / _back->getContentSize().width);
		_back->setScaleY( h / _back->getContentSize().height);
	}
	
	width = w;
	height = h;
}
/**
 * 更新
 */
void UILineValue::update()
{
	if (_span)
	{
		_span->setPosition(ccp(width * nowValue / ( maxValue - minValue),_span->getPositionY()));
	}
	std::stringstream nows;
	nows<< nowValue;
	_info->setContent(nows.str().c_str());
}
NS_CC_END