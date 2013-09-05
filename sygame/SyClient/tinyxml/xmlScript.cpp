/**
 * email:jjl_2009_hi@163.com
 * Author jijinlong
 * Data 2011.10.14~2012.3.18~~
 * All Rights Resvered
 ***/
#include "xmlScript.h"
#include "iostream"
#include "string"
#include "string.h"
namespace script{
	void xmlNodeAttr::setExtData(void * ext)
	{
		extData = ext;
	}
	void*xmlNodeAttr::getExtData()
	{
		return extData;
	}
	
	bool xmlNodeAttr::equal(const char *name)
	{
		std::string t(name);
		return t == this->nodeName;
	}
	bool xmlNodeAttr::equal(std::string str)
	{
		std::string t(str);
		return t == nodeName;
	}
	
	bool xmlNodeAttr::getAttr(const char *name,float& value)
	{
		ATTR_ITER iter = attrs.find(name);
		if (iter!=attrs.end())
		{
			value = atof(iter->second.c_str());
			return true;
		}
		return false;
	}
	bool xmlNodeAttr::getAttr(const char *name,std::string &co)
	{
		ATTR_ITER iter = attrs.find(name);
		if (iter!=attrs.end())
		{
			co= iter->second.c_str();
		}
		return false;
	}
	float xmlNodeAttr::getFloat(const char *name)
	{
		ATTR_ITER iter = attrs.find(name);
		if (iter!=attrs.end())
		{
			return atof(iter->second.c_str());
		}
		return 0;
	}

