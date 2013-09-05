#include "SoudManager.h"

#include "SimpleAudioEngine.h"

using namespace CocosDenshion;
using namespace cocos2d;
void SoudManager::play(const std::string& odds)
{
	ODDS_FILES_ITER iter = oddsFiles.find(odds);
	if (iter != oddsFiles.end())
	{
		if (iter->second.type == 0)
		{
			if (!iter->second.soundId)
			{
				SimpleAudioEngine::sharedEngine()->stopEffect(iter->second.soundId);
			}
			iter->second.soundId = SimpleAudioEngine::sharedEngine()->playEffect(
					std::string(CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(iter->second.fileInfo.c_str())).c_str());
		}
		else
		{
			SimpleAudioEngine::sharedEngine()->playBackgroundMusic(
				std::string(CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(iter->second.fileInfo.c_str())).c_str(), true);
		}
	}
}
bool SoudManager::init()
{
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData("sound.xml","rb",&nSize);
	if (!nSize)return false;
	if (script::tixmlCode::initFromString((char*) buffer))
	{
		for (ODDS_FILES_ITER iter = oddsFiles.begin(); iter != oddsFiles.end();++iter)
		{
			if (iter->second.type == 1)
			{
				 SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic( 
					 CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(iter->second.fileInfo.c_str()).c_str() );
			}
			else
			{
				SimpleAudioEngine::sharedEngine()->preloadEffect( 
					CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(iter->second.fileInfo.c_str()).c_str());
		
			}
		}
		SimpleAudioEngine::sharedEngine()->setEffectsVolume(1);
		SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(1);
		return true;
	}
	return false;
}
void SoudManager::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
		script::tixmlCodeNode soudNode = node->getFirstChildNode("sound");
		while(soudNode.isValid())
		{
			stSoudInfo info;
			std::string oddName = soudNode.getAttr("name");
			info.fileInfo = soudNode.getAttr("file");
			info.type = soudNode.getInt("type");
			oddsFiles[oddName] = info;
			soudNode = soudNode.getNextNode("sound");
		}
	}
}