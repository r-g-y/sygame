#pragma once
#include "cocos2d.h"
#include <vector>
NS_CC_BEGIN
/**
 * 绘制坐标线
 */

class MapCoordLine:public CCSprite{
public:
	/**
	 * 创建一个坐标线
	 */
	static MapCoordLine* create(const char *fileName,int width,int height,int eachWidth,int eachHeight);
	/**
	 * 使用纹理初始化
	 */
	virtual bool initWithTexture(CCTexture2D *pTexture, const CCRect& rect, bool rotated);
	/**
	 * 设置数据
	 */
	void setTextureRect(const CCRect& rect, bool rotated, const CCSize& untrimmedSize);
	/**
	 * 绘制
	 */
	virtual void draw(void);
	/**
	 * 设置说明信息
	 */
	void initDescInfo();
private:
	int _width; // 宽
	int _height; // 高
	int _eachWidth; // 间隔宽
	int _eachHeight; // 间隔高
	std::vector<ccV3F_C4B_T2F> points; // 顶点集合
};

NS_CC_END