#pragma once
#include "mcenter.h"
#include "singleton.h"
struct NodeMsgCenter:public MessageCenter,public Singleton<NodeMsgCenter>{
   DYNAMIC_API  NodeMsgCenter(){init();}
   DYNAMIC_API ~NodeMsgCenter(){destroy();}
   DYNAMIC_API  void init();
};

#define theNodeMsgCenter NodeMsgCenter::getMe()
