#include "AboutRequest.h"
#include "socket.h"
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
		printf("hello,world!");
		std::string content="test";
		theRequest.retMapInfo(socket,100,"test");
	}
REMOTE_FUNCTION_END

/**
 * 请求场景的信息
 * \param mapWidth 地图的宽
 * \param mapHeight 地图的高
 */
IMP_REMOTE_FUNCTION(AboutRequest::reqLogin)
	PARAM(std::string,name);
	PARAM(std::string,pwd);
	BODY
	{
		printf("reqLogin");
	}
REMOTE_FUNCTION_END

