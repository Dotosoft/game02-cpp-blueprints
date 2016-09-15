#include "AppDelegate.h"
#include "MainMenu.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
	// initialize director
	auto pDirector = Director::getInstance();
	auto pEGLView = pDirector->getOpenGLView();
	if (!pEGLView) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		pEGLView = GLViewImpl::createWithRect("Trampolin", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height), 1.0f, true);
#else
		pEGLView = GLViewImpl::create("Trampolin");
#endif
		pDirector->setOpenGLView(pEGLView);
	}

	FileUtils::getInstance()->addSearchPath("trampolin");
	
    // turn on display FPS
    //pDirector->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

	// Initialize global data
	GameGlobals::Init();

    // create a scene. it's an autorelease object
    CCScene *pScene = MainMenu::scene();

    // run
    pDirector->runWithScene(pScene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    CCDirector::sharedDirector()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    CCDirector::sharedDirector()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
