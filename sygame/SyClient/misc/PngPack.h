#pragma once
/**
 * 将大图片分割 小图片合并 并且提供一致的加载能力
 */
#include "cocos2d.h"
#include <list>
#include <vector>
#include <map>
#include "serialize.h"
#if (0)
#include <png.hpp>
#endif
NS_CC_BEGIN
/**
 * 纹理引索
 */
class TextureIndex : public serialize::Object{
public:
	int textureId; // 纹理编号
	int frameId; // 内部帧编号
	IMP_SERIALIZE_DATA_FUNC()
	{
		SERIALIZE_DATA(textureId,0);
		SERIALIZE_DATA(frameId,1);
	}
	TextureIndex(int textureId,int frameId)
	{
		this->textureId = textureId;
		this->frameId = frameId;
	}
	TextureIndex()
	{
		textureId = -1;
		frameId = -1;
	}
};
class PngPack;
class PackPng:public CCNode{
public:
	static PackPng* create();
	bool init();
	const CCSize & getContentSize(){
		return size;
	}
	void setPosition(const CCPoint &point)
	{
		CCNode::setPosition(ccpAdd(point,offset));
	}
	CCPoint offset;
	CCSize size;
};
/**
 * 对象数据
 */
class ObjectData: public serialize::Object{
public:
	std::string pngName; // 资源名字
	int width_size; // 图片宽
	int each_pixel_width; // 每个资源的宽
	int each_pixel_height; // 每个资源的高
	int image_width; // 图片宽
	int image_height; // 图片高
	std::vector<TextureIndex> frameIds; // 资源列表
	typedef std::vector<TextureIndex>::iterator FRAMEIDS_ITER;
#if (0)
	/**
	 * object 数据写入到png 图片
	 */
	void write(PngPack *pack,png::image< png::rgba_pixel > & pngbuffer);
#endif
	/**
	 * 将object 数据组装成sprite 集合并加入node 节点
	 */
	void write(PngPack *pack,CCNode *node);

	void write(PngPack *pack,PackPng *png);
	/**
	 * 获取一个精灵帧
	 */
	CCSpriteFrame* getFrame(PngPack *pack);
	IMP_SERIALIZE_DATA_FUNC()
	{
		SERIALIZE_DATA(pngName,0);
		SERIALIZE_DATA(frameIds,1);
		SERIALIZE_DATA(width_size,2);
		SERIALIZE_DATA(each_pixel_width,3);
		SERIALIZE_DATA(each_pixel_height,4);
		SERIALIZE_DATA(image_width,5);
		SERIALIZE_DATA(image_height,6);
	}
	/**
	 * 加载成功
	 */
	virtual void beLoaded( serialize::Stream & ss);
};
/**
 * 帧数据
 */
class FrameData : public serialize::Object{
public:
	int x,y,width,height;
	IMP_SERIALIZE_DATA_FUNC()
	{
		SERIALIZE_DATA(x,0);
		SERIALIZE_DATA(y,1);
		SERIALIZE_DATA(width,2);
		SERIALIZE_DATA(height,3);
	}
	FrameData()
	{
		x = y = width = height = 0; 
	}
};
/**
 * 纹理数据
 */
class TextureData:public serialize::Object{
public:
	enum {
		EACH_IMAGE_WIDTH = 256,
		EACH_IMAGE_HEIGHT = 256,
		EACH_TEXTURE_WIDTH = 512,
		EACH_TEXTURE_HEIGHT = 512,
		EACH_BLOCK_WIDTH = 32, // 每个网格的宽
		EACH_BLOCK_HEIGHT = 32, // 每个网格的高
	};
	int textureId; //内部的id 号
	std::vector<FrameData> frames; // 图片中必然含有的帧
	std::vector<unsigned char> contents;
	CCTexture2D* texture;
	IMP_SERIALIZE_DATA_FUNC()
	{
		SERIALIZE_DATA(textureId,0);
		SERIALIZE_DATA(frames,1);
		SERIALIZE_DATA(contents,2);
	}
	TextureData()
	{
		texture = NULL;
	}
	/**
	 * 获取最优的放置区域
	 * return ZeroRect 时说明不能放置
	 */
	CCRect getPrefectRect(const CCRect & rect);
#if (0)
	/**
	 * 增加图片到指定区域 返回 framesId
	 * \param img 待加入的img
	 * \param rect 待加入的区域
	 */
	int addImgByRect(const png::image< png::rgba_pixel > &img,const CCRect & rect);

