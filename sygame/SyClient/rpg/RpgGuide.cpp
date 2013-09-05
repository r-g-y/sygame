#include "RpgGuide.h"

NS_CC_BEGIN

void RpgGuideStepManager::start(RpgGuide *guide,int guideId)
{
	if (!guide) return;
	if (steps.empty()) return;
	if (nowGuideId != guideId && guideId) return;
	nowGuideId = steps.size();
	// 根据当前顶部行为 执行相关动作
	RpgGuideStep &step = steps.front();
	int stepId = guide->getIndexByStepName(stepName);
	guide->doAction(step.actionId,stepId);
}

void RpgGuide::init(RpgPlayer *player,RpgMap *map,RpgUI *ui)
{
	this->_player = player;
	this->_map = map;
	this->_ui = ui;
	bindActions();
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData("guide.xml","rb",&nSize);
	if (!nSize)return;
	script::tixmlCode::initFromString((char*) buffer);
	// 从UserDefault 中加载数据
	std::string guideContent = CCUserDefault::sharedUserDefault()->getStringForKey("guideContent");
	if (!guideContent.empty())
	{
		serialize::Stream stream((void*)guideContent.c_str(),guideContent.size());
		stream.fromBase64();
		this->parseRecord(stream);
	}
}

void RpgGuide::doNextStep(unsigned int & stepId,int guideId)
{
	// 根据stepId 获取
	if (stepId == 0) return;
	RpgGuideStepManager * stepManager = getGuide(this->getStepNameByIndex(stepId - 1));
	if (stepManager && stepManager->steps.size())
	{
		stepManager->steps.pop_front();
		stepManager->start(this,guideId);
		save();
	}
	stepId = 0;
}

void RpgGuide::start(const std::string& stepName)
{
	RpgGuideStepManager * stepManager = getGuide(stepName);
	if (stepManager)
	{
		if (0 == stepManager->hadLoad)
		{
			STEPS_ITER iter = _xmlSteps.find(stepName);
			if (iter != _xmlSteps.end())
			{
				stepManager->hadLoad = 1;
				stepManager->stepName = iter->second.stepName;
				stepManager->steps = iter->second.steps;
			}
			
		}
		stepManager->start(this);
	}
	save();
}
void RpgGuide::showGuideDir(int dir,const CCPoint &point)
{
	if (guideSprite)
	{
		guideSprite->setVisible(true);
		guideSprite->setPosition(point);
	}
}
void RpgGuide::save()
{
	serialize::Stream stream = toRecord();
	if (stream.size())
	{
		stream.toBase64();
		CCUserDefault::sharedUserDefault()->setStringForKey("guideContent",stream.c_str());
	}
}

void RpgGuide::redo(std::string stepName)
{
	RpgGuideStepManager * stepManager = getGuide(stepName);
	if (stepManager)
	{
		STEPS_ITER iter = _xmlSteps.find(stepName);
		if (iter != _xmlSteps.end())
		{
			stepManager->hadLoad = 1;
			stepManager->stepName = iter->second.stepName;
			stepManager->steps = iter->second.steps;
			stepManager->start(this);
		}
	}
	save();
}
/**
 * 获取当前步骤的编号
 */
int RpgGuide::getIndexByStepName(const std::string& name)
{
	static std::string names[]={
		"welcome",
		""
	};
	int step = 0;
	while (true)
	{
		if (names[step] =="")
		{
			return -1;
		}
		else
		{
			if (names[step] == name)
			{
				return step;
			}
		}
		step++;
	}
}
std::string RpgGuide::getStepNameByIndex(int stepId)
{
	static std::string names[]={
		"welcome",
		""
	};
	if (stepId < 1)
	{
		return names[stepId];
	}
	return "";
}
/**
 * 从配置文件中解析内容
 */
