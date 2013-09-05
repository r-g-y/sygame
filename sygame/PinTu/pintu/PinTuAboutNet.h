#pragma once
#include "AboutRequest.h"
class ClientNode;
/**
 * 消息中心池
 */
class AboutNet:public MessageCenter,public Singleton<AboutNet>{
public:
    AboutNet(){}
    ~AboutNet(){destroy();}
    void init();
	bool connect();
	Socket * getSocket();
	ClientNode * client;
};
extern AboutNet theNet ;
