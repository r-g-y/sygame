/**
 * 主界面 包含移动盘 技能盘 系统箱 小地图 人物头像
 */
#pragma once
#include "cocos2d.h"
#include "RpgPlayerBag.h"
#include "UICenterBag.h"
#include "xmlScript.h"
#include "UITextArea.h"
NS_CC_BEGIN
class RpgMap;
class RpgPlayer;
class RpgUI;
class RpgSkillData;
/**
 * 1.技能盘 是可变的 需要存盘 当.uipack 不存在时 从配置文件中获取ui信息 或者从.uipack 中获取信息
 * 存盘的时候 不需要与服务器同步 服务器只控制该技能是否存在 客户端在技能释放的时候 需要判定服务器的数据
 * 验证技能的存在
 *
 * 2.人物头像 点击后出现人物属性信息 位置从配置文件中加载 数据来自网络 或者 .tmp 文件
 * 3.小地图
 * 4.移动盘 配有lua脚本 用来实际定义移动盘的逻辑 和 展示
 * 5.系统箱 从配置中读取 并在lua中实现其逻辑 关闭 音效 包裹 技能 人物 聊天 ==
 * 6.访问提示信息 该部分用lua实现 on_visit 事件的触发 on_leave 事件的触发
 * 7.人物包裹
 *		数据来自 .pbag 中
 */
class RpgMoveOPUI:public CCNode{
public:
	CCPoint opMoveCenter;
	// 可以操作角色移动 
	static RpgMoveOPUI* create(RpgPlayer *player);
	bool init(RpgPlayer *player);
	RpgPlayer *player;
	CCTouch *nowTouch;
	/**
	 * 处理点击
	 * \parma touchType 点击类型
	 * \param touchPoint 点击点
	 */
	bool doTouch(int touchType,CCTouch* touch);
	RpgMoveOPUI()
	{
		nowTouch = NULL;
		player = NULL;
		dirSprite = NULL;
		nowTouchIn = false;
		tagSprite = NULL;
	}
	bool nowTouchIn; // 当前是否在其上操作

	void showTouch(int dirType);
	CCSprite *dirSprite;
	CCSprite * tagSprite;
}; // 移动盘 该盘一直在Game上

class RpgHpValueUI:public CCNode{
public:
	static RpgHpValueUI* create(float maxValue,float defaultValue,const char *upName="hpstar2.png",const  char *backName="hpstar1.png");
	bool init(float maxValue,float defaultValue,const char *upName,const  char *backName);
	
	void setValue(float value);
	RpgHpValueUI()
	{
		maxValue = 0;
		value = 0;
		backSprite = NULL;
		valueSprite = NULL;
		width = 0;
	}
private:
	int width;
	float maxValue;
	float value;
	CCSprite *backSprite;
	CCSprite *valueSprite;
}; // 角色血量值

class RpgPlayerSimpleInfoUI:public CCNode{
public:
	static RpgPlayerSimpleInfoUI* create(RpgPlayer *player);
	bool init(RpgPlayer *player);
	RpgPlayerSimpleInfoUI()
	{
		hpValue = NULL;
		player = NULL;
		tag = 0;
	}
	void update(); // 根据数据

	void resume();
private:
	int tag;
	RpgHpValueUI *hpValue;
	std::vector<RpgHpValueUI*> hpValues;
	RpgPlayer *player;

}; // 角色简易信息展示 一个头像 一个血条

/**
 * 角色装备信息 和 属性值
 */
class RpgPlayerInfoUI:public UISuperBag,public script::tixmlCode{
public:
	static RpgPlayerInfoUI* create();
	/**
	 * 初始化格子位置 以及可更新数据的位置
	 **/
	bool init();
	RpgPlayerInfoUI()
	{
		
	}
	/**
	 * 更新数值
	 */
	void updateValue(const std::string& content,float value);

	/**
	 * 获取当前可视的宽
	 */
	float getViewWidth();
	/**
	 * 获取当前可视的高
	 **/
	float getViewHeight();
	const CCSize & getContentSize()
	{
		return CCSizeMake(getViewWidth(),getViewHeight());
	}
private:
	/**
	 * 从配置文件中加载配置
	 * \param node 配置根节点
	 */
	void takeNode(script::tixmlCodeNode *node);
	/**
	 * 或者值的位置
	 */
	CCPoint getValuePosition(const std::string& content);
	std::map<std::string,CCLabelTTF*> values;
	std::map<std::string,CCPoint> valuePositions;
	typedef std::map<std::string,CCLabelTTF*>::iterator VALUES_ITER;
	typedef std::map<std::string,CCPoint>::iterator VALUES_POS_ITER;
	/**
	 * 获取当前id 的像素位置
	 */
	virtual CCPoint getPixelPosition(int id);

	virtual int getNowTouchBagIdByCursorPosition(const CCPoint& pos);
	/**
	 * 检查坐标是否在区域内
	 */
	virtual bool checkIn(int x,int y);
	std::vector<CCPoint> positions; // 点集合
}; // 角色信息 展示装备 和 文字信息 展示具体的信息