	int xmlNodeAttr::getInt(const char *name)
	{
		ATTR_ITER iter = attrs.find(name);
		if (iter!=attrs.end())
		{
			return atoi(iter->second.c_str());
		}
		return 0;
	}
	const char *xmlNodeAttr::getAttr(const char *name)
	{
		ATTR_ITER iter = attrs.find(name);
		if (iter!=attrs.end())
		{
			return iter->second.c_str();
		}
		return "";
	}
	bool xmlNodeAttr::getAttr(const char *name,char *value,unsigned int size)
	{
		ATTR_ITER iter = attrs.find(name);
		if (iter!=attrs.end())
		{
			memcpy(value,iter->second.c_str(),size);
			return true;
		}
		return false;
	}
////////////////////////tinyxml///////////////////////////////////////	
	const char * tixmlCodeNode::name()
	{
		return nodeName.c_str();		
	}
	bool tixmlCodeNode::init(TiXmlElement *node)
	{
		if (!node) return false;
		element = node;
		attrs.clear();
		TiXmlAttribute* attr = node->FirstAttribute();
		nodeName = std::string(node->Value());
		// fill attribute
		while (attr != 0)
		{
			attrs[std::string(attr->Name())] = attr->Value();
			attr = attr->Next();
		}
		return true;
	}
	/**
	 * 是否有效
	 */
	bool tixmlCodeNode::isValid()
	{
		return NULL != element;
	}
	/**
	 * 获取首个名字为name 的节点
	 */
	tixmlCodeNode tixmlCodeNode::getFirstChildNode(const char *name)
	{
		tixmlCodeNode node;
		if (element)
		{
			TiXmlElement * element1 = element->FirstChildElement(name);
			node.init(element1);
		}
		return node;
	}
	tixmlCodeNode & tixmlCodeNode::operator = (const tixmlCodeNode & node)
	{
		this->element = node.element;
		init(this->element);
		return *this;
	}
	/**
	 * 获取下一个名字为name的节点
	 */
	tixmlCodeNode tixmlCodeNode::getNextNode(const char *name)
	{
		tixmlCodeNode node;
		if (element)
		{
			TiXmlElement * element1 = element->NextSiblingElement(name);
			node.init(element1);
		}
		return node;
	}
	// 优化处理 ,指针一次分配 在资源释放的时候回收	
	tixmlCodeNode *tixmlCodeNode::childnext()
	{
		if (nowChildIndex < childs.size()) return childs[nowChildIndex++];
		nowChildIndex = 0;
		return NULL;
	}	
	tixmlCodeNode * tixmlCodeNode::firstChild()
	{
		if (childs.size()) return childs[0];
		return NULL;
	}	
	bool tixmlCode::init(const char *fileName)
	{
		TiXmlDocument doc(fileName);
		if (!doc.LoadFile()) return false;
		static std::string CODE="code";
		static std::string READ="read";
		TiXmlHandle hDoc(&doc);
		TiXmlElement* pElem = NULL;
		pElem=hDoc.FirstChildElement().Element();
		for(; pElem; pElem=pElem->NextSiblingElement())
		{
			const char *pKey=pElem->Value();
			tixmlCodeNode *node = new tixmlCodeNode();
			node->init(pElem);
			if (node->nodeName == CODE)
			{
				parseCode(node);
			}
			else if (node->nodeName == READ)
			{
				std::string fileName = node->getAttr("file");
				init(fileName.c_str()); // 加载其他文本进入当前执行环境
			}
			else
			{
				takeNode(node); // 调用使用者的方法
			}
		}
		return  true;
	}
	/**
	 * 从字符串中初始化
	 */
	bool tixmlCode::initFromString(const char *content)
	{
		TiXmlDocument doc;
		doc.Parse(content);
		static std::string CODE="code";
		static std::string READ="read";
		TiXmlHandle hDoc(&doc);
		TiXmlElement* pElem = NULL;
		pElem=hDoc.FirstChildElement().Element();
		for(; pElem; pElem=pElem->NextSiblingElement())
		{
			const char *pKey=pElem->Value();
			tixmlCodeNode *node = new tixmlCodeNode();
			node->init(pElem);
			if (node->nodeName == CODE)
			{
				parseCode(node);
			}
			else if (node->nodeName == READ)
			{
				std::string fileName = node->getAttr("file");
				init(fileName.c_str()); // 加载其他文本进入当前执行环境
			}
			else
			{
				takeNode(node); // 调用使用者的方法
			}
		}
		return  true;
	}
	tixmlCodeNode::tixmlCodeNode()
	{
		next = NULL;
		parent = NULL;
		nowChildIndex = 0;
		element = NULL;
	}
	tixmlCodeNode::~tixmlCodeNode()
	{

	}
	/**
 	 * 构建这样的树结构
	 *[A]------|
	 * |	   |
	 * |	  [A1]
	 * |	   |
	 * |	  [A2]----|
	 * |	   |	[A21]----|
	 * |       |      |      |
	 * |	 NULL     NULL [A211]
	 * |		         |
	 * |		       [A212]
	 * |			 |
	 * |			NULL
	 *[B]                  
 	 * */
	tixmlCodeNode * tixmlCode::traceNode1(tixmlCodeNode *root,TiXmlNode *node)
	{
		static std::string WHILE = "while";
		static std::string WHEN = "when";
		static std::string IF = "if";

		tixmlCodeNode * now = NULL; 
		tixmlCodeNode * parent = root;
		tixmlCodeNode * pre = NULL;
		TiXmlNode *nowNode = node;
		while (!nowNode || nowNode->Type() != TiXmlNode::TINYXML_ELEMENT)
		{
			nowNode	= nowNode->NextSiblingElement();
		} 
		if (!nowNode || nowNode->Type() != TiXmlNode::TINYXML_ELEMENT) return NULL;
		while (true)
		{
			if (!nowNode) // 当前节点为空
			{
				if (!parent) break;
				nowNode = parent->element->NextSiblingElement();		
				parent = parent->parent;
                if (!parent) break;
				continue;
			}
			now = new tixmlCodeNode;
			if (!root) 
			{
				root = now;
			}
			
			now->init(nowNode->ToElement());
//			printf("[script]处理节点 %s\n",now->nodeName.str());	
			std::string& name = now->nodeName;
			if (name == WHILE || name == WHEN || name == IF)
			{
				void * code = getAction(now->getAttr("action"));	
//				if (!code) printf("[script] 处理节点 %s 获取action 失败\n",now->getAttr("action"));
				now->setExtData(code);
			}
			else
			{
				void* code = getAction(now->nodeName.c_str());
//				if (!code) printf("[script] 处理节点 %s 获取action 失败\n",now->nodeName.str());
				now->setExtData(code);
			}
			if (pre && !pre->next)
			{
				pre->next = now;
//				printf("[script]---------------- 处理节点 %s 的下一个节点 %s-----------\n",pre->nodeName.str(),now->nodeName.str());
				pre = NULL;
			}
			now->parent = parent;
			if (now->parent)
			{
				if (parent->childs.size())
					pre = parent->childs.back();
				parent->childs.push_back(now);
			}
			TiXmlNode * child = nowNode->FirstChildElement();
			TiXmlNode *tchild = nowNode->FirstChild();
			if ((child == NULL || (tchild && tchild->Type() == TiXmlNode::TINYXML_TEXT)) && parent) // 如果当前无孩子 且有父亲节点
			{
				if (tchild)
				{
					// 获取节点的文档描述
					TiXmlText *pText = tchild->ToText();
					if (pText)
					{
						//std::string str;
						//str<<*nowNode;
						//sys::sString value(str.c_str());
						std::string value(pText->Value());
//						std::string vs = value.toXmlString();
						now->setText(value);
					//	printf("----获取文本内容 %s----\n",vs.str());
					}
				}
				if (pre && !pre->next)
				{
					pre->next = now;
				//	printf("[SCRIPT]--------------- 当前节点 %s 下一个节点%s -----------\n",pre->nodeName.str(),now->nodeName.str());
				}
				if (!now->element->NextSiblingElement())
				{	
					pre = NULL;// TODO now;
					nowNode = parent->element->NextSiblingElement();
					parent = parent->parent; 
					if (parent)
					{
				//	printf("[script] 回溯到父节点:%s\n",parent->nodeName.str());
					}
					else break;
				} 
				else
				{
					nowNode = now->element->NextSiblingElement();	
				}
				if (parent)
				{
                   // printf("[script] 当前节点%s无孩子节点 有父节点%s\n",now->nodeName.str(),parent->nodeName.str());
				}
                else
                    break;
				
			}
			else if (child) // 当前有孩子
			{
				if (pre && !pre->next)
				{
					pre->next = now;
		//			printf("[SCRIPT]--------------- 当前节点 %s 下一个节点%s -----------\n",pre->nodeName.str(),now->nodeName.str());
				}
				nowNode = child; // 准备执行下一个节点
			//	printf("[script]当前节点有孩子节点 %s\n",now->nodeName.str());
				parent = now;
			}
			else // 当前无孩子 且 无 父亲节点 则退出
			{
				break;
			}	
		}
		return root;
	}
	void tixmlCodeNode::setText(std::string &text)
	{
		this->text = text;
	}
    bool tixmlCodeNode::release()
    {
        int count = 0;
		tixmlCodeNode * temp = firstChild();
		std::vector<tixmlCodeNode*> temps;
		while (temp)
		{
			tixmlCodeNode * node = temp;
			if(temp->firstChild())
			{
				temp = temp->firstChild();
			}
			else
			{
				while (temp && !temp->next)
				{
					temp = temp->parent;
				}
				if (temp)
					temp = temp->next;
			}
			temps.push_back(temp);
			count ++;
		}
		for (std::vector<tixmlCodeNode*>::iterator iter = temps.begin(); iter != temps.end();++iter)
		{
			if (*iter)
				delete *iter;
		}
		temps.clear();
		printf("[script] 释放了 %u 个节点\n",count);
		next = 0;
        return true;
    }
	void tixmlCodeNode::traceInfo()
	{
		int count = 0;
		tixmlCodeNode * temp = this;
		std::vector<tixmlCodeNode*> temps;
		while (temp)
		{
			tixmlCodeNode * node = temp;
			if(temp->firstChild())
			{
				temp = temp->firstChild();
			}
			else
			{
				while (temp && !temp->next)
				{
					temp = temp->parent;
				}
				if (temp)
					temp = temp->next;	
			}
			temps.push_back(temp);	
			count ++;
		}
        printf("\n节点信息 %s\n",nodeName.c_str());
		for (std::vector<tixmlCodeNode*>::iterator iter = temps.begin(); iter != temps.end();++iter)
		{
            tixmlCodeNode * node= *iter;
            if (node)
            {
                printf("<%s>",node->name());
            }
		}
        printf("\n...............\n");
		temps.clear();
		next = 0;
	}
	
};
