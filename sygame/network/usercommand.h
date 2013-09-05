#pragma once
#include "mcenter.h"
/**
 * 发送聊天内容
 * */
struct stReqTalk:public CmdObject{
	stReqTalk():CmdObject(1) { }
	CMD_SERIALIZE()
	{
		BIND(content);
		BIND(uniqueId);
		BIND(boardcastType);
	}	
	std::string content; // 内容
	DWORD uniqueId; // 唯一id
	DWORD boardcastType; // 广播类型
	DWORD fromId; // 信息来自于玩家的id
};
/**
 * 请求发起攻击
 * */
struct stReqAttack:public CmdObject{
	stReqAttack():CmdObject(2){}
	CMD_SERIALIZE()
	{
		BIND(skillId);
		BIND(x);
		BIND(y);
		BIND(uniqueId);
		BIND(attackType);
	}
	DWORD skillId; // 技能id
	DWORD x; // 作用的横坐标
	DWORD y; // 作用的纵坐标
	DWORD  uniqueId; // 作用的对象
	DWORD attackType; // 攻击的对象类型
};

/**
 * 发起移动请求
 **/
struct stReqMove:public CmdObject{
    stReqMove():CmdObject(13){}
    CMD_SERIALIZE()
    {
        BIND(x);
        BIND(y);
        BIND(dir);
    }
    WORD x; // x 坐标
    WORD y; // y 坐标
    BYTE dir; // 移动方向
};
/* 
 *返回移动消息
 */
struct stRetMove:public CmdObject{
    stRetMove():CmdObject(15) {}
    CMD_SERIALIZE()
    {
        BIND(x);
        BIND(y);
        BIND(dir);
    }
    WORD x;
    WORD y;
    BYTE dir;
};
/**
 * 登录管理节点, 获取有效登录节点
 * */
struct  stReqValidLoginNode:public CmdObject{
    stReqValidLoginNode():CmdObject(5) {}
	std::string name; // 玩家名字
	std::string pwd; // 玩家密码
	CMD_SERIALIZE()
	{
		BIND(name);
		BIND(pwd);
	}
};
/**
 * 返回获取的url
 */
struct stRetValidLoginNode:public CmdObject{
	stRetValidLoginNode():CmdObject(4){}
	std::string url; // 登录节点url
	int userId; // 玩家id
	CMD_SERIALIZE()
	{
		BIND(url);
		BIND(userId);
	}
};
/**
 * 请求登录
 */
struct stReqLoginInGame : public CmdObject{
    stReqLoginInGame():CmdObject(6) {}
    std::string name;
    DWORD userId;
	std::string mapName;
	WORD x;
	WORD y;
    CMD_SERIALIZE()
    {
        BIND(name);
		BIND(userId);
		BIND(mapName);
		BIND(x);
		BIND(y);
    }
};
/**
 * 请求登出
 */
struct stReqLoginOut:public CmdObject{
    stReqLoginOut():CmdObject(7) {}
};


/**
 * 玩家数值面板更新
 * */
struct stRetPkValue:public CmdObject{
	stRetPkValue():CmdObject(8){}
};

struct stRetLoginInGame:public CmdObject{
	stRetLoginInGame():CmdObject(17){}
	std::string userName;
	DWORD id;
	CMD_SERIALIZE()
    {
		BIND(userName);
		BIND(id);
	}
};