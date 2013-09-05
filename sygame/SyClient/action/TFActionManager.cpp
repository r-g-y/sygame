#include "TFActionManager.h"
#include "AnimateMoveAction.h"
#include "SkillAction.h"
NS_CC_BEGIN
void TFFrameInfo::getFrames(std::vector<CCSpriteFrame*> &frames)
{
	frames.clear();
	for ( std::vector<std::string>::iterator iter = pngs.begin(); iter != pngs.end();++iter)
	{
		std::string &pngName = *iter;
		CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(pngName.c_str());
		
		CCSpriteFrame *frame = CCSpriteFrame::frameWithTexture(texture,CCRectMake(0,0,texture->getContentSize().width,texture->getContentSize().height));
		if (frame)
		{
			frames.push_back(frame);
			frame->retain();
			frame->setOffset(ccp(0,0));
		}
	}
}
/**
 * 获取该方向的一个行为
 */
CCFiniteTimeAction * TFActionManager::getAction(int id,int dir,const CCPoint &dest,CCNode *target)
{
	ACTIONS_ITER pos = _actions.find(id);
	if (pos != _actions.end())
	{
		CCFiniteTimeAction *preAction = NULL;
		CCFiniteTimeAction *nowAction = NULL;
		int conbineType = TFActionInfo::CONBINE_SEQUENCE;
		for ( TFAction::ACTIONS_ITER iter = pos->second.actions.begin(); iter != pos->second.actions.end(); ++iter)
		{
			TFAction::ACTION_INFOS_ITER it = iter->find(dir); // 获取该方向的infos
			if (it != iter->end())
			{
				TFActionInfo &info = it->second;
				nowAction = NULL;
				switch(info.frameType)
				{
					case TFActionInfo::FRAME_SELF:
					{
						nowAction = createSelfAction(info,dest);
					}break;
					case TFActionInfo::FRAME_OTHER:
					{
						nowAction = createFlyAction(info,dest,target);
					}break;
				}
				
				// 只有并行 和串行 两种可能 
				if (!preAction)
				{
					preAction = CCSequence::create(nowAction,NULL);
					conbineType = info.conbineType;
					continue;
				}
				else
				{
					if (conbineType == TFActionInfo::CONBINE_SEQUENCE)
					{
						preAction = CCSequence::create(preAction,nowAction,NULL);
					}
					else if (conbineType == TFActionInfo::CONBINE_SPAWN)
					{
						preAction = CCSpawn::create(preAction,nowAction,NULL);
					}
					conbineType = info.conbineType;
				}
			}
		}
		return preAction;
	}

	return NULL;
}
/**
 * 从文件中初始化
 */
void  TFActionManager::initFromFile(const char *fileName,stLoadProcessCallBack *loadCallback)
{
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData(fileName,"rb",&nSize);
	if (!nSize)return;
	this->loadCallback = loadCallback;
	script::tixmlCode::initFromString((char*)buffer);
}
/**
 * 更改自身面貌
 */
