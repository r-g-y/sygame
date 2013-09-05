/**
 * email:ji.jin.dragon@gmail.com
 * Author jijinlong
 * Data 2011.10.14~~~
 * All Rights Resvered
 ***/
#pragma once
#include "sstream"
#include "stdio.h"
#include "iostream"
#include "string.h"
#include "string"
#include "vector"
#include "set"
#include <map>
#include "sys.h"
#include "sString.h"
#ifdef _USE_ZLIB_ 
	#include "zlib.h"
#endif
#include "list"
#ifdef _USE_BASE64_
	#include "Base64.h"
#endif

#define IMP_SERIALIZE_DATA_START(__class__)\
	void __class__::__v_store__(serialize::Stores &sts,BYTE tag)\

#define IMP_SERIALIZE_DATA_FUNC()\
	void __v_store__(serialize::Stores &sts,BYTE tag)\

#define IMP_SERIALIZE_DATA_FUNC_CONDITION(__result__)\
	void __v_store__(serialize::Stores &sts,BYTE tag){\
		if ((!(__result__))&& tag == __STORE__ ) return;

#if (0)
#define SERIALIZE_DATA(__value__)\
	switch(tag)\
	{\
		case __STORE__:\
		{\
			sts.addStore(__value__,#__value__);\
		}break;\
		case __LOAD__:\
		{\
			sts.getStore(__value__,#__value__);\
		}break;\
	}
#define SERIALIZE_DATA_CONDITION(__value__,__result__)\
	switch(tag)\
	{\
		case __STORE__:\
		{\
			if ((__result__))\
			sts.addStore(__value__,#__value__);\
		}break;\
		case __LOAD__:\
		{\
			sts.getStore(__value__,#__value__);\
		}break;\
	}
#endif

#define SERIALIZE_DATA(__value__,index)\
	switch(tag)\
	{\
		case __STORE__:\
		{\
			sts.addStore(__value__,index);\
		}break;\
		case __LOAD__:\
		{\
			sts.getStore(__value__,index);\
		}break;\
	}
#define SERIALIZE_DATA_CONDITION(__value__,index,__result__)\
	switch(tag)\
	{\
		case __STORE__:\
		{\
			if ((__result__))\
			sts.addStore(__value__,index);\
		}break;\
		case __LOAD__:\
		{\
			sts.getStore(__value__,index);\
		}break;\
	}

#define IMP_SERIALIZE_DATA_END }
#define IMP_SERIALIZE_DATA_FUNC_END } 
#define CALL_SERIALIZE(__class__)\
		__class__::__store__(ss,tag);
#define __STORE__ 1
#define __LOAD__ 2

#define DEC_SERIALIZE_DATA void __v_store__(serialize::Stores &ss,BYTE tag)

namespace serialize{
	const DWORD MAX_NAME_SIZE= 32;
	#pragma pack(1)
	const BYTE __DATA_OBJECT__=1; // point 数据
	const BYTE __DATA_OBJECT_MAP__ =2; // map 集合<normal,point> 数据
	const BYTE __DATA_OBJECT_VECTOR__ = 3;
	const BYTE __DATA__INT__ = 4;
	const BYTE __DATA_FLOAT__ = 5;
	const BYTE __DATA_UNSIGNED_INT__ = 6;
	const BYTE __DATA_CHAR__ = 7;
	const BYTE __DATA_LONG__ = 8;
	const BYTE __DATA_UNSINGED_CHAR__ = 9;
	const BYTE __DATA_UNSINGED_LONG__ = 10;
	const BYTE __DATA_UNSINGED_FLOAT__ = 11;
	const BYTE __DATA_LONG_LONG__ = 12;
	const BYTE __DATA_UNSINGED_LONG_LONG__ = 13;	
	const BYTE __VOID_STAR_POINT__ = 14;
	const BYTE __DATA_STRING__ = 15;
	typedef int SEARCH_ARG; 
	/**
	 * 记录头，固定长度
	 **/
	struct CoreData{
		BYTE tag; // 类型
		int hashcode; // 名字 取前4个做校验
		DWORD size; // 大小
		CoreData()
		{
			bzero(this,sizeof(CoreData));
		}
	};
	#pragma pack()
	/**
	 * 记录
	 **/
	struct Store{
		CoreData coreData; // 记录头
		std::vector<char> content; // 记录内容
		DYNAMIC_API Store()
		{
		}
		/**
		 * 初始化
		 * \param content 内容首地址
		 * \param size 大小
		 * \param name 记录名字
		 **/
		DYNAMIC_API Store(void *content,int size,SEARCH_ARG name)
		{
			this->content.resize(size);
			bcopy(content,&this->content[0],size);
			//strncpy(coreData.name,name,MAX_NAME_SIZE -1 );
			coreData.hashcode = name;//sys::HashStr::calc(name);
			coreData.tag = 1;
			coreData.size = size;
		}
		/**
		 *  初始化
		 * \param tag 记录类型 __DATA_DWORD__ 等
		 * \param content 内容首地址
		 * \param size 大小
		 * \param name 记录名字 
		 **/
		DYNAMIC_API Store(BYTE tag,void *content,int size,SEARCH_ARG name)
		{
			this->content.resize(size);
			bcopy(content,&this->content[0],size);
			//strncpy(coreData.name,name,MAX_NAME_SIZE -1 );
			coreData.hashcode = name;//sys::HashStr::calc(name);
			coreData.tag = tag;
			coreData.size = size;
		}
		/**
		 * 赋值函数
		 **/
		DYNAMIC_API Store & operator=(const Store & store)
		{
			content.resize(0);
			content.insert(content.begin(),store.content.begin(), store.content.end());	
			//strncpy(this->coreData.name,store.coreData.name,MAX_NAME_SIZE-1);
			this->coreData.hashcode = store.coreData.hashcode;
			coreData.tag = store.coreData.tag;
			coreData.size = store.coreData.size;
			return *this;
		}
		DYNAMIC_API bool equal(SEARCH_ARG name)
		{
			return (coreData.hashcode == name /*sys::HashStr::calc(name)*/);
		}
		/**
		 * 设置序号
		 */
		DYNAMIC_API void setIndex(int index)
		{
			this->coreData.hashcode = index;
		}
		/** 
		 * 获取序号index
		 **/
		DYNAMIC_API int getIndex()
		{
			return coreData.hashcode;
		}
	};
	/**
	 * 串
	 **/
	struct Stream{
		std::vector<char> content; // 内容
		int offset;
		DYNAMIC_API Stream()
		{
			NCHAR = '\0';
			offset = 0;
		}
		/**
		 * 重置流
		 */
		DYNAMIC_API void reset();
		/**
		 * 初始化串
		 * \param buf首地址
		 * \param size buf长度
		 **/
		DYNAMIC_API Stream(void *str,unsigned int size)
		{
			content.resize(size);
			bcopy(str,&content[0],size);	
			offset = 0;
			NCHAR = '\0';
		}
		DYNAMIC_API Stream & operator = (const Stream & str)
		{
			content = str.content;
			offset = str.offset;
			NCHAR = '\0';
			return *this;
		}
		/**
		 * 设置串大小
		 * \param size 串大小
		 **/
		DYNAMIC_API void resize(DWORD size)
		{
			content.resize(size);
		}
#ifdef _USE_ZLIB_
		/**
		 * 压缩
		 **/
		void compress(char *buffer,unsigned int size)
		{
			//toBase64();
			bzero(buffer,size);
			uLongf lSize = size;
			if (!lSize) return;
			::compress((unsigned char*)buffer,&lSize,(unsigned char*)&content[0],this->size());
			content.resize(lSize);
			bcopy(buffer,&content[0],lSize);
		}
		/**
		 * 解压
		 **/
		void uncompress(char *buffer,unsigned int size)
		{
			bzero(buffer,size);
			uLongf lSize = size;
			::uncompress((unsigned char *)buffer,&lSize,(unsigned char*)&content[0],this->size());
			content.resize(lSize);
			bcopy(buffer,&content[0],lSize);
			//fromBase64(buffer,size);
		}
#endif
#ifdef _USE_BASE64_
		void toBase64()
		{
			std::string temp;
			Base64::encode(&content[0],this->size(),temp);
			content.resize(temp.size() + 1);
			bcopy(temp.c_str(),&content[0],content.size());	
		}
		void fromBase64(char *buffer,unsigned int size)
		{
			size_t tSize = size;
			bzero(buffer,size);
			std::string temp = &content[0];
			Base64::decode(temp,(unsigned char*)buffer,tSize);
			printf("ccc<%d>",tSize);
			content.resize(tSize);
			bcopy(buffer,&content[0],tSize);
		}
#endif
		char NCHAR; // 空符号
		/**
		 * 重载数组访问符号
		 **/
		DYNAMIC_API char & operator[](unsigned int i )
		{
			if (i < content.size())
				return content[i];
			return NCHAR;
		}
		DYNAMIC_API char *c_str()
		{
			if (this->content.empty()) return NULL;
			return &this->content[0];
		}
		DYNAMIC_API unsigned int size(){return (int)content.size();}
		
		/**
		 * 写入文件
		 * \param fileName 文件名
		 **/
		DYNAMIC_API void writeToFile(const char * fileName);
		/**
		 * 从文件中读入
		 * \param fileName 文件名
		 **/
		DYNAMIC_API void readFromFile(const char *fileName);

		/**
		 * 增加一个Store
		 */
		DYNAMIC_API void addStore(Store & store);
		/**
		 * 获取下一个Store
		 */
		DYNAMIC_API bool pickStore(Store &store);

		/**
		 * 增加一段数据
		 * \param data 指针
		 * \param size 数据长度
		 */
		DYNAMIC_API void addData(void *data,int size);
		/**
		 * 获取指定长度的数据
		 * \param data 外部数据缓存
		 * \param size 接受数据的长度
		 * \return 成功true 失败false
		 */
		DYNAMIC_API bool pickData(void *data,int size);
	};
	class Stores;	
	/**
	 * 序列化对象 需要继承之
	 * */
	struct Object{
		/**
		 * 将2进制数据转化为对象
		 * \param content 2进制数据
		 * \param size 数据大小
		 **/
		DYNAMIC_API void parse(void * content,unsigned int size)
		{
			if (!size) return;
			Stream ss(content,size);
			parseRecord(ss);
		}
		/**
		 * 将数据拷贝到content 里,
		 * \param content 缓存
		 * \param size 缓存大小
		 **/
		DYNAMIC_API void store(void *content,unsigned int size)
		{
			Stream ss = toRecord();
			if (ss.size() > size) return;
			bcopy(&ss[0],content,ss.size() < size ? ss.size() : size); 
		}
		/**
		 * 将数据拷贝到content 里,
		 * \param content 缓存 vector
		 **/
		DYNAMIC_API void store(std::vector<char>& content)
		{
			Stream ss = toRecord();
			content.resize(ss.size());
			bcopy(&ss[0],&content[0],ss.size()); 
		}
		/**
		 * 将数据拷贝到content 里,
		 * \param content 缓存 vector
		 **/
		DYNAMIC_API void store(std::vector<unsigned char>& content)
		{
			Stream ss = toRecord();
			content.resize(ss.size());
			bcopy(&ss[0],&content[0],ss.size()); 
		}
		/**
		 * 获取串对象
		 * \return 串
		 **/
		DYNAMIC_API virtual	Stream toRecord()
		{
			Stream ss;
			__store__(ss,__STORE__);
			return ss;
		}
		/**
		 * 将串转化为对象
		 * \param record 串
		 * \return 成功 失败
		 **/
		DYNAMIC_API virtual bool parseRecord(Stream &record)
		{
			if (!record.size()) return false;
			__store__(record,__LOAD__);
			return true;	
		}
		/**
		* 串转为对象 或者对象转为串
		* \param ss in/out 串
		* \param tag __LOAD__ 转化为对象 __STORE__ 转为串
		* 子类需实现
		**/
		DYNAMIC_API virtual void __store__(Stream & ss,unsigned char tag);
		DYNAMIC_API virtual void __v_store__(Stores & sts,unsigned char tag){
			//printf("没有定义初始化函数%x\n",this);
		};
		/**
 		 * 预备加载
 		 * */
		DYNAMIC_API virtual void preLoad(){}
		/**
		 * 预备存储
		 * */
		DYNAMIC_API virtual void preStore(){}	
		/**
 		 * 记载成功
 		 * */
		DYNAMIC_API virtual void beLoaded(){}
		/*
 		 * 存储成功
 		 * */
		DYNAMIC_API virtual void beStored(){}
		/**
		 * 获取对象的类型
		 * 如果对象需要指针持久化的,需要实现该函数
		 * \return 该对象的类型
		 */
		DYNAMIC_API virtual int takeKind() {return -1;}

	};

#define DEC_STORE_CELL_FUNC(__class__,__type__)\
DYNAMIC_API bool addStore(__class__ value,SEARCH_ARG name);\
DYNAMIC_API bool getStore(__class__ &value,SEARCH_ARG name);

#define DO_MAKE_STREAM(__type__)\
DYNAMIC_API void makeStream(__type__& value,Stream& stream)\
{\
	stream = Stream(&value,sizeof(__type__));\
}\
DYNAMIC_API void makeStream(__type__* value,Stream& stream)\
{\
	stream = Stream(value,sizeof(__type__));\
}

#define DO_PARSE_STREAM(__type__)\
DYNAMIC_API void parseStream(__type__& value,Stream& stream)\
{\
	if (stream.size() ==0) return;\
	if (sizeof(__type__) == stream.size())\
	{\
		value = *(__type__*) (&stream.content[0]);\
		return;\
	}\
	switch (stream.size())	\
	{\
		case sizeof(char):\
		{\
			value = (__type__)*(char*) (&stream.content[0]);\
		}break;\
		case sizeof(short):\
		{\
			 value = (__type__)*(short*) (&stream.content[0]);\
		}break;\
		case sizeof(int):\
		{\
			 value = (__type__)*(int*) (&stream.content[0]);\
		}break;\
		case sizeof(long long):\
		{\
			 value = (__type__)*(long long*) (&stream.content[0]);\
		}break;\
	}\
}\
DYNAMIC_API void parseStream(__type__* value,Stream& stream)\
{\
	if (stream.size() ==0) return;\
	if (sizeof(__type__) == stream.size())\
	{\
		value = (__type__*) (&stream.content[0]);\
		return;\
	}\
	switch (stream.size())	\
	{\
		case sizeof(char):\
		{\
			value = (__type__*)(char*) (&stream.content[0]);\
		}break;\
		case sizeof(short):\
		{\
			 value = (__type__*)(short*) (&stream.content[0]);\
		}break;\
		case sizeof(int):\
		{\
			 value = (__type__*)(int*) (&stream.content[0]);\
		}break;\
		case sizeof(long long):\
		{\
			 value = (__type__*)(long long*) (&stream.content[0]);\
		}break;\
	}\
}
	/**
	 * 存储
	 **/
	class Stores{
	public:
		Stores(){
		}
		/**
		 * 转为串对象
		 * \return 串
		 **/
		DYNAMIC_API Stream toRecord();
		
		/**
		 * 转为记录集合
		 * \param record 串
		 **/
		DYNAMIC_API void parseRecord(Stream& record);
	
		/**
		 * 增加一个store 
		 **/
		DYNAMIC_API void addStore(Store& store);
	
		/**
		 * 获取一个store
		 */
		DYNAMIC_API bool pickStore(Store& store,SEARCH_ARG name);
		

		DO_MAKE_STREAM(int);
		DO_MAKE_STREAM(float);
		DO_MAKE_STREAM(unsigned int);
		DO_MAKE_STREAM(double);
		DO_MAKE_STREAM(long long);
		DO_MAKE_STREAM(unsigned long long);
		DO_MAKE_STREAM(char);
		DO_MAKE_STREAM(unsigned char);
		DO_MAKE_STREAM(long);
		DO_MAKE_STREAM(short);
		DO_MAKE_STREAM(unsigned short);
		DO_MAKE_STREAM(unsigned long);
		
		DO_PARSE_STREAM(int);
		DO_PARSE_STREAM(float);
		DO_PARSE_STREAM(unsigned int);
		DO_PARSE_STREAM(double);
		DO_PARSE_STREAM(long long);
		DO_PARSE_STREAM(unsigned long long);
		DO_PARSE_STREAM(char);
		DO_PARSE_STREAM(unsigned char);
		DO_PARSE_STREAM(long);
		DO_PARSE_STREAM(unsigned short);
		DO_PARSE_STREAM(short);
		DO_PARSE_STREAM(unsigned long);
		/**
		 * 将T stream 转化为对象
		 */
		template<typename T>
		void parseStream(T&object,Stream &stream)
		{
			object.parseRecord(stream);
		}
		/**
		 * 将T stream 转化为 对象指针
		 */
		template<typename T>
		void parseStream(T * &object,Stream &stream)
		{
			int kind = 0;
			if (stream.pickData(&kind,sizeof(kind)))
			{
				if (object == NULL)
					object = T::create(kind);
				if (object)
					object->parseRecord(stream);
			}
		}
		/**
		 * 将T 对象转化为stream
		 */
		template<typename T>
		void makeStream(T& object,Stream &stream)
		{
			stream = object.toRecord();
		}
		/**
		 * 将指针对象转化为stream
		 **/
		template<typename T>
		void makeStream(T*& object,Stream &stream)
		{
			// 指针类型将特殊处理内容块
			int kind = object->takeKind();
			stream.addData(&kind,sizeof(kind));
			stream = object->toRecord();
		}
		/**
		 * 特例化string 使string 转化为stream 对象
		 */
		DYNAMIC_API void makeStream(std::string str,Stream &stream);
		/**
		 * 从string 中获取stream
		 */
		DYNAMIC_API void parseStream(std::string & str,Stream &stream);
		
		/**
		 * 持久化string 进入stream
		 */
		DYNAMIC_API void addStore(std::string str,SEARCH_ARG name);

		template<typename T>
		void addStore(T ty,SEARCH_ARG name)
		{
			Stream s;
			makeStream(ty,s);
			Store store (__DATA_OBJECT__,&(s.content[0]),s.size(),name);
			addStore(store);
		}
		/**
		 * 序列化 point 数据 继承于 Object
		 * \param object 数据指针
		 * \param name 记录名字
		 **/
		template <typename T>
		void addStore(T *object,SEARCH_ARG name)
		{
			if (!object) return;
			Stream s;
			makeStream(object,s);
			Store store (__DATA_OBJECT__,&(s.content[0]),s.size(),name);
			addStore(store);
		}
		/**
		 * 持久化非map 容器
		 */
		template<typename T,typename O>
		void doMakeCollectionStream(T objects,O o,Stream& s)
		{
			if (objects.empty()) return;
			std::vector<char> content;
			content.resize(1024);
			DWORD offset = 0;
			for (typename T::iterator iter = objects.begin(); iter != objects.end();++iter)
			{
				O& rO = *iter;
				Stream s;
				makeStream(rO,s);
				DWORD size = s.size();
				if (content.size() < offset + size)
				{
					content.resize(offset + size*2);
				}
				bcopy(&size,&content[offset],sizeof(DWORD));
				bcopy(&s.content[0],&content[offset + sizeof(DWORD)],s.size());
				offset += sizeof(DWORD) + s.size();
			}
			s = Stream(&content[0],offset);
		}
		/**
		 * 增加记录到存储器
		 * \param value std::vector<char>  数值
		 * \param name 记录名字
		 **/
		template <typename T>
		void addStore(std::list<T> objects,SEARCH_ARG name)
		{
			Stream stream;
			doMakeCollectionStream(objects,stream);
			Store store(__DATA_OBJECT_VECTOR__,&stream.content[0],stream.size(),name);
			addStore(store);
		}
		/**
		 * 增加记录到存储器
		 * \param value std::vector<char>  数值
		 * \param name 记录名字
		 **/
		template<typename T>
		void addStore(std::vector<T> objects,SEARCH_ARG name)
		{
			Stream stream;
			T t;
			doMakeCollectionStream(objects,t,stream);
			Store store(__DATA_OBJECT_VECTOR__,&stream.content[0],stream.size(),name);
			addStore(store);
		}

		/**
		 * 序列化 <set> int unsigned short 等基本数据
		 * \param objects 数据集合
		 * \param name 记录名字
		 **/
		template<typename T>
		void addStore(std::set<T> &objects,SEARCH_ARG name)
		{
			Stream stream;
			doMakeCollectionStream(objects,stream);
			Store store(__DATA_OBJECT_VECTOR__,&stream.content[0],stream.size(),name);
			addStore(store);
		}
		/**
		 * 序列化 {name,point} 数据集合 继承于 Object
		 * \param objects 数据集合
		 * \param name 记录名字
		 **/
		template<typename Key,typename Value>
		void addStore(std::map<Key,Value>& objects,SEARCH_ARG name)
		{
			std::vector<char> content;
			content.resize(1024);
			DWORD offset = 0;
			for (typename std::map<Key,Value>::iterator iter = objects.begin(); iter != objects.end();++iter)
			{
				// 记录key的长度 DWORD
				// 记录key的值 size
				// 记录value的长度 DWORD 
				// 记录value的值 size
				Key key = iter->first;
				Stream ss;
				makeStream(key,ss);
				DWORD keySize = ss.size();
				Value  value = iter->second;
				Stream s; 
				makeStream(value,s);
				DWORD valueSize = s.size(); 
				if (content.size() < offset + keySize)
				{
					content.resize(offset + keySize * 2 + valueSize + sizeof(DWORD)*2);
				}

				bcopy(&keySize,&content[offset],ss.size());
				offset += sizeof(DWORD);

				bcopy(&ss.content[0],&content[offset],keySize);
				offset += keySize;
				
				bcopy(&valueSize,&content[offset],sizeof(DWORD));
				offset += sizeof(DWORD);
				if (content.size() < offset + valueSize)
				{
					content.resize(offset + valueSize * 2 + sizeof(DWORD) * 2);
				}

				bcopy(&s.content[0],&content[offset],s.size());
				offset += s.size();
			}
			Store store(__DATA_OBJECT_MAP__,&content[0],offset,name);
			addStore(store);
		}
		/**
		 * 获取 {name,point} 数据集合 继承于 Object
		 * \param objects 数据集合
		 * \param name 记录名字
		 **/
		template<typename Key,typename Value>
		bool getStore(std::map<Key,Value> &objects,SEARCH_ARG name)
		{
			Store nowstore;
			pickStore(nowstore,name);
			if (nowstore.equal(name))
			{
				Store *store = &nowstore;
				switch( store->coreData.tag)
				{
					case __DATA_OBJECT_MAP__:
					{
						DWORD offset = 0;
						if (store->coreData.size != store->content.size()) return false;
						while(offset < store->coreData.size )
						{
							DWORD keySize = *(DWORD*)&store->content[offset];
							if (keySize > store->coreData.size)
							{
								printf("[find an error %s:%u,%u]\n",__FILE__,__LINE__,keySize);
								return false;
							}
							
							Stream skey;
							skey.content.resize(keySize);
							bcopy(&store->content[offset+sizeof(DWORD)],&skey.content[0],keySize);
							Key key;
							parseStream(key,skey);
							offset += sizeof(DWORD) + keySize;

							DWORD valueSize = 0;
							bcopy(&store->content[offset],&valueSize,sizeof(DWORD));
							offset += sizeof(DWORD);
							if (valueSize > store->coreData.size)
							{
								return false;
							}	
							Stream sr;
							sr.content.resize(valueSize);
							bcopy(&store->content[offset],&sr.content[0],valueSize);
							Value object;
							parseStream(object,sr);
							objects[key] = (object);
							offset += valueSize;
						}
						return true;
					}break;
				}

			}
			return false;
		}
		/**
		 * 获取 {name,point} 数据集合 继承于 Object
		 * \param objects 数据集合
		 * \param name 记录名字
		 **/
		template<class T>
		bool getStore(std::set<T>& objects,SEARCH_ARG name)
		{
			Store nowstore;
			pickStore(nowstore,name);
			if (nowstore.equal(name))
			{
				Store *store = &nowstore;
				switch( store->coreData.tag)
				{
					case __DATA_OBJECT_VECTOR__:
					{
						DWORD offset = 0;
						if (store->coreData.size != store->content.size()) return false;
						while(offset < store->coreData.size )
						{
							DWORD size = *(DWORD*)&store->content[offset];
							if (size > store->coreData.size)
							{
								printf("[find an error %s:%u, headerSize:%u,coreSize:%u]\n",__FILE__,__LINE__,size,store->coreData.size);
								break;
							}
							Stream sR;
							sR.content.resize(size);
							bcopy(&store->content[offset+sizeof(DWORD)],&sR.content[0],size);
							T object;
							parseStream(object,sR);
							objects.insert(object);
							offset += sizeof(DWORD) + size;
						}
						return true;
					}break;
				}
			}
			return false;
		}

		/**
		 * 获取 {name,point} 数据集合 继承于 Object
		 * \param objects 数据集合
		 * \param name 记录名字
		 **/
		template<class T>
		bool getStore(std::list<T>& objects,SEARCH_ARG name)
		{
			Store nowstore;
			pickStore(nowstore,name);
			if (nowstore.equal(name))
			{
				Store *store = &nowstore;
				switch( store->coreData.tag)
				{
					case __DATA_OBJECT_VECTOR__:
					{
						DWORD offset = 0;
						if (store->coreData.size != store->content.size()) return false;
						while(offset < store->coreData.size )
						{
							DWORD size = *(DWORD*)&store->content[offset];
							if (size > store->coreData.size)
							{
								printf("[find an error %s:%u,%u]\n",__FILE__,__LINE__,size);
								break;
							}
							Stream sR;
							sR.content.resize(size);
							bcopy(&store->content[offset+sizeof(DWORD)],&sR.content[0],size);
							T object;
							parseStream(object,sR);
							objects.push_back(object);
							offset += sizeof(DWORD) + size;
						}
						return true;
					}break;
				}
			}
			return false;
		}
		
		/**
		 * 获取 {name,point} 数据集合 继承于 Object
		 * \param objects 数据集合
		 * \param name 记录名字
		 **/
		template<class T>
		bool getStore(std::vector<T>& objects,SEARCH_ARG name)
		{
			Store nowstore;
			pickStore(nowstore,name);
			if (nowstore.equal(name))
			{
				Store *store = &nowstore;
				switch( store->coreData.tag)
				{
					case __DATA_OBJECT_VECTOR__:
					{
						DWORD offset = 0;
						if (store->coreData.size != store->content.size()) return false;
						while(offset < store->coreData.size )
						{
							DWORD size = *(DWORD*)&store->content[offset];
							if (size > store->coreData.size)
							{
								printf("[find an error %s:%u,%u]\n",__FILE__,__LINE__,size);
								break;
							}
							Stream sR;
							sR.content.resize(size);
							bcopy(&store->content[offset+sizeof(DWORD)],&sR.content[0],size);
							T object;
							parseStream(object,sR);
							objects.push_back(object);
							offset += sizeof(DWORD) + size;
						}
						return true;
					}break;
				}
			}
			return false;
		}

		/**
		 * 获取 point 数据 继承于 Object
		 * \param object 数据
		 * \param name 记录名字
		 **/
		template <typename T>
		bool getStore(T* &object,SEARCH_ARG name)
		{
		//	printf("[stores] store object %s\n",name.c_str());
			Store nowstore;
			pickStore(nowstore,name);
			if (nowstore.equal(name))
			{
				Store *store = &nowstore;
				if (store->content.empty()) return false;
				serialize::Stream ss(&store->content[0],store->coreData.size);
				parseStream(object,ss);
			}
			return false;
		}

		/**
		 * 获取基本类型数据
		 * \param name 记录名字
		 * \param t 记录
		 **/
		template <class T>
		bool getStore(T &t,SEARCH_ARG name)
		{
			Store nowstore;
			pickStore(nowstore,name);
			if (nowstore.equal(name))
			{
				Store *store = &nowstore;
				if (store->content.empty()) return false;
				Stream ss(&store->content[0],store->coreData.size);
				parseStream(t,ss);
				return true;
			}
			return false;
		}

		DEC_STORE_CELL_FUNC(int,__DATA__INT__);
		DEC_STORE_CELL_FUNC(unsigned short,__DATA__INT__);
		DEC_STORE_CELL_FUNC(float,__DATA_FLOAT__);
		DEC_STORE_CELL_FUNC(unsigned int ,__DATA_UNSIGNED_INT__);
		DEC_STORE_CELL_FUNC(char,__DATA_CHAR__);
		DEC_STORE_CELL_FUNC(long,__DATA_LONG__);
		DEC_STORE_CELL_FUNC(unsigned char,__DATA_UNSINGED_CHAR__);
		DEC_STORE_CELL_FUNC(unsigned long,__DATA_UNSINGED_LONG__);
		DEC_STORE_CELL_FUNC(long long,__DATA_LONG_LONG__);
		DEC_STORE_CELL_FUNC(unsigned long long,__DATA_UNSINGED_LONG_LONG__);
		/**
		 * 增加一个store
		 * \param value 内容起始地址
		 * \param size 内容长度
		 * \param name 引索号
		 */
		bool addStore(const void *value,unsigned int size,SEARCH_ARG name);
			
	private:
#if (0)
		Stream stream; // 内置的stream 此版不用
#endif	
		std::vector<Store> stores; // 解析暂用的store 存储
	};
};
