#include "main.h"
#include "AppDelegate.h"
#include "CCEGLView.h"

USING_NS_CC;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // create the application instance
    AppDelegate app;
    
	CCEGLView::sharedOpenGLView()->setFrameSize(960,640);
	CCSize winSize =  CCDirector::sharedDirector()->getWinSize();
    return CCApplication::sharedApplication()->run();
}
