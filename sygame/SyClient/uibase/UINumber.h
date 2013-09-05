 //CCLabelAtlas* labelAtlas = CCLabelAtlas::create("0000", "fps_images.png", 12, 32, '.');

#pragma once
#include "cocos2d.h"
#include "UIBase.h"
NS_CC_BEGIN
class UINumber:public UIBase{
public:
	void setPosition(float x,float y);

	void setSize(float x,float y);
	 /** 
	 * 检查是否在区域里
	 */
	virtual bool touchDown(float x,float y){return false;}
	/**
	 * 更新位置
	 */
	virtual bool touchMove(float x,float y){return false;}
	/**
	 * 停止拖动
	 */
	virtual bool touchEnd(float x,float y){return false;}
	/**
	 *  设置展示内容
	 */
	void setNumber(float number);

	void setEditable(bool tag){}
	void setColor(const ccColor3B &color); 
	static UINumber * create(float number,const char *pngName,float width,float height,char tap);
private:
	CCLabelAtlas* labelAtlas;
	UINumber()
	{
		labelAtlas = NULL;
	}
};

NS_CC_END