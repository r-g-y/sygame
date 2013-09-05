#pragma once
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <list>
#include <set>
#include <map>
//#include "sys.h"
/**
 * 可以发送容器类型的消息体
 * 对于消息体中的指针 外部不可以存储
 * */
#define SERIALIZE_CMD() \
	virtual void __to_msg__record__(unsigned char tag,cmd::Stream &ss)	
#define CMD_SERIALIZE()\
	virtual void __to_msg__record__(unsigned char tag,cmd::Stream &ss)
#define CALL_CMD_SERIALIZE(__parent__class__)\
    __parent__class__::__to_msg__record__(tag,ss)
#define BIND(value)\
	switch(tag)\
	{\
		case cmd::GET:\
		{\
			getRecord(value,ss);	\
		}break;\
		case cmd::ADD:\
		{\
			addRecord(value,ss);\
		}break;\
		case cmd::CLEAR:\
		{\
			clearRecord(value);\
		}break;\
	}
#define BIND_SIZE(value,size)\
	switch(tag)\
	{\
		case cmd::GET:\
		{\
			getRecord(value,size,ss);	\
		}break;\
		case cmd::ADD:\
		{\
			addRecord(value,size,ss);\
		}break;\
		case cmd::CLEAR:\
		{\
			clearRecord(value,size);\
		}break;\
	}

namespace cmd{
	typedef short BodySize_t;
	struct Stream{
		void set(void *content,int size)
		{
			_offset = 0;
			if (!size) return;
			contents.resize(size);
			memcpy(&contents[_offset],content,size);
		}
		Stream(){
			_offset = 0;
			_size = 0;
		}
		void append(const void *content,unsigned int size)
		{
			if (size == 0) return;
			if (contents.size() - _offset < size)
			{
				contents.resize(_offset + size);
			}
			memcpy(&contents[_offset],content,size);
			_offset += size;
			_size = _offset;
		}
		bool pick(void *content,unsigned int size)
		{
			if (contents.size() - _offset < size)
			{
				return false;
			}
			memcpy(content,&contents[_offset],size);
			_offset += size;
			return true;
		}
		void reset(){_offset = 0;}
		std::vector<unsigned char> contents;
		unsigned int size(){return _size;}
		void * content(){return &contents[0];}
	private:
		int _offset;
		int _size;
	};
	const int GET = 0;
	const int ADD = 1;
	const int CLEAR = 2;
#define DEC_OBJECT_RECORD(container,insert_func)\
	template<class T>\
	void addRecord(typename container<T> &objects,Stream &ss)\
	{\
		int size = objects.size();\
		ss.append(&size,sizeof(int));\
		for (typename container<T>::iterator iter = objects.begin(); iter != objects.end();++iter)\
		{\
			iter->__to_msg__record__(ADD,ss);\
		}\
	}\
	template<class T>\
	void getRecord(typename container<T> &objects,Stream &ss)\
	{\
		int size;\
		if (ss.pick(&size,sizeof(int)))\
		{\
			while ( size-- > 0)\
			{\
				T value;\
				value.__to_msg__record__(GET,ss);\
				objects.insert_func(value);\
			}\
		}	\
	}\
	template<class T>\
	void clearRecord(typename container<T> &objects)\
	{\
		objects.clear();\
	}\
    template<class T>\
	void addRecord(typename container<T*> &objects,Stream &ss)\
	{\
		int size = objects.size();\
		ss.append(&size,sizeof(int));\
		for (typename container<T*>::iterator iter = objects.begin(); iter != objects.end();++iter)\
		{\
            if (!*iter) continue;\
            int kind = (*iter)->kind; \
            ss.append(&kind,sizeof(int));\
			(*iter)->__to_msg__record__(ADD,ss);\
		}\
	}\
	template<class T>\
	void getRecord(typename container<T*> &objects,Stream &ss)\
	{\
		int size;\
		if (ss.pick(&size,sizeof(int)))\
		{\
			while ( size-- > 0)\
			{\
                int kind = 0; \
                if (ss.pick(&kind,sizeof(int))){\
				T *value = T::create(kind);/**消息体内的指针需要自己释放*/\
				value->__to_msg__record__(GET,ss);\
				objects.insert_func(value);}\
			}\
		}	\
	}\
	template<class T>\
	void clearRecord(typename container<T*> &objects)\
	{\
        for (typename container<T*>::iterator iter = objects.begin(); iter != objects.end();++iter)\
		{\
            if (*iter) delete *iter;\
		}\
		objects.clear();\
	}

#define DEC_TYPE_RECORD(container,insert_func,type)\
	void addRecord(container<type> &objects,Stream &ss)\
	{\
		int size = objects.size();\
		ss.append(&size,sizeof(int));\
		for (container<type>::iterator iter = objects.begin(); iter != objects.end();++iter)\
		{\
			type value = *iter;\
			addRecord(value,ss);\
		}\
	}\
	void getRecord(container<type> &objects,Stream &ss)\
	{\
		int size;\
		if (ss.pick(&size,sizeof(int)))\
		{\
			while ( size-- > 0)\
			{\
				type value;\
				getRecord(value,ss);\
				objects.insert_func(value);\
			}\
		}	\
	}\
	void clearRecord(container<type> &objects)\
	{\
		objects.clear();\
	}

