/**
 * email:ji.jin.dragon@gmail.com
 * Author jijinlong
 * Data 2011.10.14~2012.3.18~~
 * All Rights Resvered
 ***/
#pragma once
#include "xmlScript.h"
#include "map"
#include "var.h"
#ifdef  __USE_LUA__
#include "luadelegate.h"
#endif
namespace script{
    const  int __ACTION_CHECK__ = 0;
    const int __ACTION__RUN__ = 1;
	/**
 	 * 获取变量运算集合
 	 * */
	/**
	 * 将字符串解析为双目运算符
	 * <calc var="A=B+1"/> <!--运算语句-->
	 * <if action="calc" var="A>=B">
 	 * 		<!--如果A >= B 则执行-->
	 * </if>
	 * */
	struct CalcVar{
		enum{
			DEFAULT,
			FINISH,
			RESULT,
			FIRST,
			OP,
		};
		enum{
			NOOP = 0,
			ADD = 1,
			MUL = 2,
			SUB = 3,
			DIV = 4,
			GREAT = 5,
			GREAT_EQUAL = 6,
			LESS = 7,
			LESS_EQUAL = 8,
			EQAUL = 9,
			NOT_EQUAL = 10,
		};
		std::string result;
		std::string first;
		int op;
		std::string second;
        CalcVar()
        {
            clear();
        }
		void clear()
		{
			result = first = second = "";
			op = NOOP;
		}
		bool addName(char *word,int tag)
		{
			switch(tag)
			{
				case DEFAULT:
				{
					/**
 					 * 可能处理 result first second
 					 * */
					if (result=="")
						result = word;
					else if (first == "")
						first = word;
					else if (op == NOOP)
						setOp(word);
					else if (second == "")
						second = word;
					else return false;
					return true;
				}break;
				case FINISH:
				{
                    //	if (second == "") return false;
					return true;
				}break;
				case RESULT:
				{
					if (first == "" && second == "" && op == NOOP && result == "")
						result = word;
					else if (result == std::string(word)) return true;
					else return false;
					return true;
				}break;
				case FIRST:
				{
					if (first=="")
					{
						first = word;
					}
					else if (first == std::string(word)) return true;
					else return false;
					return true;
				}break;
				case OP:
				{
					if (op == NOOP)
						setOp(word);
					else return false;
					return true;
				}
			}
            return false;
		}
		void setOp(std::string opstr)
		{
			if (opstr == "+") op = ADD;
			else if (opstr == "-") op = SUB;
			else if (opstr == "*") op = MUL;
			else if (opstr == "/") op = DIV;
			else if (opstr == ">") op= GREAT;
			else if (opstr == "<") op = LESS;
			else if (opstr == ">=") op = GREAT_EQUAL;
			else if (opstr == "<=") op = LESS_EQUAL;
			else if (opstr == "==") op = EQAUL;
			else if (opstr == "!=") op = NOT_EQUAL;
		}
		bool checkIfCalc( char point)
		{
			static char pl[] = {'+','-','*','/','%','>','=','<','!'};
			for (int i = 0; i < sizeof(pl);i++)
			{
				if (pl[i] == point) return true;
			}
			return false;
		}
        CalcVar & operator = (const CalcVar &var)
        {
            this->op = var.op;
            result = var.result;
            first = var.first;
            second = var.second;
            return *this;
        }
    };
        struct LibCodeNode{
            tixmlCodeNode *node; // xml脚本
            std::string luaString; // lua脚本
            LibCodeNode()
            {
                node = NULL;
            }
        };
        /*
         * language action lib
         * 执行一段脚本 检查变量 执行分支,拥有的脚本能力
         * TACTIONCODE 对象函数指针 TEMPSTUB 当前对象 CHILED 库
         ***/
        template<typename TACTIONCODE,typename TEMPSTUB,typename CHILD>
        struct Lib:public tixmlCode{
            /**
             *	脚本库 清0
             **/
            Lib()
            {
            }
            /**
             *	使用脚本文件初始化脚本库
             * \param fileName 文件名字
             **/
            bool init(const char *fileName)
            {
                bindActions();
                if (!tixmlCode::init(fileName))
                {
                    return false;
                }
                return true;
            }
            /**
             * 根据名字获取行为
             * \param name 名字
             * \return 行为 不存在时为空
             **/
            virtual  void *getAction(const char *name)
            {
                if (!name) return NULL;
                ACTIONS_ITER iter = actions.find(name);
                if (iter != actions.end())
                {
                    return iter->second;
                }
                return NULL;
            }
            std::map<std::string,Var> codeVars; // 一个行为中的临时变量
            std::map<std::string,Var> functionCodeVars; // 一个行为中的临时变量
            std::map<std::string,Var> gloabVars; // 全局变量
            typedef std::map<std::string,Var>::iterator CODEVARS_ITER;
#ifdef __USE_LUA__
            LuaDelegate* luaManager;
#endif
			/**
             *	清除变量
             *		若是临时变量 清除临时变量后 不再清除附着对象的变量
             * 		若不是临时变量 尝试清除附着对象的变量
             * \param name 变量名字
             **/
            void clearVar(TEMPSTUB *stub,sys::sString name)
            {
                CODEVARS_ITER iter = codeVars.find(name.toString());
                if (iter != codeVars.end())
                {
                    codeVars.erase(iter);
                    return;
                }
                iter = gloabVars.find(name.toString());
                if (iter != gloabVars.end())
                {
                    gloabVars.erase(iter);
                    return;
                }
                iter = functionCodeVars.find(name.toString());
                if (iter != functionCodeVars.end())
                {
                    functionCodeVars.erase(iter);
                    return;
                }
            }
            enum ScopeType{
                SCOPE_TYPE_TEMP, // 临时变量
                SCOPE_TYPE_STUB, // stub变量
                SCOPE_TYPE_FUNCTION, // function 变量
                SCOPE_TYPE_GLOAB, // 全局变量
            };
			
