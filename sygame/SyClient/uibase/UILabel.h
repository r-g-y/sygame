#pragma once
#include "cocos2d.h"
#include "UIBase.h"
NS_CC_BEGIN

class UILabel:public UIBase{
public:
	static UILabel* create(const char *content,float fontSize);
	static UILabel* create();
	void beLoaded();
	bool initWithContent(const char *content,float fontSize);

	void setPosition(float x,float y);

	void setSize(float x,float y);
//	SERIALIZE_KIND(UIBase::UI_LABEL);
	 /** 
	 * 检查是否在区域里
	 */
	virtual bool touchDown(float x,float y) ;
	/**
	 * 更新位置
	 */
	virtual bool touchMove(float x,float y);
	/**
	 * 停止拖动
	 */
	virtual bool touchEnd(float x,float y);
	/**
	 *  设置展示内容
	 */
	void setContent(const char *content);

	void setEditable(bool tag)
	{
		_editable = tag;
	}
	std::string getContent(){if (text) return text->getString();return "";}
	void setColor(const ccColor3B &color); 
	/**
	 * 序列化
	 */
	IMP_SERIALIZE_DATA_FUNC()
	{
		SERIALIZE_DATA(x,0);
		SERIALIZE_DATA(y,1);
		SERIALIZE_DATA(w,2);
		SERIALIZE_DATA(h,3);
		SERIALIZE_DATA(uniqueId,4);
		SERIALIZE_DATA(name,5);
		SERIALIZE_DATA(uiType,6);
		SERIALIZE_DATA(content,7);
	}
	UILabel()
	{
		_touchIn = false;
		_editable = false;
		text = NULL;
		uiType = UIBase::UI_LABEL;
	}
private:
	CCLabelTTF *text;
	CCPoint nowTouchPoint;
	bool _editable;
	bool _touchIn;
	std::string content;
	
};

NS_CC_END