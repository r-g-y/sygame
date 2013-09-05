#pragma once
#include "cocos2d.h"
#include "xmlScript.h"
struct stSoudInfo{
	std::string fileInfo;
	int type;
	int soundId;
	enum{
		EFFECT = 0,
		BACKGROUD =1,
	};
	stSoudInfo()
	{
		soundId = 0;
		type = 0;
	}
};
class SoudManager:public script::tixmlCode{
public:
	static SoudManager & getMe()
	{
		static SoudManager sm;
		return sm;
	}
	std::map<std::string,stSoudInfo> oddsFiles;
	typedef std::map<std::string,stSoudInfo>::iterator ODDS_FILES_ITER;
	void play(const std::string& odds);
	bool init();
	void takeNode(script::tixmlCodeNode *node);
};

#define theSound SoudManager::getMe()