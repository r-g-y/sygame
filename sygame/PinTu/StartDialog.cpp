#include "StartDialog.h"
#include "UIButton.h"
#include "UIWindow.h"
NS_CC_BEGIN
bool StartDialog::initFromFile(const char *fileName)
{
	unsigned long nSize = 0;
	unsigned char * buffer = CCFileUtils::sharedFileUtils()->getFileData(fileName,"rb",&nSize);
	if (!nSize)return false;
	if (script::tixmlCode::initFromString((char*)buffer))
	{
		doInitEvent();
		return true;
	}
	return false;
}
/**
	* 从配置文件中加载配置
	* \param node 配置根节点
	*/
void StartDialog::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("Config"))
	{
 		// 加载Label TextArea Button 生成界面系统
		// 创建欢迎动画
		script::tixmlCodeNode mainNode = node->getFirstChildNode("start");
		if (mainNode.isValid() && !mainDialog)
		{
			mainDialog = UIPanel::createFromNode(&mainNode);
			window->addPanel(mainDialog);
			mainDialog->setZOrder(12);
		}
	}
}

void StartDialog::doInitEvent()
{
	
	CCSize s = CCDirector::sharedDirector()->getWinSize();
   
	do{
		GET_UI_BYNAME(mainDialog,UIButton,reqLoginBtn,"login");
		if (reqLoginBtn)
		{
			struct stReqLogin:public UICallback{
				void callback(UIBase *base)
				{
					// 尝试读取系统中的 登录名 登录密码 并发送到网络
					// 在Request.cpp 中创建场景
					// 创建等待动画
					//theNet.reqLogin("test","123456"); 尝试登录进系统
				}
				stReqLogin(UIWindow *window):window(window)
				{
				
				}
				UIWindow *window;
			};
			reqLoginBtn->bind(UIBase::EVENT_CLICK_DOWN,new stReqLogin(window));
		}
		
	} while (false);
}


NS_CC_END
