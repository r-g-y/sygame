#pragma once
#include "cocos2d.h"
#include "xmlScript.h"
#include <map>
NS_CC_BEGIN
/**
 * 帧信息
 */
class TFFrameInfo{
public:
	int framesId; // 帧编号
	std::vector<std::string> pngs; // 图片集合
	void getFrames(std::vector<CCSpriteFrame*> &frames);
	CCParticleSystemQuad * particle; // 粒子系统
	std::vector<CCSpriteFrame*> frames;
	TFFrameInfo()
	{
		framesId = -1;
		particle = NULL;
	}
};
/**
 * 每个方向的信息
 */
class TFActionInfo{
public:
	int frameType; //帧类型
	int actionType; // 行为类型
	int actionId; // 行为id
	float needTime; // 帧播完的时间
	int direction; // 方向
	int framesId; // 帧号
	int conbineType; // 下一个串接类型
	int distance; // 作用距离
	int angle; // 旋转角度
	CCPoint offset; // 偏移
	CCPoint anchor; // 锚点
	enum{
		FRAME_SELF = 0,
		FRAME_OTHER = 1,
	};
	enum
	{
		ACTION_ORIGIN = 0, // 原地移动
		ACTION_FOLLOW = 1, // 跟随敌方
		ACTION_TO = 2, // 在某个点播放、
		ACTION_DIR = 3, // 向某个方向移动
		ACTION_REVERSE_DIR = 4, // 向某个方向 反方向移动
		ACTION_SPECIAL_ROTATE = 5, // 特殊效果 自身旋转
		ACTION_SPECIAL_SCALE = 6, // 特殊效果 自身缩放
		ACTION_PARTICLE_MOVE = 7, // 移动粒子
	};
	enum{
		CONBINE_SEQUENCE = 0, // 顺序
		CONBINE_SPAWN = 1, // 合并
	};
	TFActionInfo()
	{
		actionType = FRAME_SELF;
		framesId = -1;
		distance = 0;
		angle = 0;
	}
};
/**
 * 一个魔法分为3个阶段 起始 飞行 结束 
 */
class TFAction{
public:
	std::vector<std::map<int,TFActionInfo> > actions; // 方向 
	typedef std::vector<std::map<int,TFActionInfo> >::iterator ACTIONS_ITER;
	typedef std::map<int,TFActionInfo>::iterator ACTION_INFOS_ITER;
};
struct stLoadProcessCallBack{
	virtual void doLoad(int allCount,int nowIndex) = 0;
};
/**
 * 处理人物动作的加载
 */
class TFActionManager:public script::tixmlCode{
public:
	static TFActionManager& getMe()
	{
		static TFActionManager tfm;
		return tfm;
	}
	/**
	 * \param id 动作id
	 * \param dir 方向
	 * \param dest 目标位置
	 * \param target 目标
	 * \return 动作
	 */
	CCFiniteTimeAction * getAction(int id,int dir,const CCPoint &dest,CCNode *target);
	/**
	 * 从文件中初始化
	 */
	void initFromFile(const char *fileName,stLoadProcessCallBack *loadCallback = NULL);
	stLoadProcessCallBack * loadCallback;
private:
	/**
	 * 创建自身行为
	 */
	CCFiniteTimeAction * createSelfAction(TFActionInfo &actionInfo,const CCPoint &dest);
	/**
	 * 创建离身行为
	 */
	CCFiniteTimeAction * createFlyAction(TFActionInfo &actionInfo,const CCPoint &dest,CCNode *target);
	std::map<int,TFAction> _actions; // 行为集合
	typedef std::map<int,TFAction>::iterator ACTIONS_ITER;
	/**
	 * 从配置文件中加载配置
	 * \param node 配置根节点
	 */
	void takeNode(script::tixmlCodeNode *node);

	std::map<int,TFFrameInfo> frames; // 动作帧
	typedef std::map<int,TFFrameInfo>::iterator FRAMES_ITER;

	void loadParticeFromNode(CCParticleSystemQuad * particle,script::tixmlCodeNode &frameNode);

	void loadingCallBack(CCObject *obj);
};

NS_CC_END