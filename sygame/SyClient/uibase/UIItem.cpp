#include "UIItem.h"

NS_CC_BEGIN
UIItem *UIItem::clone()
{
	UIItem *item = UIItem::create();
	if (item)
	{
		item->bagId = bagId;
		item->setSize(_oldRect.size.width,_oldRect.size.height);
		return item;
	}
	CC_SAFE_DELETE(item);
	return NULL;
}

void UIItem::show()
{
	this->setVisible(true);
}
/**
* 在包裹中的位置
*/
void UIItem::setSize(float width,float height)
{
	_oldRect = CCRectMake(0,0,width,height);
}
void UIItem::hide()
{
	this->setVisible(false);
}
void UIItem::show(const CCRect& viewRect)
{
	_viewRect = viewRect;
	showInView = true;
}
void UIItem::setPosition(float x,float y)
{
	this->setAnchorPoint(ccp(0,0));
	CCNode::setPosition(ccp(x,y));
}
	
void UIItem::visit()
{
#if (0)
	if (showInView)
	{
		CCRect oldRect = CCRectMake(this->getPositionX(),this->getPositionY(),_oldRect.size.width,_oldRect.size.height);
		CCPoint pos;
		CCRect textR;
		CCRect viewRect = CCRectMake(_viewRect.origin.x,_viewRect.origin.y,_viewRect.size.width / this->getScaleX(),_viewRect.size.height / this->getScaleY());
		if (RectIntesect::calcTexture(oldRect,viewRect,pos,textR))
		{
			CCSprite::setPosition(pos);
			this->setTextureRect(textR);
			setVisible(true);
		}
		else
		{
			setVisible(false);
		}
	}
	CCSprite::visit();
#else
	if (showInView)
	{
		CCSize szLimitSize= CCSizeMake(_viewRect.size.width ,_viewRect.size.height);

		CCPoint worldPt = ccp(_viewRect.origin.x,_viewRect.origin.y);
		worldPt = this->getParent()->convertToWorldSpace(worldPt);
		CCSize size;

		if (CC_CONTENT_SCALE_FACTOR() != 1)
		{
			size.height = szLimitSize.height * CC_CONTENT_SCALE_FACTOR();
			size.width = szLimitSize.width * CC_CONTENT_SCALE_FACTOR();
			worldPt = ccpMult(worldPt, CC_CONTENT_SCALE_FACTOR());
		}
		else

		{
			size.width = szLimitSize.width;
			size.height =szLimitSize.height;
		}
		glEnable(GL_SCISSOR_TEST);  
		CCDirector::sharedDirector()->getOpenGLView()->setScissorInPoints(worldPt.x ,worldPt.y,  size.width, size.height);
		CCNode::visit();  
		glDisable(GL_SCISSOR_TEST);  
	}
	else
		CCNode::visit();  
#endif
	
}
/**
 * 检查点是否在Item 中
 */
bool UIItem::checkIn(float x,float y)
{
	float width = _oldRect.size.width;
	float height = _oldRect.size.height;
	CCPoint pos = getPosition();
	CCRect rect = CCRectMake(
			pos.x,
			pos.y,
			width,
			height);
	if (rect.containsPoint(ccp(x,y)))
	{
		return true;
	}
	return false;
}
NS_CC_END