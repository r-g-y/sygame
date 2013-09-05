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
 
/**
 * 多行文本的解决方案
 **/
class UIMultiLineArea:public CCNode{
public:
	/**
	 * 创建文本 超出区域自动换行 设置一致的颜色 一致的大小
	 */
	static UIMultiLineArea *create(const char *str,float width,float fontSize,const ccColor3B& color);

	bool init(const char *str,float width,float fontSize,const ccColor3B& color);

	void update(const std::string &content);
	float width;
	float fontSize;
	ccColor3B color;
};
NS_CC_END