#include "mcenter.h"
#include "serialize.h"
/**
 * 处理消息串行化 和 数据库持久化
 **/
const int __DATA__CMD__  = 0;
const int __DATA__BASE__ = 0;
struct CmdBinaryObject:public CmdObject,public serialize::Object{
public:
    CmdBinaryObject(int msgid = 0):CmdObject(msgid){
    
    }
	/**
	 * 消息持久化
	 */
	void __to_msg__record__(unsigned char tag,cmd::Stream &ss)
	{
		__do__common_store(NULL,0,__DATA__CMD__,&ss,tag);
	}
	/**
 	 * 数据持久化
	 */
	void __store__(serialize::Stream &ss,BYTE tag)
	{
		serialize::Stores sts;
		if (tag == __LOAD__)
		{
			sts.parseRecord(ss);
		}
		__do__common_store(&sts,tag,__DATA__BASE__,NULL,0);
		if (tag == __STORE__)
		{
			ss = sts.toRecord();
			beStored();
		}
		else
		{
			beLoaded();
		}
	}
 	virtual	void __do__common_store(serialize::Stores *ss,BYTE tag,int storeType,cmd::Stream *cmdss,BYTE cmdtag) = 0;
};


#define DATA_SERIALIZE()\
	virtual void __do__common_store(serialize::Stores *sts,BYTE tag,int storeType,cmd::Stream *cmdss,BYTE cmdtag)
#define SERIALIZE_BASE(__class__)\
        __do__common_store(sts,tag,storeType,cmdss,cmdtag)
#define SERIALIZE(__value__,index)\
	if (storeType == __DATA__CMD__ && cmdss)\
	{\
		switch(cmdtag)\
		{\
			case cmd::GET:\
			{\
				getRecord(__value__,*cmdss);	\
			}break;\
			case cmd::ADD:\
			{\
				addRecord(__value__,*cmdss);\
			}break;\
			case cmd::CLEAR:\
			{\
				clearRecord(__value__);\
			}break;\
		}\
	}\
	else if (sts)\
	{\
		switch(tag)\
		{\
			case __STORE__:\
			{\
				sts->addStore(__value__,index);\
			}break;\
			case __LOAD__:\
			{\
				sts->getStore(__value__,index);\
			}break;\
		}\
	}

#define SERIALIZE_CONDITION(__value__,index,__result__)\
    if (storeType == __DATA__CMD__ && cmdss)\
    {\
        switch(cmdtag)\
        {\
            case cmd::GET:\
            {\
                getRecord(__value__,*cmdss);	\
            }break;\
            case cmd::ADD:\
            {\
                if ((__result__))\
                    addRecord(__value__,*cmdss);\
            }break;\
            case cmd::CLEAR:\
            {\
                clearRecord(__value__);\
            }break;\
        }\
    }\
    else if (sts)\
    {\
        switch(tag)\
        {\
            case __STORE__:\
            {\
                if ((__result__))\
                    sts->addStore(__value__,index);\
            }break;\
            case __LOAD__:\
            {\
                sts->getStore(__value__,index);\
            }break;\
        }\
    }

