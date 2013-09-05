/**
 * 线段构成的阻挡空间
 */
#pragma once
#include "LineCollide.h"
#include <vector>
#include "xmlScript.h"
NS_CC_BEGIN
/**
 * 朴素的算法 用来检测是否能将物体围在线段构成的区域里
 */
class LineBlockMap:public script::tixmlCode,public CCLayer{
public:
	bool loadFromXml(const char*fileName);
	bool checkCollide(const CCRect& rect);
	bool checkSpriteNextPointValid(CCSprite *sprite,const CCPoint &dest);
	void draw();
	CREATE_FUNC(LineBlockMap);
	bool init()
	{
		return true;
	}
private:
	std::vector<Line> _lines;
	/**
	 * 从配置文件中加载配置
	 * \param node 配置根节点
	 */
	void takeNode(script::tixmlCodeNode *node);
};

NS_CC_END