/**
 * 富文本编辑框实现
 */

#pragma once
#include "cocos2d.h"
#include "vector"
#include "xmlScript.h"
#include "UIBase.h"
NS_CC_BEGIN
struct stUIRichString{
	std::string name;
	std::string content;
	float x; // 起始横坐标
	float y;  // 起始纵坐标
	float fontSize; // 字体大小
	float width; // 区域所占的宽
	float height; // 区域所占的高
	int stringType; //类型 
	ccColor3B color; // 颜色
	enum{
		UI_STRING_TYPE_FONT, // 字体
		UI_STRING_TYPE_IMG, // 图像
	};
	stUIRichString()
	{
		x = y = 0;
		fontSize = 0;
		width = height = 0;
		stringType = UI_STRING_TYPE_FONT;
		color = ccWHITE;
	}
};

/**
 * 该文本框可以承载图片 文字
 */
class UIRichTextArea :public CCLabelBMFont,public script::tixmlCode{
public:
	static UIRichTextArea* createFromFile(CCNode *parent,const char *fileName);
	static UIRichTextArea* createFromString(CCNode *parent,const char *content);
	/**
	 * 重新实现该函数即可
	 */
	void createFontChars();
	void createFonts(const unsigned short * m_sString,int nextFontPositionX,int nextFontPositionY,const ccColor3B& m_tColor,int fontSize);
	void createImage(const char * pngName,float x,float y,float w,float h);

	void update(const std::string& name,const std::string& content);
	std::string getValueByName(const std::string &name);
protected:
	std::vector<stUIRichString> _contents;
	typedef std::vector<stUIRichString>::iterator CONTENTS_ITER;
	/**
	 * 从配置文件中加载配置
	 * \param node 配置根节点
	 */
	void takeNode(script::tixmlCodeNode *node);
	float width;
	std::string fntFileName;
	float height;
	CCNode *node ;
	UIRichTextArea()
	{
		node = NULL;
		width = height = 0;
	}
	bool initWithXml(const char *fileName);
	bool initFromString(const char *content);
};

class UIRichArea:public UIBase{
public:
	static UIRichArea * create(const char *fileName);
	static UIRichArea * createFromString(const char *content);
	/**
	 * 设置位置
	 */
	virtual void setPosition(float x,float y);
	
	/**
	 * 设置大小
	 */
	virtual void setSize(float w,float h);
	 /** 
	 * 检查是否在区域里
	 */
	virtual bool touchDown(float x,float y);
	/**
	 * 更新位置
	 */
	virtual bool touchMove(float x,float y);
	/**
	 * 停止拖动
	 */
	virtual bool touchEnd(float x,float y);

	/**
	 * 设置为可编辑模式
	 */
	virtual void setEditable(bool tag){}

	void setContent(const std::string& content);

	void update(const std::string& name,const std::string& content);
	std::string getValueByName(const std::string &name);
private:
	bool init(const char *fileName);
	bool initFromString(const char *content);
	UIRichTextArea * _textarea;
};
NS_CC_END