CCFiniteTimeAction * TFActionManager::createSelfAction(TFActionInfo &actionInfo,const CCPoint &dest)
{
	CCAnimation* animation = CCAnimation::create();
	FRAMES_ITER pos = frames.find(actionInfo.framesId);
	CCAnimate* animateAction = NULL;
	if (pos != frames.end())
	{
#define SELF_CAN_ACTION_TO 1
		std::vector<CCSpriteFrame*> &frames = pos->second.frames;
		pos->second.getFrames(frames) ;
#if (SELF_CAN_ACTION_TO)
		if (actionInfo.actionType == TFActionInfo::ACTION_ORIGIN)
#endif
		{
			for (int i = 0; i < frames.size(); i++)
			{
				animation->addSpriteFrame(frames[i]);
			}
			if (frames.empty()) return NULL;
			animation->setDelayPerUnit(actionInfo.needTime / frames.size());
			animation->setRestoreOriginalFrame(true);
			animateAction = CCAnimate::create(animation);
		}
#if (SELF_CAN_ACTION_TO)
		else if (actionInfo.actionType == TFActionInfo::ACTION_TO)
		{
			AnimateMoveAction *moveAction = AnimateMoveAction::create(actionInfo.needTime / frames.size(),dest,frames);
			return moveAction;
		}
		else if (actionInfo.actionType == TFActionInfo::ACTION_DIR)
		{
			ActionMoveDirAction * moveAction = ActionMoveDirAction::create(actionInfo.needTime / frames.size(),dest,frames,actionInfo.distance,ActionMoveDirAction::TO_DIR);
			return moveAction;
		}
		else if (actionInfo.actionType == TFActionInfo::ACTION_REVERSE_DIR)
		{
			ActionMoveDirAction * moveAction = ActionMoveDirAction::create(actionInfo.needTime / frames.size(),dest,frames,actionInfo.distance,ActionMoveDirAction::TO_REVERSE_DIR);
			return moveAction;
		}
		else if (actionInfo.actionType == TFActionInfo::ACTION_SPECIAL_ROTATE)
		{
			SkillRotateBy *roteby = SkillRotateBy::create(actionInfo.needTime,actionInfo.angle,actionInfo.anchor,actionInfo.offset);
			return roteby;
		}
		else if (actionInfo.actionType == TFActionInfo::ACTION_SPECIAL_SCALE)
		{
			SkillScaleTo *scaleTo = SkillScaleTo::create(actionInfo.needTime,actionInfo.angle,actionInfo.anchor,actionInfo.offset);
			return scaleTo;
		}	
#endif

	}
	return animateAction;
}
/**
 * 飞行的离身动作
 */
