#include "ModelLoader.h"
#include <sstream>
NS_CC_BEGIN
CCTexture2D * ModelData::getTextureById(int id)
{
	if (id < images.size())
		return images.at(id).texture;
	return NULL;
}
void ModelData::beLoaded()
{
	for (int i = 0; i < images.size();i++)
	{
		ImageData & data = images.at(i); // 只支持png 格式
		CCImage image;
		if (data.contents.size())
		{
			if (image.initWithImageData((void*)&data.contents[0], data.contents.size(),  CCImage::kFmtPng))
			{
				CCTexture2D *texture = new CCTexture2D();
                
                if( texture && texture->initWithImage(&image) )
				{
					data.texture = texture;
				}
			}
		}
	}
}
void ModelData::loadImageData(int id, const char *pngName)
{
	 std::string pathKey = pngName;

    pathKey = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(pathKey.c_str());

    std::string fullpath = pathKey;
    {
        std::string lowerCase(pngName);
        for (unsigned int i = 0; i < lowerCase.length(); ++i)
        {
            lowerCase[i] = tolower(lowerCase[i]);
        }
        do 
        {
           
            CCImage::EImageFormat eImageFormat = CCImage::kFmtUnKnown;
            if (std::string::npos != lowerCase.find(".png"))
            {
                eImageFormat = CCImage::kFmtPng;
            }
			else
			{
				break;
			}
            CCImage image;      
            unsigned long nSize = 0;
            unsigned char* pBuffer = CCFileUtils::sharedFileUtils()->getFileData(fullpath.c_str(), "rb", &nSize);
			if (nSize)
			{
				ImageData data;
				data.imageName = pngName;
				data.contents.resize(nSize);
				memcpy(&data.contents[0],(void*)pBuffer,nSize);
				if (id >= images.size())
				{
					images.resize(id + 1);
				}
				images[id] = data;
				if (image.initWithImageData((void*)&data.contents[0], data.contents.size(),  CCImage::kFmtPng))
				{
					if (images[id].texture)
					{
						images[id].texture->release();
						images[id].texture= NULL;
					}
					CCTexture2D *texture = new CCTexture2D();
                
					if( texture && texture->initWithImage(&image) )
					{
						images[id].texture = texture;
					}
				}
			}
		}while(0);
	}
}
/**
 * 合并算法 处理图片的整合
 */
void ModelData::conbine()
{
#if (0)
	std::vector<png::image< png::rgba_pixel > > rgbaimages;
	for (int i = 0; i < images.size();	i++)
	{
		png::image< png::rgba_pixel > img;
		img.read(images[i].imageName.c_str());
		rgbaimages.push_back(img);
	}
	images.clear();
	{
		png::image< png::rgba_pixel > image;
		ImageData data;
		int width = 512; int height = 512;
		image.resize(width,height);
		int offsetx = 0;
		int offsety = 0;
		int maxHeight = 0;
		for (std::vector<png::image< png::rgba_pixel > >::iterator iter =  rgbaimages.begin(); iter != rgbaimages.end();++iter)
		{
			png::image< png::rgba_pixel > & nowData = *iter; 
			if (offsetx + nowData.get_width() > width)
			{
				offsetx = 0;
				offsety += maxHeight;
			}
			if (offsety + nowData.get_height()> height )
			{
				doGetImage(data,image);
				image = png::image< png::rgba_pixel > ();
				image.resize(width,height);
				images.push_back(data);
				data.clear();
				 offsetx = 0;
				 offsety = 0;
				 maxHeight = 0;
			}
			int count = 0;
			for (int y = offsety; y < offsety + nowData.get_height(); y ++)
				for (int x = offsetx; x <  offsetx + nowData.get_width(); x++)
				{
					image.set_pixel(x,y,nowData.get_pixel(x - offsetx,y - offsety));
				}
			offsetx += nowData.get_width() + 2;
			maxHeight = maxHeight < nowData.get_height() ? nowData.get_height():maxHeight;
			doMakeRect(data,nowData,offsetx,offsety);
		}
		if (offsetx != 1 && offsety != 1)
		{
			doGetImage(data,image);
			image = png::image< png::rgba_pixel > ();
			image.resize(width,height);
			images.push_back(data);
			data.clear();
		}
	}
	beLoaded(); // 重新设定新的texture
#endif
}
#if (0)
void ModelData::doMakeRect(ImageData &data,png::image< png::rgba_pixel > &img,int x,int y)
{
	FrameData frameData;
	frameData.x = x;
	frameData.y = y;// 512 - y - img.get_height();
	frameData.width = img.get_width();
	frameData.height = img.get_height();
	data.frames.push_back(frameData);
}
void ModelData::doGetImage(ImageData &data,png::image< png::rgba_pixel > &img)
{
	std::stringstream ss;
	img.write_stream(ss);
	data.contents.resize(ss.str().size());
	bcopy((void*)ss.str().c_str(),&data.contents[0],ss.str().size());
	std::stringstream name;
	static int index = 0;
	name << "conbine" << index++ << ".png";
	img.write(name.str().c_str());
}
#endif
bool ModelData::checkImageDataValid(int id)
{
	return images.at(id).contents.size();
}
/**
 * 获取模型的第id 号 动画序列
 */
bool ModelData::getFramesById(int id,std::vector<CCSpriteFrame*> &frames)
{
	if (id < images.size())
	{
		ImageData &imageData = images[id];
		for (int i = 0; i < imageData.frames.size();i++)
		{
			FrameData &frame = imageData.frames[i];
			CCSpriteFrame *cf = CCSpriteFrame::frameWithTexture(imageData.texture, CCRectMake(frame.x,frame.y, frame.width, frame.height));
			frames.push_back(cf);
		}
		return frames.size();
	}
	return false;
}
ModelData * ModelLoader::getModelDataByName(std::string name)
{
	MODELS_ITER iter = models.find(name);
	if (iter != models.end())
	{
		return iter->second;
	}
	serialize::Stream stream;
	stream.readFromFile(name.c_str());
	ModelData *data = new ModelData();
	if (data->parseRecord(stream))
		models[name] = data;

	return data;
}

bool ModelLoader::getFileList(std::vector<std::string> & fileList)
{
#if (0)
	WIN32_FIND_DATA fd; 
    ZeroMemory(&fd, sizeof(WIN32_FIND_DATA)); 
  
	std::string filePathName = "." ;
  
     filePathName += ('\\'); 
    filePathName += ("*"); 
  
  
    HANDLE m_hFind = FindFirstFile(filePathName.c_str(), &fd); 
    //如果目录不是".",".." 
    if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && strcmp(fd.cFileName, (".")) && strcmp(fd.cFileName, (".."))) 
    { 
       std::string fname = fd.cFileName;
	   if (fname.find("png") != std::string::npos)
			fileList.push_back(fname);
    } 
  
    BOOL bSearchFinished = FALSE; 
    while( !bSearchFinished ) 
    { 
        if( FindNextFile(m_hFind, &fd)) 
        { 
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && strcmp(fd.cFileName, (".")) && strcmp(fd.cFileName, (".."))) 
			{ 
			   std::string fname = fd.cFileName;
			   if (fname.find("png") != std::string::npos)
				 fileList.push_back(fname);
			} 
  
        } 
        else
        { 
            break; 
        } 
    } 
    FindClose(m_hFind);     
    return true; 
#endif
	return true;
}
NS_CC_END