void RpgGuide::takeNode(script::tixmlCodeNode *node)
{
	if (!node) return;
	if (node->equal("Config"))
	{
		/**
		 <stepAction name="welcome">
			<step actionname="Guide_step_1" delaytime=""/>
		 </stepAction>
		 */
		
		script::tixmlCodeNode stepActionNode = node->getFirstChildNode("stepAction");
		while (stepActionNode.isValid())
		{
			RpgGuideStepManager stepManager;
			stepManager.stepName = stepActionNode.getAttr("name");
			script::tixmlCodeNode stepNode = stepActionNode.getFirstChildNode("step");
			while (stepNode.isValid())
			{
				RpgGuideStep step;
				step.actionName = stepNode.getAttr("actionname");
				stepNode.getAttr("delaytime",step.delayTime);
				step.actionId = getGuideActionByName(step.actionName);
				stepManager.steps.push_back(step);
				stepNode = stepNode.getNextNode("step");
			}
			_xmlSteps[stepManager.stepName] = stepManager;
			stepActionNode = stepActionNode.getNextNode("stepAction");
		}
		script::tixmlCodeNode aniNode = node->getFirstChildNode("guideanimation");
		if(aniNode.isValid())
		{
			float needTime = 0;
			aniNode.getAttr("needtime",needTime);
			script::tixmlCodeNode fileNode = aniNode.getFirstChildNode("file");
			if (guideSprite)
			{
				guideSprite->removeFromParentAndCleanup(true);
			}
			guideSprite = CCSprite::create();
			CCAnimation* animation = CCAnimation::create();
			int  count = 0;
			while(fileNode.isValid())
			{
				std::string fileName = fileNode.getAttr("name");
				CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(fileName.c_str());
				count++;
				CCSpriteFrame *frame = CCSpriteFrame::frameWithTexture(texture,CCRectMake(0,0,texture->getContentSize().width,texture->getContentSize().height));
				animation->addSpriteFrame(frame);
				fileNode = fileNode.getNextNode("file");
			}
			if (count && guideSprite && _ui)
			{
				_ui->getParent()->addChild(guideSprite,100);
				animation->setDelayPerUnit(needTime / count);
				animation->setRestoreOriginalFrame(true);
				guideSprite->runAction(CCRepeatForever::create(CCAnimate::create(animation)));
				guideSprite->setVisible(false);
				guideSprite->setScale(0.5);
			}
		}
	}
}
int RpgGuide::getGuideActionByName(const std::string& name)
{
	std::map<std::string,int>::iterator iter =  stepNameIds.find(name);
	if (iter != stepNameIds.end())
	{
		return iter->second;
	}
	return -1;
}
void RpgGuide::setStepTag(CCObject *object,int stepID)
{
	object->m_uID = stepID+1;
	RpgGuideStepManager * stepManager = getGuide(this->getStepNameByIndex(stepID));
	if (stepManager && stepManager->steps.size())
	{
		object->m_nLuaID = stepManager->nowGuideId;
	}
}
/**
 * 不会新建引导类型
 */
RpgGuideStepManager * RpgGuide::findGuide(const std::string &stepName)
{
	STEPS_ITER iter = _steps.find(stepName);
	if (iter != _steps.end())
	{
		return &iter->second;
	}
	return NULL;
}
/**
 * 没有会新建引导类型
 */
RpgGuideStepManager * RpgGuide::getGuide(const std::string &stepName)
{
	RpgGuideStepManager *guide = findGuide(stepName);
	if (!guide)
	{
		RpgGuideStepManager stepManager;
		stepManager.stepName = stepName;
		_steps[stepName] =  stepManager;
	}
	return findGuide(stepName);
}

void RpgGuide::Guide_step_1(int stepID)
{
	if (_ui)
	{
		CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		this->showGuideDir(0,ccp(visibleSize.width - 145,84));
		setStepTag(_ui->quickOP,stepID); // 设置当前步行
	}
}
void RpgGuide::Guide_step_2(int stepID)
{
	if (_ui)
	{
		guideSprite->setVisible(true);
		guideSprite->setColor(ccc3(0,233,0));
		CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		this->showGuideDir(0,ccp(145,84));
		setStepTag(_ui->moveOP,stepID); // 设置当前步行
	}
}
void RpgGuide::Guide_step_3(int stepID)
{
	guideSprite->setVisible(false);
}
/**
 * 执行行为
 */
void RpgGuide::doAction(int actionId,int stepId)
{
	if (actionId < stepFunctions.size())
	{
		GUIDE_STEP_FUNCTION function = stepFunctions.at(actionId);
		(this->*function)(stepId);
	}
}
NS_CC_END