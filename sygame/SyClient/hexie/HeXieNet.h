#pragma once
#include "socket.h"
#include "remotefunction.h"
#include <string>
#include "sys.h"
/**
 * 关于登陆流程
 * 客户端先向LoginCenter 请求有效的登陆节点,
 * LoginCenter 会在Login 创建驻留玩家
 * 根据创建效果,返回
 */
class HeXieNet:public RemoteFunction,public Singleton<HeXieNet>,public CallDelegate{
public:
	HeXieNet():CallDelegate("aboutnet")
	{
	
	}
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
};

#define theNet HeXieNet::getMe()