	class Object{
	public:
		DEC_OBJECT_RECORD(std::vector,push_back);
		DEC_OBJECT_RECORD(std::list,push_back);
		DEC_OBJECT_RECORD(std::set,insert);
		DEC_TYPE_RECORD(std::vector,push_back,int);
		DEC_TYPE_RECORD(std::list,push_back,int);
		DEC_TYPE_RECORD(std::set,insert,int);
		DEC_TYPE_RECORD(std::vector,push_back,char);

		template <class T>
		void addRecord(T& value,Stream &ss)
		{
			ss.append(&value,sizeof(T));
		}
		template<class T>
		void getRecord(T &value,Stream &ss)
		{
			ss.pick(&value,sizeof(T));
		}
		template<class T>
		void clearRecord(T &value)
		{
			memset(&value,0,sizeof(T));
		}

		void addRecord(std::string &value,Stream& ss)
		{
			BodySize_t size = value.size();
			ss.append(&size,sizeof(BodySize_t));
			ss.append((void*)value.c_str(),size);
		}
		void addRecord(const void *value,int size,Stream &ss)
		{
			ss.append((void*)value,size);
		}
		void getRecord(void *value,int size,Stream &ss)
		{
			ss.pick(value,size);
		}
		void clearRecord(void * value,int size)
		{
			memset(value,0,size);
		}
		void addRecord(const std::basic_string<char> &value,Stream &ss)
		{
			BodySize_t size = value.size();
			ss.append(&size,sizeof(BodySize_t));
			ss.append((void*)value.c_str(),size);
		}
		void getRecord(std::basic_string<char> &value,Stream &ss)
		{
			BodySize_t size = 0;
			if (ss.pick(&size,sizeof(BodySize_t)))
			{
				if (size > 0)
				{
					char *str = new char [size +1];
					memset(str,0,size+1);
					if (ss.pick(str,size))
						value = str;
					delete[] str;
				}
			}
		}
		void clearRecord(std::string &value)
		{
			value = "";
		}
		void addRecord(std::vector<std::string> &value,Stream &ss)
		{
			BodySize_t size = value.size();
			ss.append(&size,sizeof(BodySize_t));
			for (std::vector<std::string>::iterator iter = value.begin(); iter != value.end();++iter)
			{
				addRecord(*iter,ss);
			}
		}
		void getRecord(std::vector<std::string> &value,Stream &ss)
		{
			BodySize_t size = 0;
			if (ss.pick(&size,sizeof(BodySize_t)))
			{
				while (size -- >0)
				{
					std::string str;
					getRecord(str,ss);
					value.push_back(str);
				}
			}	
		}
		void clearRecord(std::vector<std::string> &value)
		{
			value.clear();
		}

		template<class KEY,class VALUE>
		void addRecord(typename std::map<KEY,VALUE> &value,Stream &ss)
		{
			BodySize_t size = value.size();
			ss.append(&size,sizeof(BodySize_t));
			for (typename std::map<KEY,VALUE>::iterator iter = value.begin(); iter != value.end();++iter)
			{
				addRecord(iter->first,ss);
				addRecord(iter->second,ss);
			}
		}
		template<class KEY,class VALUE>
		void getRecord(typename std::map<KEY,VALUE> &values,Stream &ss)
		{
			BodySize_t size = 0;
			if (ss.pick(&size,sizeof(BodySize_t)))
			{
				while (size -- >0)
				{
					KEY key;
					VALUE value;
					getRecord(key,ss);
					getRecord(value,ss);	
					values[key] = value;
				}
			}	
		}
		template<class KEY,class VALUE>
		void clearRecord(typename std::map<KEY,VALUE> &values)
		{
			values.clear();
		}

	public:
		Stream toStream()
		{
			Stream ss;
			__to_msg__record__(ADD,ss);
			ss.reset();
			return ss;
		}
		void fromStream(void * content,int size)
		{
			if (size == 0) return;
			Stream ss;
			ss.set(content,size);
			fromStream(ss);
		}
		void fromStream(Stream& ss)
		{
			ss.reset();
			__to_msg__record__(GET,ss);
			ss.reset();
		}
		void clearContent()
		{
			Stream ss;	
			__to_msg__record__(CLEAR,ss);
		}
	protected:
		virtual void __to_msg__record__(unsigned char tag,Stream& ss)
    	{
    	};
	};
};
