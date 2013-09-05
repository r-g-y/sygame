#pragma once
#include "cocos2d.h"
#include <vector>
#include "MapObject.h"
NS_CC_BEGIN
/**
 * 地表
 */
class MapBackGroud:public MapObject{
public:
	MapBackGroud():MapObject(OBJECT_BACK_GROUD)
	{
	
	}
	/**
	 * 创建一个地表
	 */
	static MapBackGroud* create(const char *fileName,int width,int height,int eachWidth,int eachHeight);
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
	virtual void updateTransform(void);

	bool checkIn(float x,float y);
private:
	std::vector<ccV3F_C4B_T2F> points; // 顶点集合
	int _width;
	int _height;
	int _eachWidth;
	int _eachHeight;
};

NS_CC_END