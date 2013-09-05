#pragma once
#include "xmlScript.h"
#include "serialize.h"
#include "RpgPlayer.h"
#include "RpgMap.h"
#include "RpgUI.h"

NS_CC_BEGIN
/**
 * RPG的引导步骤
 */
class RpgGuideStep:public serialize::Object{
public:
	int actionId; // 执行号
	int delayTime; // 延时执行的时间
	std::string actionName; // 行为名字
	IMP_SERIALIZE_DATA_FUNC()
	{
		SERIALIZE_DATA(actionId,0);
		SERIALIZE_DATA(delayTime,1);
		SERIALIZE_DATA(actionName,2);
	}
	RpgGuideStep()
	{
		actionId = 0;
		delayTime = 0;
	}
};
class RpgGuide;
typedef void (RpgGuide::*GUIDE_STEP_FUNCTION)(int);
#define BIND_GUIDE_ACTION(functionName) \
	stepNameIds[#functionName] = stepFunctions.size();\
	stepFunctions.push_back(&RpgGuide::functionName);
/**
 * 引导执行
 */
class RpgGuideStepManager:public serialize::Object{
public:
	std::string stepName;
	int hadLoad; // 已经从配置中加载过?
	std::list<RpgGuideStep> steps; // 引导的步骤
	RpgGuideStepManager()
	{
		hadLoad = 0;
		nowGuideId = 0;
	}
	int nowGuideId; // 向导号 CCObject::m_nLuaID 被用作引导标示

	void start(RpgGuide *guide,int guideId = 0);

	IMP_SERIALIZE_DATA_FUNC()
	{
		SERIALIZE_DATA(stepName,0);
		SERIALIZE_DATA(hadLoad,1);
		SERIALIZE_DATA(steps,2);
		SERIALIZE_DATA(nowGuideId,3);
	}
};

/**
 * RPG向导
 */
class RpgGuide:public script::tixmlCode,public serialize::Object{
public:
	static RpgGuide & getMe()
	{
		static RpgGuide guide;
		return guide;
	}
	/**
	 * 初始化
	 */
	void init(RpgPlayer *player,RpgMap *map,RpgUI *ui);
	/**
	 * 执行下一个步骤
	 * 执行编号 引导编号
	 */
	void doNextStep(unsigned int & stepId,int guideId);
	/**
	 * 启动
	 */
	void start(const std::string& stepName);
	/**
	 * 存档
	 */
	void save();
	/**
	 * 重新开始引导
	 */
	void redo(std::string stepName);

	void takeNode(script::tixmlCodeNode *node);
	/**
	 * 不会新建引导类型
	 */
	RpgGuideStepManager * findGuide(const std::string &stepName);
	/**
	 * 没有会新建引导类型
	 */
	RpgGuideStepManager * getGuide(const std::string &stepName);
public:
	/**
	 * 绑定行为
	 */
	void bindActions()
	{
		BIND_GUIDE_ACTION(Guide_step_1);
		BIND_GUIDE_ACTION(Guide_step_2);
		BIND_GUIDE_ACTION(Guide_step_3);
	}
	/**
	 * 系统定义的执行步骤
	 */
	void Guide_step_1(int stepID);
	void Guide_step_2(int stepID);
	void Guide_step_3(int stepID);
	/**
	 * 获取当前步骤的编号
	 */
	int getIndexByStepName(const std::string& name);
	std::string getStepNameByIndex(int stepId);
	std::vector<GUIDE_STEP_FUNCTION> stepFunctions; // 步骤对应的行为
	std::map<std::string,int> stepNameIds; // 每步名字对应的ID

	int getGuideActionByName(const std::string& name);
	
	IMP_SERIALIZE_DATA_FUNC()
	{
		SERIALIZE_DATA(_steps,0);
	}
	/**
	 * 执行行为
	 */
	void doAction(int actionId,int stepId);
	CCSprite * guideSprite;

	void showGuideDir(int dir,const CCPoint &point);

	void setStepTag(CCObject *object,int stepID);
private:
	std::map<std::string,RpgGuideStepManager> _steps;
	std::map<std::string,RpgGuideStepManager> _xmlSteps;
	typedef std::map<std::string,RpgGuideStepManager>::iterator STEPS_ITER;
	RpgPlayer *_player;
	RpgMap *_map;
	RpgUI *_ui;
	RpgGuide()
	{
		_player = NULL;
		_map = NULL;
		_ui = NULL;
		guideSprite = NULL;
	}
};
NS_CC_END
#define theGuide RpgGuide::getMe()