/**
 * 横轴方向滚动展示信息
 */
class RpgAdSysUI:public CCNode{
public:
	static RpgAdSysUI* create();
	bool init();
	/**
	 * 定时器
	 */
	void tick();

	void addContent(const std::string& content);
private:
	std::list<CCLabelTTF*> infos;
	std::vector<CCLabelTTF*> temps;
}; // 系统公告展示

/**
 * 快速攻击条目
 */
class RpgQuickItem:public UICenterItem{
public:
	RpgQuickItem()
	{
		quickType = 0;
		skillData = NULL;
		invalid = false;
		cdtime = 1.0f;
	}
	int cdtime;
	int quickType; // 快速类型
	std::string skillName; // 技能名字
	RpgSkillData *skillData; // 技能数据
	static RpgQuickItem* create(int quickId);
	static RpgQuickItem * create(const char *upName,const char * downName,const CCSize & rect);
	bool invalid; // 当前处在无效的状态
	/**
	 * 展示CD 特效
	 */
	void startShowCD();
	void cdTimeEnd(CCNode *pSender,void *arg);
};
class RpgQuickOPUI:public UICenterBag,public script::tixmlCode{
public:
	// 初始就一个基本攻击 后面可以点击道具到快捷栏 可以是药品 或者 技能 条目之间可以相互移动
	static RpgQuickOPUI * create(RpgPlayer *player);
	bool init(RpgPlayer *player);
	RpgPlayer *player;
	RpgQuickOPUI()
	{
		player = NULL;
		
	}
	/**
	 * 展示条目
	 * \param item 待展示的item 没有延时直接展示
	 */
	virtual void showItemWithXml(int dirType,UICenterItem *item);
	/**
	 * 从配置文件中加载配置
	 * \param node 配置根节点
	 */
	void takeNode(script::tixmlCodeNode *node);
	/**
	 * 展示条目
	 * \param item 待展示的item 没有延时直接展示
	 */
	virtual void showItem(int dirType,UICenterItem *item);

	void onClick();
	std::vector<CCPoint> pointList;
};// 攻击盘

class RpgSummonOPUI
{
public:
	// 从玩家包裹里读取可以召唤的道具 生成操作Item 点击后 在目标处寻找空闲区 存放
};// 召唤盘 与攻击盘不可同时展示
/**
 * 操作游戏的 
 * 1.关闭 
 * 2.展示人物的包裹 
 * 3.展示召唤盘
 */
class RpgSystemOPUI:public UICenterBag{
public:
	static RpgSystemOPUI* create(RpgUI *uiSys);
	bool init(RpgUI *uiSys);
	RpgSystemOPUI()
	{
		
	}
	void showItem(int dirType,UICenterItem *item);
}; // 系统操作盘

class RpgTalkNpcOPUI{

}; // NPC聊天操作盘

/**
 * 将系统中的怪物以红色点 用相对位置标识处理 中心点是玩家为中心
 */
class MiniMap:public CCSprite{
public:
	static MiniMap* create();
	bool init();
	void visit();
	bool checkTimeOut();
	cc_timeval lastTime;
};
class RpgMinMapUI:public CCNode{
public:
	static RpgMinMapUI* create(RpgMap *map);
	bool init(RpgMap *map);
	RpgMinMapUI()
	{
		map = NULL;
		mask = NULL;
		miniPlayer = NULL;
		target = NULL;
		miniMap = NULL;
	}
	void setSize(const CCSize &size);
	/**
	 * 更新点
	 */
	void updatePosition(const CCPoint &point);
	void visit();
private:
	MiniMap * miniMap;
	CCSprite * target;
	RpgMap *map;
	CCSprite *mask;
	CCSize minMapSize;
	CCSprite *miniPlayer;
}; // 小地图 
/**
 * 地图编辑器指令
 */
class RpgMapOPUI:public UICenterBag{
public:
	static RpgMapOPUI* create(RpgUI *uiSys);
	bool init(RpgUI *uiSys);
	RpgMapOPUI()
	{
		
	}
	void showItem(int dirType,UICenterItem *item);
}; // 地图编辑器

/**
 * 一轮结束后信息告示
 */
class RpgText:public CCNode{
public:
	static RpgText * create(script::tixmlCodeNode *node);
	static RpgText *create(const char *str,float width,float fontSize,const ccColor3B& color);
	bool init(const char *str,float width,float fontSize,const ccColor3B& color);
	void update(const std::string &content);
private:
	UIMultiLineArea* infoLabel;
	RpgText()
	{
		infoLabel = NULL;
	}
};
class RpgInfoUI:public CCNode,public script::tixmlCode{
public:
	/**
	 * 初始化
	 */
	bool init(const char *fileName);
	/**
	 * 处理节点
	 */
	void takeNode(script::tixmlCodeNode *node);
	std::map<std::string,RpgText*> updateContents;
	typedef std::map<std::string,RpgText*>::iterator UPDATE_CONTENTS_ITER;

