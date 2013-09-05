#pragma once
/**
 * email:jjl_2009_hi@163.com
 * Author jijinlong
 * Data 2011.10.14~2012.3.18~~
 * All Rights Resvered
 ***/

#include "sys.h"
#include "stdio.h"
#include "vector"
#include "stdlib.h"
#include "memory.h"
#include "string"
using namespace serialize;
namespace sys{
	/*
	 * 判断两个字符串是否相等
	 **/
	struct EqualStr{
		bool operator()(const char*s1,const char *s2)const
		{
			return (s1 == s2) || (s1 && s2 && strcmp(s1, s2) == 0);
		}
	};
	/*
 	 * 计算hash 值
	 **/
	struct HashStr{
		static int calc(const char* str) ;
		int operator()(const char *str) const
		{
			return calc(str);
		}
	};
	/*
	 * 封装字符串操作
	 **/
	struct sString{
		/**
 		 * 字符串默认构造函数
 		 * */
		DYNAMIC_API sString(){}
		/**
 		 * 使用字符串构造
 		 * \param content 字符串
 		 * */
		DYNAMIC_API sString(const char*content);
		/*
 		 *  使用string 构造
 		 *  \param content stl::string
 		 * **/
		DYNAMIC_API sString(std::string& content);
		/*
 		 * 转为GB2312 编码
 		 * **/
		DYNAMIC_API sString toGB2312();
				/*
 		 * 将[text] jjj [\text] 转化为 <text> jjj </text>
 		 * \return 新的字符串
 		 * **/
		DYNAMIC_API sString toXmlString();
		
		/*
 		 * 分割字符串 
 		 * \param str 分隔符"*&&^"
 		 * \param strs 子字符串
 		 * **/
		DYNAMIC_API void split(const char *str,std::vector<std::string>& strs);
		/*
 		 * 头尾分隔符 去掉
 		 * */
		DYNAMIC_API void trim();
		
		/*
 		 * 将内容拷贝到buf中
 		 * \param buf 目的缓存
 		 * \param size 缓存大小
 		 * \return 拷贝的大小
 		 * **/
		DYNAMIC_API unsigned int toBinary(void * buf,unsigned int size);
		
		/*
 		 * 返回字符串大小
 		 * **/
		DYNAMIC_API unsigned int size();
		
		/*
 		 * 转为为std::string
 		 * \return stl 标准子串
 		 * **/
		DYNAMIC_API std::string toString();
		
		/*
 		 * 输出二进制数据
 		 * \param value 二进制数据
 		 * \param size 数据长度
 		 * **/
		DYNAMIC_API static void printBinary(const char *value,int size);
		
		/**
 	 	 * 构造字符串
 	 	 * \param content 子串
 	 	 * \param size 大小
 	 	 * */
		DYNAMIC_API sString(const char *content,int size);
		
		/**
 		 * 重载 运算符
 		 * */
		DYNAMIC_API sString &operator = (const sString &content);
		DYNAMIC_API bool operator == (const sString &content);
		DYNAMIC_API bool operator != (const sString &content);
		DYNAMIC_API sString operator +(const sString &content);
		DYNAMIC_API sString& operator += (const sString &content);
		
		/*
 	 	 * 函数字符串指针
 	 	 * **/
		DYNAMIC_API const char *str();
		
		/**
 		 * 尾部 增加 字符
 		 * \param c 字符
 		 * */
		DYNAMIC_API void append(char c);
		
		/**
		 * 尾部 增加字符串
		 * \param str 字符串
		 * */
		DYNAMIC_API void append(const char *str);
		
		/**
 		 * 清除
 		 * */
		DYNAMIC_API void clear();
		
		std::vector<char> contents; // 内容
		int hash_code; // hash 值
		
		/**
 		 * \param s1 原字符串
 		 * \param s2 目的字符串
 		 * \return 是否相等
 		 * */
		DYNAMIC_API static bool cmp(const sString & s1,const sString &s2 ) ;
		
		/**
 		 * 计算字符串长度
 		 * \param str 字符串
 		 * \return 字符串长度
 		 * */
		DYNAMIC_API static int mstrlen(const char *str);
		
		/**
 		 * 计算 字符串长度
 		 * \param str 字符串
 		 * \param size 最大字符长度
 		 * \return 实际字符串长度
 		 * */
		DYNAMIC_API static int mstrnlen(const char *str,int size);
		
		/**
 		 * 比较 与 字符串 是否相等  
 		 * \param str 字符串
 		 * \return 是否相等
 		 * */
		DYNAMIC_API bool equal(const char *str);
		DYNAMIC_API bool parseRecord(Stream &record);
		DYNAMIC_API Stream toRecord();
		DYNAMIC_API bool operator < (const sString &c) const
		{
			if (c.contents.size() > contents.size()) return false;
			return -1 == strcmp(&c.contents[0],&contents[0]);
		}
		DYNAMIC_API bool operator !=(const sString &i) const
		{
			return 0 != strcmp(&i.contents[0],&contents[0]);

		}
	
		DYNAMIC_API static void split(const char * string,std::vector<std::string>& strings,const char* tag=" ")
		{
			char * buf = new char [strnlen(string,sizeof(short) * 1024)];
            strncpy(buf,string,sizeof(short) * 1024);
            char *nexttoken =NULL;
			char *token = NULL;
			token = strtok_s(buf,tag,&nexttoken);
			while ( token != NULL)
			{
				strings.push_back(token);
               // printf("<%s>",token);
				token = strtok_s(nexttoken,tag,&nexttoken);
			}
            delete [] buf;
		}
        DYNAMIC_API static void split(char * string,std::vector<char*>& strings,const char* tag=" ")
		{
            char *token = NULL;
			 char *nexttoken =NULL;
			token = strtok_s(string,tag,&nexttoken);
			while ( token != NULL)
			{
				strings.push_back(token);
				token = strtok_s(nexttoken,tag,&nexttoken);
			}
		}

		DYNAMIC_API static const char*replace(const char *str,const char src,const char dest)
		{
			char *temp = new char[strnlen(str,1024)];
			strncpy(temp,str,1024);
			char *temp1 = temp;
			while (*temp1)
			{
				if (*temp1 == src)
					*temp1 = dest;
				temp1++;
			}
			return temp;
		} 
	};
}; // end namespace sys
