#pragma once
#include "sys.h"
#include "serialize.h"
#include <vector>
namespace script{
	/*
  	 * 变量设计
 	 **/
	struct Var:public serialize::Object{
		/**
 		 * 判断数字
 		 * \param value 字符串
 		 * \return 是true 否false
 		 * */
		static bool judgeNumber(const char *value);
		/**
 		 * 定义持久化
 		 * */
		DEC_SERIALIZE_DATA;

		std::string value; // 值
		std::string name; // 名字
		sys::stTimeValue useTime; // 使用时间
		DWORD lastTime;// 持续时间
		BYTE type; //实效性
		enum{
			FOROVER = 0, // 外部控制
			WEEK, // 每周切换
			DAY, // 每天切换
			MONTH, // 每月切换
            YEAR, // 每年切换
		};
		/**
		 * 返回数字
		 * \return 数字
		 **/
		double number();
		/**
 		 * 返回变量值
		 */
		std::string str();
		/**
 		 * 重载赋值符号
 		 * \param var 变量
 		 * \return 变量
 		 * */
		Var & operator = (const Var &var);
		/**
 		 * 判断变量是否是数字
 		 * \return 是数字  true 不是数字false
 		 * */
		bool isNumber();
		/**
 		 * 判断与 字符串是否相等
 		 * \return 相等true 不等false
 		 * */	
		bool equal(const char *value);
		/**
 		 * 当前值 加上 value
 		 * \param value 字符串
 		 * */
		void add(const char *value);
		/**
 		 * 当前值 减去 value
 		 * \param value 减少的值
 		 * */
		void minus(const char *value);
		
		void div(const char *value);
		void mul(const  char *value);
		bool operator < (const char *value);
		bool operator > (const char *value);
		bool operator <= (const char *value);
		bool operator >=(const char *value);
		bool operator == (const char *value);
		bool operator != (const char *value);
		Var & operator = (const char *value);
		/**
		 * 变量的处理
 		 * */	
		/**
 		 * 判断与 字符串是否相等
 		 * \return 相等true 不等false
 		 * */	
		bool equal(Var &var);
		/**
 		 * 当前值 加上 value
 		 * \param value 字符串
 		 * */
		void add(Var &var)
		{
			add(var.value.c_str());
		}
		/**
 		 * 当前值 减去 value
 		 * \param value 减少的值
 		 * */
		void minus(Var &var)
		{
			minus(var.value.c_str());
		}
		/**
 		 * 除以传入变量
 		 * **/	
		void div(Var &var)
		{
			div(var.value.c_str());
		}
		/**
 		 * 乘以传入变量
 		 * */
		void mul(Var &var)
		{
			mul(var.value.c_str());
		}
		bool operator < (const Var &value)
		{
			return less(value.value.c_str());
		}
		bool operator > (const Var &value)
		{
			return great(value.value.c_str());
		}
		bool operator <= (const Var &value)
		{
			return lessequal(value.value.c_str());
		}
		bool operator >=(const Var &value)
		{
			return greatequal(value.value.c_str());
		}
		bool operator == (const Var &value)
		{
			return equal(value.value.c_str());
		}
		bool operator != (const Var &value)
		{
			return !equal(value.value.c_str());
		}

		bool less(const char *value);
		bool great(const char *value);
		bool lessequal(const char *value);
		bool greatequal(const char *value);
		
		/**
 		 * 设置 
 		 * \param value 值
 		 * */
		void set(const char *value);
		template<class T>
		void set(T value)
		{
			std::stringstream ss;
			ss << value;
			this->value = ss.str();
		}	
		Var(const char*name,unsigned int value);
		Var(const char *name,int value);
		Var(const char *value);
		Var();
		/**
 		 * 设置变量的类型
 		 * \param type 时间类型
 		 * */
		void init(BYTE type);
		/**
 		 * 超时检查
 		 * \return 有效 true 过期false
 		 * */
		bool checkValid();
	};
};	
