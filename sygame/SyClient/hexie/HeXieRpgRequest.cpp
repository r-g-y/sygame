#include "AboutRequest.h"
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