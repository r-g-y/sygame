#pragma once
/**
 * 模型加载器 从文件中加载整个模型
 **/
#include <list>
#include <vector>
#include <map>
#include "serialize.h"
#include "cocos2d.h"
//#include <png.hpp>
NS_CC_BEGIN
/**
 * 帧数据
 */
class FrameData:public serialize::Object{
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
 * 图像数据
 */
class ImageData:public serialize::Object{
public:
	std::vector<FrameData> frames; // 一张图片里的帧数
	int id;
	std::string imageName;
	std::vector<unsigned int> contents;
	CCTexture2D* texture;
	/**
	 * 序列化对象
	 */
	IMP_SERIALIZE_DATA_FUNC()
	{
		SERIALIZE_DATA_CONDITION(imageName,0,imageName.size());
		SERIALIZE_DATA_CONDITION(contents,1,contents.size());
		SERIALIZE_DATA_CONDITION(frames,2,frames.size());
		SERIALIZE_DATA(id,3);
	}
	void clear()
	{
		contents.clear();
		frames.clear();

		texture = NULL;
	}
	ImageData()
	{
		texture = NULL;
	}
};
class ModelData:public serialize::Object{
public:
	std::string name; // 模型名字
	int version; // 版本号
	int hashcode; // 校验码
	std::vector<ImageData> images;
	/**
	 * 序列化对象
	 */
	IMP_SERIALIZE_DATA_FUNC()
	{
		SERIALIZE_DATA(version,0);
		SERIALIZE_DATA(hashcode,1);
		SERIALIZE_DATA_CONDITION(images,2,images.size());
	}
	bool checkImageDataValid(int id);
	
	CCTexture2D * getTextureById(int id);
	bool getFramesById(int id,std::vector<CCSpriteFrame*> &frames);
	/**
	 * 优化模型数据 主要是将images 合并 将 小于 256 * 256 的图片进行合并
	 * 新建一个png 512 * 512 然后放置图片 和 frameData 超出512 计算新的图片数据 images 放置 512 * 512 的图片
	 */
	void conbine();
	/**
	 * 获取png 数据放置于imgages
	 */
	void loadImageData(int id, const char *pngName);
protected:
	void beLoaded();
#if (0)
	void doGetImage(ImageData &data,png::image< png::rgba_pixel > &img);
	void doMakeRect(ImageData &data,png::image< png::rgba_pixel > &img,int width,int height);
#endif
	typedef std::vector<ImageData>::iterator IMAGEDATA_LIST_ITER;
};

class ModelLoader{
public:
	static ModelLoader & getMe()
	{
		static ModelLoader loader;
		return loader;
	}
	std::map<std::string,ModelData*> models;
	typedef std::map<std::string,ModelData*>::iterator MODELS_ITER;
	ModelData * getModelDataByName(std::string name);

	static bool getFileList(std::vector<std::string> & fileList);
};

NS_CC_END
