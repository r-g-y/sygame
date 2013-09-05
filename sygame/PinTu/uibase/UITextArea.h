#pragma once
#include "cocos2d.h"
#include "vector"
NS_CC_BEGIN
/**
 * 实现颜色多变的文本框 信息展示用 这里是最简单的方法 
 */
class UITextArea:public CCLabelBMFont{
public:
	/**
	 * 创建UITextArea
	 */
	static UITextArea * create(const char *str, const char *fntFile);
	/**
	 * 重新实现该文件即可
	 */
	void createFontChars();
	/**
	 * 设置字体颜色
	 */
	void setColors(int startId,int endId,const ccColor3B& color);
private:
	std::vector<ccColor3B> colors;
};
 
// TODO 富文本的信息展示 展示的内容来自xml 文件 用来信息描述
/***
 * <richtext fnt="">
 *		<font x="" y="" fontSize="" color="" content=""/>
 *		<font x="" y ="" fontSize="" color="" content=""/>
 *		<img src="" x="" y="" width="" height=""/>
 * </richtext>
 */
NS_CC_END