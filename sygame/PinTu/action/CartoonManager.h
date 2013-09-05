#pragma once
#include "vector"
#include "Cartoon.h"
NS_CC_BEGIN
class CartoonLoadCallback{

};

/**
 * 动画管理器
 */
class CartoonManager{
public:
	/**
	 * 获取动作信息 将实际填充CartoonInfo::frames 信息
	 */
	bool getCartoonInfo(const std::string& name,int dir,CartoonInfo &info);
	/**
	 * 异步加载资源 开辟一个线程来加载
	 * 从 cartoon 中加载动画
	 * <pack name="animation.pack">
	 *      <cartoon name="" id=""><!--引索-->
	 *			<dir value="0" name=""/> <!--实际名字 方向+实际名字-->
	 *		</cartoon>
	 * </pack>
	 */
	void loadSync(const char *configName,CartoonLoadCallback *back = NULL);
	
	static void* threadLoadResource(void* object);
private:
	/**
	 * 引索名字 + [dir,cartoon]
	 **/
	std::map<std::string,std::map<int,CartoonInfo> > cartoonInfos;
};

NS_CC_END