#pragma once
#include "remotefunction.h"
#include <string>
#include "AboutData.h"
class AboutRequest:public RemoteFunction,public Singleton<AboutRequest>,public CallDelegate{
public:
	MAKE_REMOTE_CLASS(AboutRequest)

	/**
	 * 请求场景的信息
	 * \param mapWidth 地图的宽
	 * \param mapHeight 地图的高
	 */
	DEC_REMOTE_FUNCTION_2(getMapInfo,
		int mapWidth,int mapHeight)
	{
		PRE_CALL_2(getMapInfo,mapWidth,mapHeight);
	}
	/**
	 * 返回场景信息
	 */
	DEC_REMOTE_FUNCTION_2(retMapInfo,int mapId,const std::string& mapName)
	{
		PRE_CALL_2(retMapInfo,mapId,mapName);
	}

	/**
	 * 请求登录进系统
	 */
	DEC_REMOTE_FUNCTION_2(reqLogin,const std::string &name,const std::string &pwd)
	{
		PRE_CALL_2(reqLogin,name,pwd);
	}
	/**
	 * 返回当前可以玩的信息
	 */
	DEC_REMOTE_FUNCTION_1(retCanPlay,std::vector<stBagInfo>& bags)
	{
		PRE_CALL_1(retCanPlay,bags);
	}
};


#define theRequest AboutRequest::getMe()