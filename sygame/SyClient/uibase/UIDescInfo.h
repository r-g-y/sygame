#pragma once
#include "cocos2d.h"
#include "UIXmlBag.h"
#include "UICenterBag.h"
#include "xmlScript.h"
#include "UITextArea.h"
NS_CC_BEGIN
/**
 * 一轮结束后信息告示
 */
class RpgText:public CCNode{
public:
	static RpgText * create(script::tixmlCodeNode *node);
	static RpgText *create(const char *str,float width,float fontSize,const ccColor3B& color);
	bool init(const char *str,float width,float fontSize,const ccColor3B& color);
	void update(const std::string &content);
private:
	UIMultiLineArea* infoLabel;
	RpgText()
	{
		infoLabel = NULL;
	}
};
class RpgInfoUI:public CCNode,public script::tixmlCode{
public:
	/**
	 * 初始化
	 */
	bool init(const char *fileName);
	/**
	 * 处理节点
	 */
	void takeNode(script::tixmlCodeNode *node);
	std::map<std::string,RpgText*> updateContents;
	typedef std::map<std::string,RpgText*>::iterator UPDATE_CONTENTS_ITER;

	std::map<std::string,CCSprite*> sprites; // 精灵列表
	std::vector<UIViewBag*> bags;
	/**
	 * 更新name 指定的数值
	 */
	void updateNumber(const std::string& name,int number);

	void updateString(const std::string &name,const std::string& content);
	/**
	 * 控制每个tag的展示
	 */
	void show(const std::string &name,bool showTag);

	/**
	 * 展示
	 */
	void show();
	/**
	 * 隐藏
	 */
	void hide();
	/**
	 * 处理点击
	 **/
	bool doTouch(int touchType,const CCPoint &touchPoint);
    UICenterItem * getBtnByName(const std::string & name);
    std::map<std::string,UICenterItem*> buttons;
	//UICenterItem *close;
	const CCSize & getContentSize();
	CCSize backSize;
};

NS_CC_END