CCFiniteTimeAction *TFActionManager::createFlyAction(TFActionInfo &actionInfo,const CCPoint &dest,CCNode *target)
{
	SkillAction * flyAction = NULL;
	FRAMES_ITER pos = frames.find(actionInfo.framesId);
	if (pos != frames.end())
	{
		std::vector<CCSpriteFrame*> &frames = pos->second.frames;
		pos->second.getFrames(frames) ;
		if (actionInfo.actionType == TFActionInfo::ACTION_FOLLOW)
			flyAction = SkillAction::create(actionInfo.needTime / frames.size(),frames,target); // 随着target 移动
		else if (actionInfo.actionType == TFActionInfo::ACTION_ORIGIN)
		{
			flyAction = SkillAction::create(actionInfo.needTime / frames.size(),frames); // 本地移动
		}
		else if (actionInfo.actionType == TFActionInfo::ACTION_TO)
		{
			flyAction = SkillAction::create(actionInfo.needTime / frames.size(),dest,frames); // 移动到指定点
		}
		else if (actionInfo.actionType == TFActionInfo::ACTION_DIR)
		{
			flyAction = SkillMoveAction::create(actionInfo.needTime / frames.size(),dest,frames,actionInfo.distance,SkillMoveAction::TO_DIR);
			return flyAction;
		}
		else if (actionInfo.actionType == TFActionInfo::ACTION_REVERSE_DIR)
		{
			flyAction = SkillMoveAction::create(actionInfo.needTime / frames.size(),dest,frames,actionInfo.distance,SkillMoveAction::TO_REVERSE_DIR);
			return flyAction;
		}
		else if (actionInfo.actionType == TFActionInfo::ACTION_SPECIAL_ROTATE)
		{
			SkillRotateBy *roteby = SkillRotateBy::create(actionInfo.needTime,actionInfo.angle,actionInfo.anchor,actionInfo.offset);
			return roteby;
		}
		else if (actionInfo.actionType == TFActionInfo::ACTION_SPECIAL_SCALE)
		{
			SkillScaleTo *scaleTo = SkillScaleTo::create(actionInfo.needTime,actionInfo.angle,actionInfo.anchor,actionInfo.offset);
			return scaleTo;
		}
		else if (actionInfo.actionType == TFActionInfo::ACTION_PARTICLE_MOVE)
		{
			SkillParticleAction * particleTo = SkillParticleAction::create(pos->second.particle,actionInfo.needTime,dest);
			return particleTo;
		}
	}
	return flyAction;
}
/**
int frameType; //帧类型
	int actionType; // 行为类型
	int actionId; // 行为id
	int needTime; // 帧播完的时间
	int direction; // 方向
	int framesId; // 帧号
	int conbineType; // 下一个串接类型
*/
void TFActionManager::takeNode(script::tixmlCodeNode *node)
{
	if (!node) return;
	int index = 0;
	_actions.clear();
	if (node->equal("Config") && _actions.empty())
	{
		// 加载行为
		script::tixmlCodeNode actionsNode = node->getFirstChildNode("actions");
		int actionsIndex = 0;
		while (actionsNode.isValid())
		{
			actionsNode.getAttr("id",actionsIndex);
			TFAction action;
			script::tixmlCodeNode actionNode = actionsNode.getFirstChildNode("action");
			while (actionNode.isValid())
			{
				std::map<int,TFActionInfo> frames; // 8个方向的帧
				script::tixmlCodeNode framesNode = actionNode.getFirstChildNode("frames");
				while (framesNode.isValid())
				{
					TFActionInfo actionInfo;
					framesNode.getAttr("dir",actionInfo.direction); // 方向
					framesNode.getAttr("framesid",actionInfo.framesId); // 帧号
					framesNode.getAttr("conbinetype",actionInfo.conbineType); // 合并类型
					framesNode.getAttr("frametype",actionInfo.frameType); // 是否自身播放
					framesNode.getAttr("needtime",actionInfo.needTime); // 所需时间
					if (!actionInfo.needTime) actionInfo.needTime = 1; 
					framesNode.getAttr("actiontype",actionInfo.actionType); // 行为类型
					framesNode.getAttr("distance",actionInfo.distance);
					framesNode.getAttr("angle",actionInfo.angle);
					framesNode.getAttr("offsetx",actionInfo.offset.x);
					framesNode.getAttr("offsety",actionInfo.offset.y);
					framesNode.getAttr("anchorx",actionInfo.anchor.x);
					framesNode.getAttr("anchory",actionInfo.anchor.y);
					frames[actionInfo.direction] = actionInfo;
					framesNode = framesNode.getNextNode("frames");
				}
				action.actions.push_back(frames);
				actionNode = actionNode.getNextNode("action");
			}
			_actions[actionsIndex] = action; //获取到一个行为
			actionsNode = actionsNode.getNextNode("actions");
		}
		// 记载帧数
		script::tixmlCodeNode frameNode = node->getFirstChildNode("frames");
		while (frameNode.isValid())
		{
			std::string type = frameNode.getAttr("type");
			TFFrameInfo frameInfo;
			frameNode.getAttr("id",frameInfo.framesId);
			if (type == std::string("files"))
			{
				script::tixmlCodeNode fileNode = frameNode.getFirstChildNode("file");
				while (fileNode.isValid())
				{
					std::string pngName = fileNode.getAttr("value");
					frameInfo.pngs.push_back(pngName);
					
					CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(pngName.c_str());
				
					CCSpriteFrame *frame = CCSpriteFrame::frameWithTexture(texture,CCRectMake(0,0,texture->getContentSize().width,texture->getContentSize().height));
					if (frame)
					{
						frameInfo.frames.push_back(frame);
					//	frame->retain();
						frame->setOffset(ccp(0,0));
					}
					
					fileNode = fileNode.getNextNode("file");
				}
			}
			if (type == std::string("framecache"))
			{
				
				std::string plistName = frameNode.getAttr("plistname");
				std::string pngName = frameNode.getAttr("pngname");
				CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(plistName.c_str(), pngName.c_str());
				script::tixmlCodeNode fileNode = frameNode.getFirstChildNode("file");
				while (fileNode.isValid())
				{
					std::string value = fileNode.getAttr("value");
					CCSpriteFrame *frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(value.c_str());
					if (frame)
					{
						frameInfo.frames.push_back(frame);
					//	frame->retain();
					}
					
					fileNode = fileNode.getNextNode("file");
				}
				
			}
			if (type == std::string("particlexml"))
			{
				return;
				int numberOfParticles = 0;
				frameNode.getAttr("numberOfParticles",numberOfParticles);
				if (!numberOfParticles) numberOfParticles = 100;
				frameInfo.particle = CCParticleSystemQuad::createWithTotalParticles(numberOfParticles);
				std::string pngname = frameNode.getAttr("pngname");
				CCParticleSystemQuad *particle = frameInfo.particle;
			//	particle->retain();
				
				if( particle)
				{
					loadParticeFromNode(particle,frameNode);
					particle->setTexture( CCTextureCache::sharedTextureCache()->addImage(pngname.c_str() ));
				}
				
			}
			if (type == std::string("particleplist"))
			{
				
			}
			frames[frameInfo.framesId] = frameInfo;
			frameNode = frameNode.getNextNode("frames");

			if (loadCallback)
		    {
				loadCallback->doLoad(0,index++);
			}
		}
		
	}
}
void TFActionManager::loadingCallBack(CCObject *obj)
{
  
}
void TFActionManager::loadParticeFromNode(CCParticleSystemQuad * particle,script::tixmlCodeNode &frameNode)
{
	if (!particle) return;
	// duration
	particle->m_fDuration = kCCParticleDurationInfinity;
	std::string emitter = frameNode.getAttr("emitter");
	if (emitter == "gravity")
		// Emitter mode: Gravity Mode
		particle->m_nEmitterMode = kCCParticleModeGravity;
	else
		particle->m_nEmitterMode = kCCParticleModeRadius; // 默认是发散发射器
	script::tixmlCodeNode modeANode = frameNode.getFirstChildNode("modeA");
	if (modeANode.isValid())
	{
		script::tixmlCodeNode gravityNode = modeANode.getFirstChildNode("gravity");
		if (gravityNode.isValid())
		{
			// Gravity Mode: gravity
			//particle->modeA.gravity = ccp(0,0);
			gravityNode.getAttr("x",particle->modeA.gravity.x);
			gravityNode.getAttr("y",particle->modeA.gravity.y);
		}
		modeANode.getAttr("radialAccel",particle->modeA.radialAccel);
		modeANode.getAttr("radialAccelVar",particle->modeA.radialAccelVar);
		// Gravity Mode: radial acceleration
		//particle->modeA.radialAccel = 0;
		//particle->modeA.radialAccelVar = 0;

		// Gravity Mode: speed of particles
		//particle->modeA.speed = 25;
		modeANode.getAttr("speed",particle->modeA.speed);
		//particle->modeA.speedVar = 10;
		modeANode.getAttr("speedVar",particle->modeA.speedVar);
	}
	frameNode.getAttr("m_fAngle",particle->m_fAngle);
	frameNode.getAttr("m_fAngleVar",particle->m_fAngleVar);
	// angle
	//particle->m_fAngle = 90;
	//particle->m_fAngleVar = 5;

	// emitter position
	script::tixmlCodeNode m_tPosVarNode = frameNode.getFirstChildNode("m_tPosVar");
	if (m_tPosVarNode.isValid())
	{
		m_tPosVarNode.getAttr("x",particle->m_tPosVar.x);
		m_tPosVarNode.getAttr("y",particle->m_tPosVar.y);
	}
	//particle->m_tPosVar = ccp(20, 0);
	
	frameNode.getAttr("m_fLife",particle->m_fLife);
	frameNode.getAttr("m_fLifeVar",particle->m_fLifeVar);
	// life of particles
	//particle->m_fLife = 4;
	//particle->m_fLifeVar = 1;
	frameNode.getAttr("m_fStartSize",particle->m_fStartSize);
	frameNode.getAttr("m_fStartSizeVar",particle->m_fStartSizeVar);
	// size, in pixels
	//particle->m_fStartSize = 60.0f;
	//particle->m_fStartSizeVar = 10.0f;
	particle->m_fEndSize = kCCParticleStartSizeEqualToEndSize;

	// emits per frame
	particle->m_fEmissionRate = particle->m_uTotalParticles/particle->m_fLife;
	script::tixmlCodeNode m_tStartColorNode = frameNode.getFirstChildNode("m_tStartColor");
	if (m_tStartColorNode.isValid())
	{
		m_tStartColorNode.getAttr("r",particle->m_tStartColor.r);
		m_tStartColorNode.getAttr("g",particle->m_tStartColor.g);
		m_tStartColorNode.getAttr("b",particle->m_tStartColor.b);
		m_tStartColorNode.getAttr("a",particle->m_tStartColor.a);
	}
	// color of particles
	//particle->m_tStartColor.r = 0.8f;
	//particle->m_tStartColor.g = 0.8f;
	//particle->m_tStartColor.b = 0.8f;
	//particle->m_tStartColor.a = 1.0f;

	script::tixmlCodeNode m_tStartColorVarNode = frameNode.getFirstChildNode("m_tStartColorVar");
	if (m_tStartColorVarNode.isValid())
	{
		m_tStartColorVarNode.getAttr("r",particle->m_tStartColorVar.r);
		m_tStartColorVarNode.getAttr("g",particle->m_tStartColorVar.g);
		m_tStartColorVarNode.getAttr("b",particle->m_tStartColorVar.b);
		m_tStartColorVarNode.getAttr("a",particle->m_tStartColorVar.a);
	}
	//particle->m_tStartColorVar.r = 0.02f;
	//particle->m_tStartColorVar.g = 0.02f;
	//particle->m_tStartColorVar.b = 0.02f;
	//particle->m_tStartColorVar.a = 0.0f;
	script::tixmlCodeNode m_tEndColorNode = frameNode.getFirstChildNode("m_tEndColor");
	if (m_tEndColorNode.isValid())
	{
		m_tEndColorNode.getAttr("r",particle->m_tEndColor.r);
		m_tEndColorNode.getAttr("g",particle->m_tEndColor.g);
		m_tEndColorNode.getAttr("b",particle->m_tEndColor.b);
		m_tEndColorNode.getAttr("a",particle->m_tEndColor.a);
	}
	//particle->m_tEndColor.r = 0.0f;
	//particle->m_tEndColor.g = 0.0f;
	//particle->m_tEndColor.b = 0.0f;
	//particle->m_tEndColor.a = 1.0f;
	script::tixmlCodeNode m_tEndColorVarNode = frameNode.getFirstChildNode("m_tEndColorVar");
	if (m_tEndColorVarNode.isValid())
	{
		m_tEndColorVarNode.getAttr("r",particle->m_tEndColorVar.r);
		m_tEndColorVarNode.getAttr("g",particle->m_tEndColorVar.g);
		m_tEndColorVarNode.getAttr("b",particle->m_tEndColorVar.b);
		m_tEndColorVarNode.getAttr("a",particle->m_tEndColorVar.a);
	}
	//particle->m_tEndColorVar.r = 0.0f;
	//particle->m_tEndColorVar.g = 0.0f;
	//particle->m_tEndColorVar.b = 0.0f;
	//particle->m_tEndColorVar.a = 0.0f;

	// additive
	std::string blendAdditiveStr = frameNode.getAttr("blendAdditive");
	if (blendAdditiveStr == "false")
		particle->setBlendAdditive(false);
	else
		particle->setBlendAdditive(true);
}

NS_CC_END