	/**
	 * 将区域画在图片中
	 */
	void write(png::image< png::rgba_pixel > &img,int frameId,const CCRect & rect);
	void write(png::image< png::rgba_pixel > &img);
#endif
	/**
	 * 获取一帧
	 */
	CCSpriteFrame * getFrame(int frameId);
	/**
	 * 将区域挂载在节点中
	 */
	void write(CCNode *node,int frameId,const CCRect & rect);
	/**
	 * 加载成功
	 */
	virtual void beLoaded( serialize::Stream & ss);
};
/**
 * 加载资源回调
 */
struct stLoadPackCallback{
	virtual void doGet(int allSize,int nowTap) = 0;
};
/**
 * 将png 打包
 */
class PngPack :public serialize::Object{
public:
	PngPack()
	{
	//	loadCallback = NULL;
	}
	static PngPack & getMe()
	{
		static PngPack pack;
		return pack;
	}
	/**
	 * 合并图片进pack系统
	 */
	void addPng(const char *pngName);
	/**
	 * 保存到pack 里
	 */
	void save(const char *packName);
	/**
	 * 从pack 中加载
	 */
	void load(const char *packName);
	/**
	 * 从pack 中加载
	 */
	void load(const char *packName,stLoadPackCallback *callback);
	/**
	 * 解包 将数据恢复成可读的png 图片
	 */
	void unpack();
	
	void saveAllTextures();
	TextureData *getTextureByID(int txtId);
public:
	/**
	 * 获取合并的Png
	 */
	PackPng * getPackPng(const char *name);
	/**
	 * 获取节点 自动将相关的图片组合成Node 节点
	 */
	CCNode * getNode(const char*pngName);
	/**
	 * 获取SpriteFrame
	 */
	CCSpriteFrame * getFrame(const char *pngName);
	/**
	 * 获取帧数
	 **/
	bool getFrames(const char *pngName,std::vector<CCSpriteFrame*> & frames);
	/**
	 * 获取精灵
	 * \param pngName 图片名字
	 */	
	CCSprite * getSprite(const char *pngName);

	/**
	 * 获取帧数据
	 * \param index 引索数据
	 */
	FrameData * getFrameData(const TextureIndex &index);
	std::vector<TextureData> textures; // 纹理数据
	std::map<std::string,ObjectData> objects; // 对象数据
	typedef std::vector<TextureData>::iterator TEXTURES_ITER;
	typedef std::map<std::string,ObjectData>::iterator OBJECTS_ITER;
	IMP_SERIALIZE_DATA_FUNC()
	{
		SERIALIZE_DATA(textures,0);
		SERIALIZE_DATA(objects,1);
	}
	/**
	 * 预备加载
	 * */
	virtual void preLoad(serialize::Stream & ss){
		allBinarySize = ss.size();
	}
	/**
	 * 加载成功
	 */
	virtual void beLoaded(serialize::Stream & ss)
	{
		if (loadCallback)
		{
			loadCallback->doGet(allBinarySize,allBinarySize);
		}
		loadCallback = NULL;
	}
	static stLoadPackCallback *loadCallback;
	int allBinarySize; // 整个二进制文件的大小
	/**
	 * 当前加载的大小
	 */
	void doGetBack(int size);
};

NS_CC_END