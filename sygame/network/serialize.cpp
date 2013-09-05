#include "serialize.h"
#include "stdio.h"
using namespace serialize;
#define IMP_STORE_CELL_FUNC(__class__,__type__)\
bool Stores::addStore(__class__ value,SEARCH_ARG name)\
{\
	Stream s;\
	makeStream(value,s);\
	if (s.size()==0) return false;\
	Store store (__type__,&(s.content[0]),s.size(),name);\
	addStore(store);\
	return true;\
}\
bool Stores::getStore(__class__ &value,SEARCH_ARG name)\
{\
	{\
		Store store;\
		if (pickStore(store,name))\
		{\
			{\
				Stream ss(&store.content[0],store.coreData.size);\
				parseStream(value,ss);\
				return true;\
			}\
		}\
	}	\
	return false;\
}
/**
 * 转为串对象
 * \return 串
 **/
Stream Stores::toRecord()
{
#if (0)
	stream.reset();
	return stream;
#endif
	Stream stream;
	for (std::vector<Store>::iterator iter = stores.begin(); iter != stores.end();++iter)
	{
		stream.addStore(*iter);
	}
	stream.reset();
	return stream;
}
/**
 * 转为记录集合
 * \param record 串
 **/
void Stores::parseRecord(Stream& record)
{
#if (0)
	stream = record;
	return;
#endif
	record.reset();
	Store store;
	while(record.pickStore(store))
	{
		int index = store.getIndex();
		if (index < 0) continue;
		if (stores.size() <= index)
			stores.resize(index + 5);
		stores[index] = store;
	}
}
/**
 * 增加一个store 
 **/
void Stores::addStore(Store& store)
{
#if (0)
	stream.addStore(store);
#endif
	stores.push_back(store);
}
/**
 * 获取一个store
 */
bool Stores::pickStore(Store& store,SEARCH_ARG name)
{
#if (0)
	stream.pickStore(nowstore);
#endif
	if (name < stores.size())
	{
		store = stores[name];
		if (store.content.empty()) return false;
		return true;
	}
	return false;
}

/**
 * 特例化string 使string 转化为stream 对象
 */
void Stores::makeStream(std::string str,Stream &stream)
{
	int size = str.size()+1;
	stream.addData(&size,sizeof(size));
	stream.addData((void*)str.c_str(),size);
}
void Stores::parseStream(std::string & str,Stream &stream)
{
	int size = 0;
	if (stream.pickData(&size,sizeof(size)))
	{
		char * content = new char[size];
		stream.pickData(content,size);
		str = content;
		delete content;
	}
}
/**
 * 持久化string 进入stream
 */
void Stores::addStore(std::string str,SEARCH_ARG name)
{
    Stream s;
	makeStream(str,s);
	Store store (__DATA_STRING__,&(s.content[0]),s.size(),name);
	addStore(store);
}
bool Stores::addStore(const void *value,unsigned int size,SEARCH_ARG name)
{
	if (!value) return false;
	Store store (__VOID_STAR_POINT__,(void*)value,size,name);
	addStore(store);
	return true;
}	
void Object::__store__(Stream & ss,unsigned char tag)
{
	Stores sts;
	if (tag == __LOAD__)
	{
		preLoad();
		sts.parseRecord(ss);
	}
	else
	{
		preStore();
	}
	__v_store__(sts,tag);
	if (tag == __LOAD__)
	{
		beLoaded();
	}
	else
	{
		ss = sts.toRecord();	
		beStored();
	}
}

void Stream::writeToFile(const char *fileName)
{
    FILE * file = fopen(fileName, "wb");
    if (file)
    {
        char version = 's';
        fwrite(&version, sizeof(char), 1, file);
        int size = content.size();
        fwrite(&size, sizeof(int), 1, file);
        fwrite(&content[0], content.size(), 1, file);
        fclose(file);
    }
}

void Stream::readFromFile(const char *fileName)
{
    FILE * file = fopen(fileName, "rb");
    if (file)
    {
        char version = '\0';
        fread(&version, sizeof(char), 1, file);
        if (version == 's')
        {
            int size = 0;
            fread(&size, sizeof(int), 1, file);
            content.resize(size);
            fread(&content[0], size, 1, file);
        }
        fclose(file);
    }
	
}
/**
* 重置流
*/
void Stream::reset()
{
	offset = 0;
}
/**
* 增加一个Store
*/
void Stream::addStore(Store & store)
{
	if (store.content.empty()) return;
	if (offset + sizeof(CoreData) + store.coreData.size > size())
	{
		resize(offset + sizeof(CoreData) + store.coreData.size);
	}
	bcopy(&store.coreData,&content[offset],sizeof(CoreData));
	bcopy(&(store.content[0]),+ (&content[sizeof(CoreData) + offset]),store.coreData.size);
	offset += sizeof(CoreData) + store.coreData.size;
}

/**
 * 获取下一个Store
 */
bool Stream::pickStore(Store &store)
{
	if (offset + sizeof(CoreData) <= size() )
	{
		bcopy(&content[offset] ,&store.coreData,sizeof(CoreData));
		if (0 == store.coreData.size) return false;
		store.content.resize(store.coreData.size);
		if (sizeof(CoreData)+offset + store.coreData.size <= size())
			bcopy(&content[sizeof(CoreData)+offset] ,&store.content[0],store.coreData.size);
		else return false;
		offset += sizeof(CoreData) + store.coreData.size;
		return true;
	}
	return false;
}

/**
 * 增加一段数据
 * \param data 指针
 * \param size 数据长度
 */
void Stream::addData(void *data,int size)
{
	if (0 == size) return;
	if (offset + size > this->size())
	{
		content.resize(offset + size);
	}
	bcopy(data,&content[offset],size);
	offset += size;
}
/**
 * 获取指定长度的数据
 * \param data 外部数据缓存
 * \param size 接受数据的长度
 * \return 成功true 失败false
 */
bool Stream::pickData(void *data,int size)
{
	if (0 == size) return false;
	if (offset + size <= this->size())
	{
		bcopy(&content[offset],data,size);
	}
	else return false;
	offset += size;
	return true;
}

IMP_STORE_CELL_FUNC(int,__DATA__INT__);
IMP_STORE_CELL_FUNC(unsigned short,__DATA__INT__);
IMP_STORE_CELL_FUNC(float,__DATA_FLOAT__);
IMP_STORE_CELL_FUNC(unsigned int ,__DATA_UNSIGNED_INT__);
IMP_STORE_CELL_FUNC(char,__DATA_CHAR__);
IMP_STORE_CELL_FUNC(long,__DATA_LONG__);
IMP_STORE_CELL_FUNC(unsigned char,__DATA_UNSINGED_CHAR__);
IMP_STORE_CELL_FUNC(unsigned long,__DATA_UNSINGED_LONG__);
IMP_STORE_CELL_FUNC(long long,__DATA_LONG_LONG__);
IMP_STORE_CELL_FUNC(unsigned long long,__DATA_UNSINGED_LONG_LONG__);
