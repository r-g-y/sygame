/**
 * 将大图片分割 小图片合并 并且提供一致的加载能力
 */
#include "PngPack.h"
NS_CC_BEGIN
PackPng* PackPng::create()
{
	PackPng * pRet = new PackPng();
	if (pRet && pRet->init())
	{
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool PackPng::init()
{
	return true;
}
/**
 * 获取最优的放置区域
 * return ZeroRect 时说明不能放置
 */
CCRect TextureData::getPrefectRect(const CCRect & rect)
{
	std::vector<char> grids;
	int gridWidth = (EACH_TEXTURE_WIDTH / EACH_BLOCK_WIDTH);
	int gridHeight = (EACH_TEXTURE_HEIGHT / EACH_BLOCK_HEIGHT);
	grids.resize( gridWidth * gridHeight);
	for (int i = 0; i < frames.size();i++)
	{
		FrameData & data = frames.at(i);
		for (int i = data.x ; i < data.x + data.width;i++)
			for (int j = data.y ; j < data.y + data.height;j++)
			{
				grids[(i  / EACH_BLOCK_WIDTH) + (j /EACH_BLOCK_HEIGHT)  * gridWidth ] = 1;
			}
	}

	for (int i = 0; i < gridWidth;i++)
		for (int j = 0; j < gridHeight;j ++)
		{
			bool collide = false;
			for ( int h = 0; h < rect.size.width / EACH_BLOCK_WIDTH ;h++)
			{
				for ( int k = 0; k < rect.size.height / EACH_BLOCK_HEIGHT ; k ++)
				{
					int index = ( i + h) + (j + k) * gridWidth ;
					if (index >= grids.size() || 1 == grids[index ])
					{
						collide = true;
					}
				}
				if (collide)
				{
					break;
				}
			}
			if (!collide)
			{
				return CCRectMake(i *EACH_BLOCK_WIDTH ,j * EACH_BLOCK_HEIGHT,rect.size.width,rect.size.height);
			}
		}
	return CCRectZero;
}
#if (0)
/**
 * 增加图片到指定区域 返回 framesId
 * \param img 待加入的img
 * \param rect 待加入的区域
 */
int TextureData::addImgByRect(const png::image< png::rgba_pixel > &img,const CCRect & rect)
{
	png::image< png::rgba_pixel > pngbuffer;
	if (contents.empty())
		pngbuffer.resize(EACH_TEXTURE_WIDTH,EACH_TEXTURE_HEIGHT);
	if (contents.size())
	{
		std::stringstream ss;
		ss.write((char*)&contents[0],contents.size());
		pngbuffer.read_stream(ss);
	}
	for ( int i = 0; i < rect.size.width;i++)
		for (int j = 0; j <rect.size.height;j++)
		{
			int sx = i + rect.origin.x;
			int sy = j + rect.origin.y;
			pngbuffer.set_pixel(sx,sy,img.get_pixel(i, j));
		}
	std::stringstream buffer;
	pngbuffer.write_stream(buffer);
	contents.resize(buffer.str().size());
	memcpy(&contents[0],(void*)buffer.str().c_str(),buffer.str().size());
	FrameData data;
	data.x = rect.origin.x;
	data.y = rect.origin.y;
	data.width = rect.size.width;
	data.height = rect.size.height;
	int frameId = frames.size();
	frames.push_back(data);
	return frameId;
}
/**
 * 将区域画在图片中
 */
void TextureData::write(png::image< png::rgba_pixel > &img,int frameId,const CCRect & rect)
{
	if (frameId >= frames.size() && frameId >= 0) return;
	png::image< png::rgba_pixel > buffer;
	buffer.resize(EACH_TEXTURE_WIDTH,EACH_TEXTURE_HEIGHT);
	std::stringstream ss;
	ss.write((char*)&contents[0],contents.size());
	buffer.read_stream(ss);
	FrameData& data = frames.at(frameId);
	CCRect srcRect = CCRectMake(data.x,data.y,data.width,data.height);
	if ( rect.size.width != srcRect.size.width || rect.size.height != srcRect.size.height) return;
	for ( int i = 0; i < rect.size.width;i++)
		for (int j = 0; j < rect.size.height;j++)
		{
			int dx = rect.origin.x + i;
			int dy = rect.origin.y + j;
			int sx = srcRect.origin.x + i;
			int sy = srcRect.origin.y + j;
			img.set_pixel(dx,dy,buffer.get_pixel(sx,sy));
		}
}
void TextureData::write(png::image< png::rgba_pixel > &img)
{
	img.resize(EACH_TEXTURE_WIDTH,EACH_TEXTURE_HEIGHT);
	std::stringstream ss;
	ss.write((char*)&contents[0],contents.size());
	img.read_stream(ss);
}
#endif
/**
 * 将区域挂载在节点中
 */
void TextureData::write(CCNode *node,int frameId,const CCRect & rect)
{
	if (frameId >= frames.size() && frameId >= 0) return;
	
	FrameData& data = frames.at(frameId);
	CCRect srcRect = CCRectMake(data.x,data.y,data.width,data.height);
	
    CCImage image;      


	if ( !texture && image.initWithImageData((void*)&contents[0], contents.size(),  CCImage::kFmtPng))
	{
		texture = new CCTexture2D();
		
		if( texture && texture->initWithImage(&image) )
		{
			texture->setAliasTexParameters();
		}
	}
	CCSprite *sprite = CCSprite::spriteWithTexture(texture,srcRect);
	
	if (sprite)
	{
		node->addChild(sprite);
		sprite->setPosition(rect.origin);
	}
}
/**
 * 获取一帧
 */
CCSpriteFrame * TextureData::getFrame(int frameId)
{
	if (frameId >= frames.size() && frameId >= 0) return NULL;
	
	FrameData& data = frames.at(frameId);
	CCRect srcRect = CCRectMake(data.x,data.y,data.width,data.height);
	
    CCImage image;      


	if ( !texture && image.initWithImageData((void*)&contents[0], contents.size(),  CCImage::kFmtPng))
	{
		texture = new CCTexture2D();
		if( texture && texture->initWithImage(&image) )
		{
			texture->setAliasTexParameters();
		}
	}

	return CCSpriteFrame::createWithTexture(texture,srcRect);
}
/**
 * 加载成功
 */
void TextureData::beLoaded( serialize::Stream & ss)
{
	PngPack::getMe().doGetBack(ss.size());
}
#if (0)
void ObjectData::write(PngPack *pack,png::image< png::rgba_pixel > & pngbuffer)
{
	pngbuffer.resize(this->image_width,this->image_height);	
	for ( int i = 0; i < frameIds.size();i++)
	{
		int y = i / width_size;
		int x = i % width_size;
		// 将图片复原
		TextureData * data = pack->getTextureByID(frameIds[i].textureId);
		if (data)
		{
			data->write(pngbuffer,frameIds[i].frameId,CCRectMake(x * each_pixel_width ,y * each_pixel_height,each_pixel_width,each_pixel_height));
		}
	}
}
#endif
/**
 * 将object 数据组装成sprite 集合并加入node 节点
 */
void ObjectData::write(PngPack *pack,CCNode *node)
{
	for ( int i = 0; i < frameIds.size();i++)
	{
		int y = i / width_size;
		int x = i % width_size;
		// 将图片复原
		TextureData * data = pack->getTextureByID(frameIds[i].textureId);
		if (data)
		{
			int rectx = x * each_pixel_width + each_pixel_width / 2;
			int recty = image_height - y * each_pixel_height - each_pixel_height / 2;
			data->write(node,frameIds[i].frameId,CCRectMake(rectx ,recty,each_pixel_width,each_pixel_height));
		}
	}
}
void ObjectData::write(PngPack *pack,PackPng *png)
{
	png->offset = ccp(each_pixel_width/2,each_pixel_width/2);
	for ( int i = 0; i < frameIds.size();i++)
	{
		int y = i / width_size;
		int x = i % width_size;
		// 将图片复原
		TextureData * data = pack->getTextureByID(frameIds[i].textureId);
		if (data)
		{
			int rectx = x * each_pixel_width + each_pixel_width / 2;
			int recty = image_height - y * each_pixel_height - each_pixel_height / 2;
			data->write(png,frameIds[i].frameId,CCRectMake(rectx ,recty,each_pixel_width,each_pixel_height));
		}
	}
	png->size = CCSizeMake(image_width,image_height);
}
stLoadPackCallback *PngPack::loadCallback = NULL;
/**
 * 获取一个精灵帧
 */
CCSpriteFrame* ObjectData::getFrame(PngPack *pack)
{
	if (frameIds.empty()) return NULL;
	TextureData * data = pack->getTextureByID(frameIds[0].textureId);
	if (data)
	{
		return data->getFrame(frameIds[0].frameId);
	}
	return NULL;
}
/**
 * 加载成功
 */
void ObjectData::beLoaded( serialize::Stream & ss)
{
	PngPack::getMe().doGetBack(ss.size());
}
/**
* 解包 将数据恢复成可读的png 图片
*/
void PngPack::unpack()
{
#if (0)
	for ( OBJECTS_ITER iter = objects.begin(); iter != objects.end();++iter)
	{
		ObjectData & object = iter->second;
		png::image< png::rgba_pixel > pngbuffer;
		object.write(this,pngbuffer);
		pngbuffer.write("pack.png");
	}
#endif
}
//int PngPack::allBinarySize = 1;
/**
 * 合并图片进pack系统
 */
void PngPack::addPng(const char *pngName)
{
#if (0)
	/**
	 * 获取图片信息
	 */
	png::image< png::rgba_pixel > img;
	img.read(pngName); // 尝试切割在256 * 256 
	
	std::vector<png::image< png::rgba_pixel> > tmpImgs;
	int width_size = img.get_width() / TextureData::EACH_IMAGE_WIDTH;
	int width_index = img.get_width() % TextureData::EACH_IMAGE_WIDTH;
	if (width_index)
	{
		width_size ++;
	}
	int width_pixel_width = img.get_width() / width_size; // 每个子图片的宽

	int height_size = img.get_height() / TextureData::EACH_IMAGE_HEIGHT;
	int height_index = img.get_height() % TextureData::EACH_IMAGE_HEIGHT;
	if (height_index)
	{
		height_size ++;
	}
	int height_pixel_height = img.get_height() / height_size; // 每个字图片的高

	tmpImgs.resize(width_size * height_size);
	for (int i = 0; i < tmpImgs.size(); i++)
	{
		tmpImgs[i].resize(width_pixel_width,height_pixel_height);
	}
	for (int i = 0 ;i < img.get_width();i++)
		for (int j = 0; j < img.get_height();j++)
		{
			int imgId = i / width_pixel_width + (j / height_pixel_height) * width_size;
			tmpImgs[imgId].set_pixel(i % width_pixel_width,j % height_pixel_height,img.get_pixel(i,j));
		}
#ifdef _PNG_PACK_TEST // 查看切割情况
	for (int i = 0; i < tmpImgs.size(); i++)
	{
		std::stringstream ss;
		ss << i<<"y=" <<i / width_size <<"x=" <<i % width_size<<".png" ;

		tmpImgs[i].write(ss.str().c_str());	
	}
	png::image< png::rgba_pixel > imgEnd;
	imgEnd.resize(img.get_width(),img.get_height());
	for (int i = 0; i < tmpImgs.size(); i++)
	{
		int y = i / width_size;
		int x = i % width_size;
		// 将图片复原
		for ( int h = x * width_pixel_width; h < (x + 1) * width_pixel_width;h++)
			for (int j = y * height_pixel_height ; j < (y + 1) * height_pixel_height; j++)
			{
				imgEnd.set_pixel(h,j,tmpImgs[i].get_pixel(h -x * width_pixel_width,j -y * height_pixel_height));
			}
	}
	imgEnd.write("back.png");
#endif
	// 将图片写入pack 中
	ObjectData object;
	object.width_size = width_size;
	object.each_pixel_height = height_pixel_height;
	object.each_pixel_width = width_pixel_width;
	object.image_width = img.get_width();
	object.image_height = img.get_height();
	// 查看当前图片可以写入的纹理
	for (int i = 0; i < tmpImgs.size();i++)
	{
		png::image< png::rgba_pixel > &nowImg = tmpImgs[i];
		CCRect nowRect = CCRectMake(0,0,nowImg.get_width(),nowImg.get_height());
		TextureData * nowTexture = NULL;
		CCRect targetRect = CCRectZero;
		for ( TEXTURES_ITER iter = textures.begin(); iter != textures.end();++iter)
		{
			targetRect = iter->getPrefectRect(nowRect);
			if (targetRect.equals(CCRectZero))
			{
				continue;
			}
			nowTexture = &(*iter);
		}
		if (!nowTexture) // 创建一个新的纹理对象
		{
			TextureData textureData;
			textureData.textureId = textures.size();
			targetRect = textureData.getPrefectRect(nowRect);
			textures.push_back(textureData);
			nowTexture = &textures.back();
		}
		
		int frameId = nowTexture->addImgByRect(nowImg,targetRect);
		if (frameId != -1)
			object.frameIds.push_back(TextureIndex(nowTexture->textureId,frameId));
	}
	object.pngName = pngName;
	objects[pngName] = object;
#endif
}
/**
 * 保存到pack 里
 */
void PngPack::save(const char *packName)
{
	serialize::Stream ss = toRecord();
	ss.writeToFile(packName);
}
/**
 * 从pack 中加载
 */
void PngPack::load(const char *packName)
{
	serialize::Stream ss;
	ss.readFromFile(packName);
	parseRecord(ss);
}
/**
 * 从pack 中加载
 */
void PngPack::load(const char *packName,stLoadPackCallback *callback)
{
	loadCallback = callback;
	serialize::Stream ss;
	ss.readFromFile(packName);
	parseRecord(ss);
}
/**
 * 当前加载的大小
 */
void PngPack::doGetBack(int size)
{
	if (loadCallback)
	{
		loadCallback->doGet(allBinarySize,size);
	}
}
PackPng * PngPack::getPackPng(const char *name)
{
	OBJECTS_ITER iter = objects.find(name);
	if ( iter != objects.end())
	{
		PackPng *node = PackPng::create();
		iter->second.write(this,node);
		return node;
	}
	return NULL;
}
/**
 * 获取节点 自动将相关的图片组合成Node 节点
 */
CCNode * PngPack::getNode(const char*pngName)
{
	OBJECTS_ITER iter = objects.find(pngName);
	if ( iter != objects.end())
	{
		CCNode *node = CCNode::create();
		iter->second.write(this,node);
		return node;
	}
	return NULL;
}
/**
 * 获取SpriteFrame
 */
CCSpriteFrame * PngPack::getFrame(const char *pngName)
{
	OBJECTS_ITER iter = objects.find(pngName);
	if ( iter != objects.end())
	{
		return iter->second.getFrame(this);;
	}
	return NULL;
}
/**
 * 获取帧数
 **/
bool PngPack::getFrames(const char *pngName,std::vector<CCSpriteFrame*> & frames)
{
	return false;
}
/**
 * 获取精灵
 * \param pngName 图片名字
 */	
CCSprite * PngPack::getSprite(const char *pngName)
{
	return NULL;
}

TextureData *PngPack::getTextureByID(int txtId)
{
	if (txtId < textures.size())
	{
		return &textures[txtId];
	}
	return NULL;
}
void PngPack::saveAllTextures()
{
	int i = 0;
#if (0)
	for ( TEXTURES_ITER iter = textures.begin(); iter != textures.end();++iter)
	{
		png::image< png::rgba_pixel > temppng;
		iter->write(temppng);
		std::stringstream ss;
		ss << i++ << ".png";
		temppng.write(ss.str().c_str());
	}
#endif
}
NS_CC_END