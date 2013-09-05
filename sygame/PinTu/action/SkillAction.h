
#pragma once
#include "cocos2d.h"
#include "ModelLoader.h"
#include "AnimateMoveAction.h"
NS_CC_BEGIN
/**
 * 技能的子动作
 * 自身动作 , Action 的展示
 * 无相关动作
 */
class SkillAction:public AnimateMoveAction{
public:
	 static SkillAction* create(float duration, const CCPoint& position,const std::vector<CCSpriteFrame*> &frames,CCParticleSystemQuad *actionParticle = NULL);
	 static SkillAction* create(float duration,const CCPoint &src, const CCPoint& position,const std::vector<CCSpriteFrame*> &frames,CCParticleSystemQuad *actionParticle = NULL);
	 static SkillAction* create(float duration,const std::vector<CCSpriteFrame*> &frames);
	 static SkillAction* create(float duration,const std::vector<CCSpriteFrame*> &frames,CCNode *target);
	/**
	 * 更新
	 */
	 virtual void update(float t);
	 /**
	  * 启动
	  */
	 virtual void startWithTarget(CCNode *pTarget);
protected:
	bool hadFrom;
	bool hadEnd;
	// 新建一个CCSprite 用来承载动作
	CCSprite *actionSprite;
	CCParticleSystemQuad *actionParticle; // 跟随的粒子系统
	CCNode *target;
	/**
	 * 为使动作更加连续
	 */
	void stop(void);
	
	SkillAction()
	{
		actionSprite = NULL;
		actionParticle = NULL;
		hadFrom = false;
		hadEnd = true;
		target = NULL;
	}
	
};
/**
 * 技能 携带粒子系统的移动 
 */
class SkillParticleAction:public CCMoveTo{
public:
	virtual void update(float time);
	static SkillParticleAction* create(CCParticleSystemQuad *actionParticle,float duration, const CCPoint& dest);
	static SkillParticleAction* create(CCParticleSystemQuad *actionParticle,float duration, const CCPoint& src,const CCPoint &dest);
	void startWithTarget(CCNode *pTarget);
	void stop(void);
protected:
	CCParticleSystemQuad *actionParticle;
	bool hadFrom;
	SkillParticleAction()
	{
		hadFrom = false;
		actionParticle = NULL;
	}
};
/**
 * 向某个方向移动的技能
 */
class SkillMoveAction:public SkillAction{
public:
	static SkillMoveAction* create(float duration, const CCPoint& position,const std::vector<CCSpriteFrame*> &frames,float distance,int moveType);
	enum{
		TO_DIR, // 向正方向
		TO_REVERSE_DIR, // 向反方向
	};
	 /**
	  * 启动
	  */
	 void startWithTarget(CCNode *pTarget);
private:
	float _distance;
	float _dirType;
	SkillMoveAction()
	{
		_distance = 0;
		_dirType = 0;
	}
};
/**
 * 向某个方向旋转 可以指定anchorPoint
 */
class SkillRotateBy:public CCRotateBy{
public:
	 static SkillRotateBy* create(float duration, float fDeltaAngle,const CCPoint& newAnchorPoint,const CCPoint &offsetPoint);
	 void startWithTarget(CCNode *pTarget);
	 virtual void update(float time);
	 void stop(void);
private:
	CCPoint oldAnchorPoint;
	CCPoint nowPoint;
};
/**
 * 按比例缩放
 * 可以指定 anchor 和 播放偏移
 */
class SkillScaleTo:public CCScaleTo{
public:
	 static SkillScaleTo* create(float duration, float scale,const CCPoint& newAnchorPoint,const CCPoint &offsetPoint);
	 void startWithTarget(CCNode *pTarget);
	 virtual void update(float time);
	 void stop(void);
private:
	CCPoint oldAnchorPoint;
	CCPoint nowPoint;
};
NS_CC_END