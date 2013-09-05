/**
 * email:jjl_2009_hi@163.com
 * Author jijinlong
 * Data 2011.10.14~2012.3.18~~
 * All Rights Resvered
 ***/
#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <map>
#include "tinyxml.h"
#include <vector>
namespace script{
	/**
 	 * 读取属性值
 	 * */
	class xmlNodeAttr{
	public:
		/**
 		 * 设置数据
 		 * \param ext 第三方数据
 		 * */
		virtual void setExtData(void * ext);
		/*
 		 * 获取第3方数据
 		 * \return 第3方数据指针
 		 * **/
		virtual void*getExtData();

		void *extData; // 附加数据
		std::map<std::string,std::string> attrs; // 属性列表
		typedef std::map<std::string,std::string>::iterator ATTR_ITER;	
		/**
 		 * 判断当前节点 名字
 		 * \param name 名字
 		 * \return 相等true 不等false
 		 * */
		bool equal(const char *name);
		
		/**
 		 * 判断当前节点 
 		 * \param str 名字
 		 * \return 相等与否
 		 * */
		bool equal(std::string str);
		
		/**
 		 * 获取当前节点
 		 * \param name 名字
 		 * \param value out float值
 		 * \return 存在与否
 		 * */
		virtual bool getAttr(const char *name,float& value);
		/**
 		 * 获取当前节点
 		 * \param name 名字
 		 * \param value out int值
 		 * \return 存在与否
 		 * */
        template<class T>
        bool getAttr(const char *name,T& value)
        {
            ATTR_ITER iter = attrs.find(name);
            if (iter!=attrs.end())
            {
                value = atoi(iter->second.c_str());
                return true;
            }
            return false;
        }
		bool getAttr(const char *name,std::string &contnt);
		float getFloat(const char *name);
		int getInt(const char *name);
		/**
 		 * 获取当前节点
 		 * \param name 名字
 		 * \param value out 字符串缓存 size 缓存大小 
 		 * \return 存在与否
 		 * */

		virtual bool getAttr(const char *name,char *value,unsigned int size);
		/**
 		 * 获取当前节点名字
 		 * \param name 属性名字
 		 * \return 属性值
 		 * */
		virtual const char *getAttr(const char *name);
		std::string nodeName; // 当前节点
	};
///////////////////tinyxml/////////////////////////	
	
	/*
	 * tinyxml Node 节点
	 **/
	class tixmlCodeNode:public xmlNodeAttr{
	public:
		tixmlCodeNode();
		~tixmlCodeNode();
		/**
 		 *\return  节点名字
 		 * */
		const char*name();
		/**
 		 * 获取子节点的下一个节点
 		 * \return 子节点下一个节点
 		 * */
		virtual tixmlCodeNode *childnext();
		/**
 		 * 子节点集合
 		 * */
		std::vector<tixmlCodeNode*> childs;
		typedef std::vector<tixmlCodeNode*>::iterator CHILDS_ITER;
		TiXmlElement * element; // tixml 的节点
		/**
		 * 是否有效
		 */
		bool isValid();
		/**
		 * 获取首个名字为name 的节点
		 */
		tixmlCodeNode getFirstChildNode(const char *name);
		/**
		 * 获取下一个名字为name的节点
		 */
		tixmlCodeNode getNextNode(const char *name);
		/**
		 * 初始化 
		 * \param element 节点内容
		 * \return 成功与否
		 * */
		bool init(TiXmlElement * element);
		unsigned int nowChildIndex; // 当前子节点序号	
		tixmlCodeNode *next; // 当前下一个节点 
		tixmlCodeNode *parent; // 当前父亲节点
		/**
 		 * 返回首个节点
 		 * */
		tixmlCodeNode *firstChild();
		/**
 		 * 设置节点文本内容
 		 * <XXX>text</XXX>
 		 * 设置的text 是这里的text XXX 为节点
 		 * */
		void setText(std::string & text);
		/**
 		 * 释放资源
 		 * \return 成功 与否
 		 * */
		bool release();
		std::string text; // 文本内容
        /**
         * 打印出执行树
         */
        void traceInfo();
		/**
		 * 赋值运算
		 */
		tixmlCodeNode & operator = (const tixmlCodeNode & node);
	};
	/*
	 * 遍历配置文件得到节点
	 **/
	 class tixmlCode{
	 public:
		/**
 		 * 初始化
 		 * \param filename 文件名
 		 * \return 成功与否	
 		 * */
		virtual bool init(const char *fileName);
		/**
		 * 从字符串中初始化
		 */
		bool initFromString(const char *content);
		/**
 		 * 获取<XXXX> 节点 并解析
 		 * \param node 节点
 		 * */
		virtual void takeNode(tixmlCodeNode *node){};
		/**
 		 * 获取<code> 节点 并解析
 		 * \param code 脚本节点
 		 * **/
		virtual void parseCode(tixmlCodeNode *code){};
		/**
 		 * 释放资源
 		 * */
		virtual bool release(){return false;}
		/**
 		 * 遍历code  节点 获取执行树
 		 * \parma root NULL 
 		 * \param node 当前<code>节点
 		 * \return code
 		 * */
		tixmlCodeNode * traceNode1(tixmlCodeNode *root,TiXmlNode *node);
		/**
 		 * 获取节点的行为
 		 * \param name 节点名字
 		 * \return 行为
 		 * */
		virtual void * getAction(const char *name){return NULL;};
	};
	
};
