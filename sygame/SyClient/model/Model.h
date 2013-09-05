#pragma once
#include "cocos2d.h"
#include "ModelLoader.h"
NS_CC_BEGIN
struct stModelExecCharStar{
public:
	virtual void exec(const char *name) = 0;
};
/**
 * 主要是继承于Sprite 
 *		可以walkDir(int dir) 向某个方向走一步 ModelMoveAction
 */
class Model:public CCSprite{
public:
	CREATE_FUNC(Model);
	static Model * createWithModelName(const char *name);
	static Model * createWithModeXmlName(const char *name);
	/**
	 * 模型默认初始化
	 */
	bool init();
	/**
	 * 从xml 文件中加载模型数据 当模型数据存在时 不加载
	 */
	bool initWithXml(const  char *name);
	/**
	 * 从模型数据中加载
	 */
	bool initWithModelName(const char*name);
	/**
	 * 通过方向 和 id 获取移动纹理
	 */
	CCTexture2D *getMoveTexutueByDirAndFrameId(int dir,int nFrame);
	/**
	 * 更新帧的信息
	 */
	void updateFrame(int id,const char *pngName);
	/**
	 * 更新帧的数据
	 **/
	void updateFrame(const char *state,const char *dir,int frameId,const char *pngName);
	static const int DIR_COUNT = 8; // 标识8方向
	static const int STATE_COUNT = 4; // 标识状态数量
	static const int DIR_FRAMES = 8; // 每个方向的帧数
	static int getIdByActionName(const char*state,const char *dir);

	static void execEachState(stModelExecCharStar * exec);
	static void execEachDir(stModelExecCharStar * exec);

	/**
	 * 通过内置的方向枚举获取方向名字
	 */
	static std::string getDirNameByDirEnum(int dir);

	static int getDirEnumNyName(const std::string &dirName);
public:
	/**
	 * 停在在某个方向
	 **/
	void stopDir(const char *dir);
	/**
	 * 向一个方向移动一步
	 * 会自动执行一个moveAction
	 */
	void moveDir(int dir);

	void moveDir(const std::string& dirName);
	/**
	 * 判断是否到终点
	 */
	void moveActionEnd();
private:
	int step;
	ModelData *data; // 模型的数据
	std::string name; // 模型的名字
	CCPoint nowPosition; // 当前所在的网格位置
	Model()
	{
		data = NULL;
		step = 45;
	}
};
NS_CC_END