            /**
             * 增加变量
             * \param name 变量名字
             * \param var 变量
             * \param scope 1 临时变量 其他玩家变量
             * \param type var::timetype
             **/
            void addCodeVar(TEMPSTUB *stub,std::string name,std::string value,ScopeType scope,BYTE timeType,tixmlCodeNode * temp)
            {
                if (name == "") return; // 变量为空时返回
				if (!temp) return;
                Var var;
                var.name = name;
                var.value = value;
                if (scope == SCOPE_TYPE_STUB)
                    var.init(timeType);
                else
                    var.init(Var::FOROVER);
                switch(scope)
                {
                    case SCOPE_TYPE_TEMP:
                    {
                        CODEVARS_ITER iter = codeVars.find(name);
                        if (iter == codeVars.end())
                        {
                            codeVars[name] = var;
                        }
                        else
                            iter->second.value = value;
                    } break;
					case SCOPE_TYPE_STUB:
					{
						vAddVar(stub,temp,var);
					}break;
                    case SCOPE_TYPE_FUNCTION:
                    {
                        CODEVARS_ITER iter = functionCodeVars.find(name);
                        if (iter == functionCodeVars.end())
                        {
                            functionCodeVars[name] = var;
                        }
                        else
                            iter->second.value = value;
                    }break;
                    case SCOPE_TYPE_GLOAB:
                    {
                        CODEVARS_ITER iter = gloabVars.find(name);
                        if (iter == gloabVars.end())
                        {
                            gloabVars[name] = var;
                        }
                        else
                            iter->second.value = value;
                    }
                        break;
                }
            }
			virtual void vAddVar(TEMPSTUB *stub,tixmlCodeNode * node,Var &var)
			{
				printf("该系统未定义变量拓展\n");
			} 
			virtual bool vGetVar(TEMPSTUB *stub,tixmlCodeNode *node,Var &var)
			{
				printf("该系统未定义变量拓展");
				return false;
			}
            /**
             * 获取变量
             * \param name 变量名字
             * \param var 变量
             * return 存在与否
             **/
            bool getVar(TEMPSTUB *stub,std::string name,Var &var,tixmlCodeNode *node)
            {
				if (!node) return false;
                if (name == "") return false;
                CODEVARS_ITER iter = codeVars.find(name);
                if (iter != codeVars.end())
                {
                    var = iter->second;
                    return true;
                }
                iter = gloabVars.find(name);
                if (iter != gloabVars.end())
                {
                    var = iter->second;
                    return true;
                }
                iter = functionCodeVars.find(name);
                if (iter != functionCodeVars.end())
                {
                    var = iter->second;
                    return true;
                }
               	// 尝试获取扩展库的变量系统
				var.name = name;
               	if (vGetVar(stub,node,var)) return true;
                return false;
            }
            /**
             * 释放资源
             * return 成功与否
             **/
            bool release()
            {
                for (CODES_ITER iter = codes.begin(); iter != codes.end();++iter)
                {
                    tixmlCodeNode * code = iter->second.node;
                    if (code)
                    {
                        code->release();
                        delete code;
                    }
                }
                codes.clear();
                return true;
            }
            /**
             *执行脚本
             * \param name 脚本名字
             **/
            virtual void execScript(TEMPSTUB *stub,const char *name)
            {
                functionCodeVars.clear();
                if (!execCode(stub,name))
                {
					printf("执行脚本失败:%s\n",name);
                }
                functionCodeVars.clear();
            }
        public:
			#ifdef __USE_LUA__
				void setLuaDelegate(LuaDelegate *luadelegate)
				{
					this->luaMamager = luadelegate;
				}
			#endif
            /**
             * 执行lua 脚本
             * \param commonStub 参数
             * \param codeSring lua的脚本
             * 脚本库入口函数 function start(stub) end
             **/
            void doLuaString(TEMPSTUB *commonStub,const char *codeString)
            {
#ifdef __USE_LUA__
			   if (luaManager)
			   {
				   lua_tinker::dostring(luaManager->getState(),codeString);		
				   if (checkHadFunc(codeString,"start"))
					lua_tinker::call<void>(luaManager->getState(),"start",commonStub);
			   }
#endif
            }
			bool checkHadFunc(std::string codeString,std::string funcName)
			{
				if (codeString.find(funcName) == std::string::npos)
					return false;
				return true;
			}
            /*
             *  执行一段代码
             *  \param name代码名称
             ***/
            virtual bool execCode(TEMPSTUB *commonStub,const char*name)
            {
                codeVars.clear();
                CODES_ITER iter = codes.find(name);
                tixmlCodeNode * temp = NULL;
                if (iter != codes.end())
                {
                    temp = iter->second.node;
                    if (!temp)
                    {
                        if (iter->second.luaString != "")
                        {
                            doLuaString(commonStub, iter->second.luaString.c_str());
                        }
                        else return false;
                    }
					if (temp)
					{
						temp->traceInfo();
						temp = (tixmlCodeNode*)temp->firstChild();
					}
				}
                if (!temp) return false;
                CHILD * child = static_cast<CHILD*>( this);
                if (!child)return false;
                int count = 0;
                while (temp)
                {
                    if (count++)
                    {
                        //	printf("执行到第%d个标签<%s>\n",count,temp->name());
                    }
                    // action 变量设置
                    //				printf("[SCRIPT] 当前执行节点 node :%s\n",temp->nodeName.str());
					static std::string WHILE = "while"; // 关键字
                    static std::string WHEN = "when"; // 关键字
                    static std::string IF = "if"; // 关键字
                    static std::string ELSE1 = "else"; // 关键字
                    static std::string CODE = "code"; // 关键字
                    if (temp->nodeName == CODE)
                    {
                   //     printf("[script] ...当前回溯到了code 节点,程序有BUG...\n");
                        break;
                    }
                    TACTIONCODE *code = (TACTIONCODE*) temp->extData;
                    if (!code && temp->nodeName != WHEN && temp->nodeName != ELSE1)
                    {
                     //   printf("[script] 当前节点%s 获取code 失败\n",temp->nodeName.str());
                        return false; // 错误了
                    }
                    if (temp->nodeName == WHILE) // 如果是while 节点
                    {
                        // 执行首个点,内置的条件语句 主要处理变量
                        if ((child->*code->action)(commonStub,temp))
                        {
                            //temp = temp->firstChild();
                            doChild(temp);
                        }
                        else
                        {
                            doWhileNext(temp);
                        }
                    }
                    else if (temp->nodeName == WHEN) // WHEN CASE 节点
                    {
                        // 遍历child 节点 将下一节点 设置成可用节点
                        bool next = true;
                        for (tixmlCodeNode::CHILDS_ITER iter = temp->childs.begin(); iter != temp->childs.end();++iter)
                        {
                            tixmlCodeNode * node = *iter;
                            if (!node)
                            {
                         //       printf("[SCRIPT] when 执行错误\n");
                                return false;
                            }
                            TACTIONCODE * code = (TACTIONCODE*) node->extData;
                            if (code && (child->*code->action)(commonStub,node))
                            { // 存在分支 检查通过
                                tixmlCodeNode * tnext = temp->next;
								doChild(temp);
                                //temp = node->firstChild();
                                // temp->next = tnext;
                                next = false;
                                break;
                            }
                        }
                        if (next)
                        {
                            doNext(temp);
                        }
                    }
                    else if (temp->nodeName == IF) // IF 节点
                    {
                        int result = (child->*code->action)(commonStub,temp);
                        if (result)
                        {
                            // IF 条件检查通过
                            //						printf("[SCRPT] 执行if 条件通过 %s result:%u\n",temp->getAttr("action"),result);
                          //  temp=temp->firstChild();
                              doChild(temp);
                        }
                        else
                        {
                            // IF 条件检查失败
                     //       printf("[SCRIPT] 执行if 条件非通过 %s\n",temp?temp->getAttr("action"):"null");
                            doNext(temp);
                        }
                    }
                    else if (temp->nodeName == ELSE1)
                    {
                        
                        int result = 1;
                        if (code)
                            result = (child->*code->action)(commonStub,temp);
                        if (result)
                        {
                            // ELSE 条件检查通过
                            //						printf("[SCRPT] 执行else 条件通过 %s result:%u\n",temp->getAttr("action"),result);
                            //temp=temp->firstChild();
                            doChild(temp);
                        }
                        else
                        {
                            // ELSE 条件检查失败
                            doNext(temp);
                            //						printf("[SCRIPT] 执行else 条件非通过 %s\n",temp->getAttr("action"));
                        }
                    }
                    else if (code && code->type == __ACTION_CHECK__) // 检查节点
                    {
                        // 如果为CHECK点
                        if ((child->*code->action)(commonStub,temp))
                        {
                            //temp = temp->firstChild();
                            doChild(temp);
                        }
                        else
                        {
                            doNext(temp);
                            //						printf("[SCRIPT] 处理一下个节点:%s\n",temp?temp->nodeName.str():"null");
                            
                        }
                    }
                    else if (code)// 普通语句
                    {
                        (child->*code->action)(commonStub,temp);
                        doNext(temp);
                        //					printf("[SCRIPT] 处理一下个节点:%s\n",temp?temp->nodeName.str():"null");
                    }
                    else
                        return false;
                    // action 变量清除
                }
                codeVars.clear();
                return true;
            }
			inline void doChild(tixmlCodeNode *&temp)
			{
				if (temp){
				   tixmlCodeNode * child = temp->firstChild();
				   if (!child) doNext(temp);
				   else temp = child;
				}
			}
            /**
             * 执行当前节点的下一个节点
             * \param temp 当前几点
             * */
            inline void doNext(tixmlCodeNode * &temp)
            {
                static const std::string WHILE = "while"; // 关键字
                static const std::string WHEN = "when"; // 关键字
                static const std::string IF = "if"; // 关键字
                static const std::string ELSE1 = "else"; // 关键字
                if (temp && !temp->next &&temp->parent &&temp->parent->nodeName == IF &&
                    temp->parent->next && temp->parent->next->nodeName == ELSE1)
				/**如果当前节点的父节点是if 且下一个节点为else 执行else 的下一个节点**/
                {
                    temp = temp->parent->next;
                }
                bool tag = true;
                while (temp && (!temp->next || (
                                                temp->next == temp->parent->next &&
                                                temp->parent && temp->parent->nodeName == WHILE)))
                {
                    if (temp->nodeName == WHILE)
                    {
                        tag = false;
                        break;
                    }
                    
                    temp = temp->parent;
                    
                    if (temp && temp->nodeName == IF && temp->next && temp->next->nodeName == ELSE1)
                    {
                        temp = temp->next;
                    }
                }
                if (temp && temp->nodeName == WHILE)
                {
                    tag = false;
                }
                if (temp && tag)
                    temp = temp->next;
            }
            /**
             * 执行while false的下一个节点
             * */
            inline void doWhileNext(tixmlCodeNode * &temp)
            {
                static std::string WHILE = "while"; // 关键字
                static std::string WHEN = "when"; // 关键字
                static std::string IF = "if"; // 关键字
                static std::string ELSE1 = "else"; // 关键字
                
                if (temp && !temp->next &&temp->parent &&temp->parent->nodeName == IF &&
                    temp->parent->next && temp->parent->next->nodeName == ELSE1)
                {
                    temp = temp->parent->next;
                }
                bool tag = true;
                while (temp && (!temp->next || (
                                                temp->next == temp->parent->next &&
                                                temp->parent && temp->parent->nodeName == WHILE)))
                {
                    temp = temp->parent;
                    if (temp && temp->nodeName == WHILE)
                    {
                        tag = false;
                        break;
                    }
                    temp = temp->parent;
                    
                    if (temp && temp->nodeName == IF && temp->next && temp->next->nodeName == ELSE1)
                    {
                        temp = temp->next;
                    }
                }
                if (temp && tag)
                    temp = temp->next;
            }
	public:
            std::map<std::string,LibCodeNode> codes; //在初始化时生成
            typedef std::map<std::string,LibCodeNode>::iterator CODES_ITER;
            typedef typename std::map<std::string,TACTIONCODE*>::iterator ACTIONS_ITER;
            typename std::map<std::string,TACTIONCODE*> actions;
		public:
            /*
             * 绑定行为
             **/
            virtual void bindActions()
            {
            }
            /**
             *	解析文本得到 code
             **/
            virtual void parseCode(tixmlCodeNode *code)
            {
                // 遍历code 填充action
                //<code /> 为之填充所有相关节点
                // 生成链式可执行文本，关键算法
                // 得到code 节点 树遍历，后期优化成while 遍历
                tixmlCodeNode * tiNode = (tixmlCodeNode*) code;
                sys::sString type = "xml";
                static sys::sString XML="xml";
                static sys::sString LUA = "lua";
                if (tiNode)
                {
                    type = tiNode->getAttr("type");
                    if (type == "")
                        type = "xml"; // 默认使用xml 脚本
                }
                else
                    return;
                if (type == XML)
                {
                    tixmlCodeNode * node = traceNode1(NULL,tiNode->element);
                    const  char * codeName = node->getAttr("name");
                    LibCodeNode codeNode;
                    codeNode.node = node;
                    if (codeName)
                        codes[codeName] = codeNode;
                    else
                        printf("[script] codename 未指定 ");
                }
                else if (type == LUA) // 执行lua
                {
                    // 像lua库提供<name,text>
                    const char *codeName =tiNode->getAttr("name");
                    TiXmlNode * tchild = tiNode->element->FirstChild();
                    if (tchild->Type() == TiXmlNode::TINYXML_TEXT)
                    {
                        TiXmlText *pText = tchild->ToText();
                        LibCodeNode codeNode;
                        codeNode.luaString = pText->Value();
                        codes[codeName] = codeNode;
                    }
                }
            }
			enum{
				PASS = 1,  // 1 条件通过
				NO_PASS = 0, // 0 条件非通过
			};
            /**
             * 解析变量
             * 支撑 双目运算符，比较运算符，赋值运算
             * \return 最终运算的变量
             * */
			virtual int parseVar(TEMPSTUB *stub,tixmlCodeNode *node,std::string var="var")
            {
				if (!stub || !node) return NO_PASS;
                std::vector<CalcVar> cals;
				const char* sentence = node->getAttr(var.c_str());
                getCalcVars(stub,sentence,cals); // 使用状态机 获取每个变量
                bool result = false;
                for (std::vector<CalcVar>::iterator iter = cals.begin(); iter != cals.end();++iter)
                {
                    CalcVar & calcVar = *iter;
                    //  printf("当前操作符号:%d\n",calcVar.op);
                    Var first = iter->first.c_str();
                    if(!getVar(stub,iter->first,first,node))
                    {
                        first.name = iter->first;
                        first.type = Var::FOROVER;
                        first.value = iter->first;
                        first.type= Var::FOROVER;
                    }
                    Var second = iter->second.c_str();
                    if (!getVar(stub,iter->second,second,node))
                    {
                        second.value = iter->second;
                        second.type = Var::FOROVER;
                        second.name = iter->second;
                        second.type= Var::FOROVER;
                    }
                    if (iter->result == "")
                    {
                        switch(iter->op)
                        {
                            case CalcVar::GREAT:
                            {
                                result = first > second;
                            }break;
                            case CalcVar::LESS:
                            {
                                result = first < second;
                            }break;
                            case CalcVar::EQAUL:
                            {
                                result = first == second;
                            }break;
                            case CalcVar::GREAT_EQUAL:
                            {
                                result = first >= second;
                            }break;
                            case CalcVar::LESS_EQUAL:
                            {
                                result = first <= second;
                            }break;
                            case CalcVar::NOT_EQUAL:
                            {
                                result = first != second;
                            }break;
                            default:
                                return NO_PASS;
                        }
                    }
                    else
                    {
                        Var result;
                        result.name = iter->result;
                        result.type = Var::FOROVER;
                        if (!getVar(stub,iter->result,result,node))
                        {
                            addCodeVar(stub,iter->result,"",SCOPE_TYPE_TEMP,0,node); // 增加临时变量
                        }	
                        switch(iter->op)
                        {
                            case CalcVar::ADD:
                            {
                                first.add(second);
                            }break;	
                            case CalcVar::MUL:
                            {
                                first.mul(second);
                            }break;
                            case CalcVar::SUB:
                            {
                                first.minus(second);
                            }break;
                            case CalcVar::DIV:
                            {
                                first.div(second);
                            }break;
                            default:
                                return NO_PASS;
                        }
                        result.value = first.value;
                        addCodeVar(stub,result.name,result.value,SCOPE_TYPE_TEMP,result.type,node);
                    }
                }
                if (result) return PASS;
                return NO_PASS;
            }
            /**
             * 解析语句获取运算符列表
             * \paran sentence 带解析的语句
             * \param calc 运算符列表
             * */	
            virtual void getCalcVars(TEMPSTUB *stub,const char *sentence,std::vector<CalcVar> & calcs)
            {
                const char * pointer = sentence;
                const char * nowwordbegin = NULL;
                int nowLen = 0;
                char word[1024] = {'\0'};
                CalcVar calcVar;
                bool hadEqual = false;
                while (*pointer && *pointer != '\0')
                {
                    if (*pointer == ' ' ||
                        *pointer == '\t' || *pointer=='\n')
                    {
                        if (nowwordbegin)
                        {
                            bzero(word,1024);
                            strncpy(word,nowwordbegin,nowLen);
                            if (!calcVar.addName(word,CalcVar::DEFAULT))
                            {
                                // TODO ERROR
                                break;
                            }
                            nowwordbegin = NULL;
                            nowLen = 0;
                            bzero(word,1024);
                        }
                        pointer ++;
                        continue;
                    }
                    if (*pointer == ';')
                    {
                        if (nowwordbegin)
                        {
                            bzero(word,1024);
                            strncpy(word,nowwordbegin,nowLen);
                            if (!calcVar.addName(word,CalcVar::DEFAULT))
                            {
                                // TODO ERROR
                                break;
                            }
                            bzero(word,1024);
                        }
                        if(calcVar.addName(NULL,CalcVar::FINISH))
                        {
                            if (!hadEqual)
                            {
                                std::string second = calcVar.first;
                                calcVar.first = calcVar.result;
                                calcVar.second = second;
                                calcVar.result = "";
                            }
                            hadEqual = false;
                            calcs.push_back(calcVar);	
                            calcVar.clear();
                            pointer ++;
                            nowwordbegin = NULL;
                            nowLen = 0;
                            bzero(word,1024);
                            continue;
                        }
                        else
                        {
                            // TODO error
                            break;
                        }
                    }
                    if (*pointer == '=')
                    {
                        if (*(pointer + 1) != '=') // 不是==比较运算符
                        {
                            if (nowwordbegin)
                            {
                                strncpy(word,nowwordbegin,nowLen);
                                if (!calcVar.addName(word,CalcVar::RESULT))
                                {
                                    // TODO error
                                    break;
                                }
                                
                            }	
                            nowwordbegin = NULL;
                            nowLen = 0;
                            pointer ++;
                            hadEqual = true;
                            bzero(word,1024);
                            continue;
                        }
                    }
                    if (calcVar.checkIfCalc(*pointer))
                    {
                        if (nowwordbegin)
                        {
                            strncpy(word,nowwordbegin,nowLen);
                            if (!calcVar.addName(word,CalcVar::FIRST))
                            {
                                // TODO error
                                break;
                            }
                        }
                        bzero(word,1024);
                        if (*pointer == '-' && *(pointer +1 ) && *(pointer + 1) != '\0') // 识别负数
                        {
                            if (*(pointer+1) >= '0' && *(pointer+1) <= '9')
                            {
                                // 是负数
                                nowwordbegin = pointer;
                                nowLen = 1;
                                pointer ++;
                                continue;
                            }
                        }
                        nowwordbegin = NULL;
                        nowLen = 0;
                        
                        if (*(pointer+1) == '=')
                        {
                            strncpy(word,pointer,2);
                            if (!calcVar.addName(word,CalcVar::OP))
                            {
                                // TODO error
                                break;
                            }
                            bzero(word,1024);
                            pointer ++;
                        }
                        else
                        {
                            strncpy(word,pointer,1);
                            calcVar.addName(word,CalcVar::OP);
                            bzero(word,1024);
                        }	
                        if (*pointer =='\0')
                        {
                            // TODO error
                            break;
                        }
                        pointer ++;
                        continue;
                    }
                    if (!nowwordbegin) nowwordbegin = pointer;
                    nowLen ++;
                    pointer ++;
                }
                if (nowwordbegin)
                {
                    bzero(word,1024);
                    strncpy(word,nowwordbegin,nowLen);
                    if (!calcVar.addName(word,CalcVar::DEFAULT))
                    {
                        // TODO ERROR
                        return;
                    }
                    bzero(word,1024);
                }
                if(calcVar.op != CalcVar::NOOP && calcVar.addName(NULL,CalcVar::FINISH))
                {
                    if (!hadEqual)
                    {
                        std::string second = calcVar.first;
                        calcVar.first = calcVar.result;
                        calcVar.second = second;
                        calcVar.result = "";
                    }
                    hadEqual = false;
                    calcs.push_back(calcVar);	
                    calcVar.clear();
                    pointer ++;
                    nowwordbegin = NULL;
                    nowLen = 0;
                    bzero(word,1024);
                    return;
                }
                
            }
            /**
             * 将字符串中A {xxx } B指定的变量变为值
             **/
            std::string parseString(TEMPSTUB *stub,tixmlCodeNode *node)
            {
                std::string str;
                std::string varname;
                bool hadword = false;
                bool beginVar = false;
				const char *temp = node->getAttr("content");
				if (!temp) return "";
                while(*temp && *temp != '\0')
                {
                    if (*temp == '{')
                    {
                        beginVar = true;
                        temp++;
                        continue;
                    }
                    if (*temp == '}')
                    {
                        beginVar = false;
                        hadword = false;	
                        Var var;
                        if (getVar(stub,varname,var,node))
                        {
                            str.append(var.value);
                        }
						varname = "";
                        temp ++;
                        continue;
                    }
                    if (beginVar)
                    {
                        if (hadword)
                        {
                            temp++;
                            continue;
                        }
                        if (*temp != ' ')
                        {
                            varname.append(temp,1);
                        }	
                        else
                        {
                            if (varname != "")
                            {
                                hadword = true;
                            }
                        }
                    }
                    else
                    {
                        str.append(temp,1);
                    }
                    temp ++;
                }
                return str;
            }
            /**
             * 处理几个基本的语句 包括计算变量 calc print var 语句
             * */
            int var(TEMPSTUB *stub,script::tixmlCodeNode * node)
            {
                std::string varname = node->getAttr("name");
                script::Var var;
                if (varname == "")
                    return 0;
                else
                {
                    if (getVar(stub,varname,var,node))
                    {
                        var.set(node->getAttr("value"));
                        addCodeVar(stub,varname, var.value,SCOPE_TYPE_TEMP,0,node);
                        return 1;
                    }
                    else
                    {
                        ScopeType tag = SCOPE_TYPE_FUNCTION;
                        std::string scope = node->getAttr("scope");
                        if (scope == "stub")
                        {
                            tag = SCOPE_TYPE_STUB;
                        }
                        else if (scope == "temp")
                        {
                            tag = SCOPE_TYPE_TEMP;
                        }
                        else if (scope == "gloab")
                        {
                            tag = SCOPE_TYPE_GLOAB;
                        }
						else if (scope == "function")
						{
							tag = SCOPE_TYPE_FUNCTION;
						}
						else if (scope != "")
						{
							return 0;
						}
                        std::string type = node->getAttr("type");
                        
                        BYTE time = script::Var::FOROVER;
                        
                        if (type == "month")
                        {
                            time = script::Var::MONTH;
                        }
                        else if (type == "year")
                        {
                            time = script::Var::YEAR;
                        }
                        else if (type == "day")
                        {
                            time = script::Var::DAY;
                        }
						else if (type == "week")
						{
							time = script::Var::WEEK;
						}
						else if (type != "")
						{
							return 0;
						}
                        addCodeVar(stub,varname, node->getAttr("value"),tag,time,node);
                        return 1;
                    }
                }
                return 0;
            }
            int print(TEMPSTUB* stub,script::tixmlCodeNode * node)
            {
                script::Var var;
                printf("print:%s\n",parseString(stub,node).c_str());
                return 0;
            }
            int calc(TEMPSTUB* stub,script::tixmlCodeNode * node)
            {
                return parseVar(stub,node);
            }
            int exec(TEMPSTUB* stub,script::tixmlCodeNode * node)
            {
                execCode(stub,node->getAttr("name"));
                return 0;
            }
        };
    };
