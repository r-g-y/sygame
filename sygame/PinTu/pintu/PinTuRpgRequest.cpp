#include "AboutRequest.h"
#include "cocos2d.h"
/**
 * 请求场景的信息
 * \param mapWidth 地图的宽
 * \param mapHeight 地图的高
 */
IMP_REMOTE_FUNCTION(AboutRequest::getMapInfo)
	PARAM(int,mapWidth);
	PARAM(int ,mapHeight);
	BODY
	{
	}
REMOTE_FUNCTION_END


/**
 * 请求场景的信息
 * \param mapWidth 地图的宽
 * \param mapHeight 地图的高
 */
IMP_REMOTE_FUNCTION(AboutRequest::retMapInfo)
	PARAM(int,mapId);
	PARAM(std::string ,mapName);
	BODY
	{
		printf("%s",mapName.c_str());
	}
REMOTE_FUNCTION_END

/**
 * 请求登录
 */
IMP_REMOTE_FUNCTION(AboutRequest::reqLogin)
	PARAM(std::string,name);
	PARAM(std::string ,pwd);
	BODY
	{
	}
REMOTE_FUNCTION_END

/**
 * 返回场景信息集合
 */
IMP_REMOTE_FUNCTION(AboutRequest::retCanPlay)
	PARAM(std::string,name);
	PARAM(std::vector<stBagInfo>,bags);
	BODY
	{
		// 处理UI的展示
		// thePinTu->showBags(BagDatas);
	}
REMOTE_FUNCTION_END