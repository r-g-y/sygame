/**
 * MT 的行为
 */
#pragma once
#include "cocos2d.h"

NS_CC_BEGIN
class MTAction{
 public:
	 /**
	  * 以某个行为为初始执行状态
	  */
	 void start(int id)
	 {
		tryNextAction(id);
		CCFiniteTimeAction * action = makeAction(id);
		if (action)
			putAction(action);
	 }
	 void tryNextAction(int id)
	 {
		actionType = id;
	 }
	 /**
	  * 并行的执行行为
	  */
	 void tryConbineAction(int type)
	 {
		actionType |= type;
	 }
	 int actionType;
	 int oldActionType;
	 MTAction()
	 {
		actionType = 0;
		oldActionType = -1;
	 }
	 /**
	  * 检查当前行为
	  */
	 bool isNowAction(int actionType)
	 {
		if (oldActionType == -1) return false;
		return (oldActionType & actionType);
	 }
	 /**
	  * 下一个动作里去除该系
	  */
	 void clearAction(int type)
	 {
		actionType ^= type;
	 }
	 /**
	  * 根据优先级 生成相应的动作
	  */
	 void putConbineAction()
	 {
		
		for (int i = 0; i < 16;i++)
		{
			if (actionType & (1 << i))
			{
				CCFiniteTimeAction *action = makeAction(1<<i);
				if (action)
				{
					clearAction(1<<i);
					putAction(action);
					return ;
				}
			}
		}
		CCFiniteTimeAction *action = makeAction(1<<5);
		if (action)
		{
			clearAction(1<<5);
			putAction(action);
			return ;
		}
	 }
protected:
	 void setAction(int actionType)
	 {
		oldActionType = actionType;
	 }
	 virtual CCFiniteTimeAction* makeAction(int actionType)
	 {
		oldActionType = actionType; 
		return v_makeAction(actionType);
	 }
	 virtual void putAction(CCFiniteTimeAction *action)
	 {
		v_putAction(action);
	 }
	 virtual CCFiniteTimeAction* v_makeAction(int actionType) = 0;
	 virtual void v_putAction(CCFiniteTimeAction *action) = 0;
 };
NS_CC_END