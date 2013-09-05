#include "CartoonManager.h"
#include <pthread.h>
NS_CC_BEGIN
/**
* 获取动作信息 将实际填充CartoonInfo::frames 信息
*/
bool CartoonManager::getCartoonInfo(const std::string& name,int dir,CartoonInfo &info)
{
	return false;
}
/**
* 异步加载资源 开辟一个线程来加载
* 从 cartoon 中加载动画
* <pack name="animation.pack" cartooncount="100">
*      <cartoon name="" id=""><!--引索-->
*			<dir value="0" name=""/> <!--实际名字 方向+实际名字-->
*		</cartoon>
* </pack>
*/
void CartoonManager::loadSync(const char *configName,CartoonLoadCallback *back)
{
	pthread_t hid;
	pthread_create(&hid, NULL, threadLoadResource, this);
}
void* CartoonManager::threadLoadResource(void* object)
{
	CartoonManager * loader = (CartoonManager*) object;
	if (loader)
	{
		// 加载资源
		// 先加载 cartoon 生成cartoon 信息表
		// 再加载pack 信息 [较大]
		// 先读盘 每加载一个对象回调一次
		// thePack.setCallBack(PackLoadCallBack);
		// thePack.doCallback(count); // 当前对象的字节数
		// loadObject() 计数 + 1
	}
	return NULL;
}
NS_CC_END