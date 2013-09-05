#include "AboutRequest.h"
#include "socket.h"
/**
 * 请求场景的信息
 * \param mapWidth 地图的宽
 * \param mapHeight 地图的高
 */
IMP_REMOTE_FUNCTION(AboutRequest::retCanPlay)
	PARAM(std::vector<stBagInfo>,bags);
	BODY
	{
		printf("服务器本地调用");
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
		printf("服务器本地调用");
	}
REMOTE_FUNCTION_END