	std::map<std::string,CCSprite*> sprites; // 精灵列表
	std::vector<RpgXmlBag*> bags;
	/**
	 * 更新name 指定的数值
	 */
	void updateNumber(const std::string& name,int number);

	void updateString(const std::string &name,const std::string& content);
	/**
	 * 控制每个tag的展示
	 */
	void show(const std::string &name,bool showTag);

	/**
	 * 展示
	 */
	void show();
	/**
	 * 隐藏
	 */
	void hide();
	/**
	 * 处理点击
	 **/
	bool doTouch(int touchType,const CCPoint &touchPoint);
    UICenterItem * getBtnByName(const std::string & name);
    std::map<std::string,UICenterItem*> buttons;
	//UICenterItem *close;
	const CCSize & getContentSize();
	CCSize backSize;
};

class RpgStepEndUI:public RpgInfoUI{
public:	
	/**
	 * 创建结束
	 */
	static RpgStepEndUI* create();
};

class RpgVisitUI:public RpgInfoUI{
public:
	/**
	 * 创建结束
	 */
	static RpgVisitUI* create();
};

class RpgScoreUI:public RpgInfoUI{
public:
	static RpgScoreUI*create();
};
/**
 * 道具信息说明
 */
class RpgItemDescUI:public RpgInfoUI{
public:
	/**
	 * 创建道具的描述
	 */
	static RpgItemDescUI *create();
	/**
	 * 展示道具
	 */
	void showItem(RpgItem *item);
	
};
/**
 * RPG 的UI系统
 */
class RpgUI:public UIBase{
public:
	/**
	 * 直接创建
	 */
	static RpgUI* create(RpgMap *map,RpgPlayer *player,CCNode *parent);
	/**
	 * 初始化 设定初始布局
	 **/
	bool init(RpgMap *map,RpgPlayer *player,CCNode *parent);
	/**
	 * 根据uiType 展示不同的UI
	 */
	void showUI(int uiType);
	
	//static RpgPlayer * gloabPlayer;
	enum{
		TOUCH_DOWN = 0, // 按下
		TOUCH_MOVE = 1, // 移动
		TOUCH_END = 2, // 结束
	};
	enum{
		UI_TYPE_MOVE, // 移动盘
		UI_TYPE_QUICK, // 快捷区 可以将不同类型的UI 放其上 然后点击出现该UI的事件响应 对应于Item
		UI_TYPE_SUMMON, // 召唤盘
		UI_TYPE_TALK, // 对话框
		UI_TYPE_PLAYER_INFO, // 角色信息区
		UI_TYPE_MINI_MAP, // 小地图
		UI_TYPE_NPC_TALK, // NPC聊天信息
		UI_TYPE_MAP_OP, // 地图操作
		UI_TYPE_BAG, // 人物包裹
		UI_TYPE_SHOP, // 商店
	};
	/**
	 * 处理点击
	 * \parma touchType 点击类型
	 * \param touchPoint 点击点
	 */
	bool doTouch(int touchType,CCTouch *touch);
/**
 * 系统中当前的界面元素
 **/
	RpgPlayer *player; // 当前角色
	RpgMap *map; // 地图
	RpgMoveOPUI *moveOP; // 移动盘
	RpgPlayerInfoUI *playerInfo; // 角色信息框
	RpgPlayerSimpleInfoUI *simplePlayerInfo;//简单角色信息
	RpgQuickOPUI *quickOP;// 快捷盘
	RpgSummonOPUI *summonOP; // 召唤盘
	RpgPlayerBag *playerBag; // 玩家包裹
	RpgSystemOPUI *systemOP; // 系统操作
	RpgMinMapUI *minMapInfo; // 小地图信息
	RpgTalkNpcOPUI *talkNpcOP; // 聊天操作盘
	RpgAdSysUI *adUI; // 广告节目
	RpgMapOPUI *mapOpUI;//地图操作器 
	RpgShop *shop; // 商店
	RpgStepEndUI *stepEnd;
	RpgVisitUI *visitUI; // 访问UI
	RpgScoreUI *scoreUI; // 积分UI
	RpgItemDescUI *descUI; // 描述UI
	RpgUI()
	{
		descUI = NULL;
		scoreUI = NULL;
		visitUI = NULL;
		stepEnd = NULL;
		simplePlayerInfo = NULL;
		moveOP = NULL;
		playerInfo = NULL;
		quickOP = NULL;
		summonOP = NULL;
		player = NULL;
		playerBag = NULL;
		systemOP = NULL;
		minMapInfo = NULL;
		talkNpcOP = NULL;
		adUI = NULL;
		mapOpUI = NULL;
		map = NULL;
		shop = NULL;
	}
	/**
	 * 执行定时行为
	 */
	void tick();
	void doItemDownPlayerBag(UIBase *base);

	void updateHp(RpgPlayer *player);

	void showItemDescInfo(RpgItem *item);
};
NS_CC_END