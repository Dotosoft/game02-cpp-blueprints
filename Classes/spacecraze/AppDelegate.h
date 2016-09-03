#ifndef  _SPACECRAZE_APP_DELEGATE_H_
#define  _SPACECRAZE_APP_DELEGATE_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

/**
@brief    The cocos2d Application.

The reason for implement as private inheritance is to hide some interface call by CCDirector.
*/
namespace spacecraze {
	static cocos2d::Size designResolutionSize = cocos2d::Size(1280, 800);
	static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
	static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
	static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

	class  AppDelegate : private cocos2d::Application
	{
	public:
		AppDelegate();
		virtual ~AppDelegate();

		virtual void initGLContextAttrs();

		/**
		@brief    Implement Director and Scene init code here.
		@return true    Initialize success, app continue.
		@return false   Initialize failed, app terminate.
		*/
		virtual bool applicationDidFinishLaunching();

		/**
		@brief  The function be called when the application enter background
		@param  the pointer of the application
		*/
		virtual void applicationDidEnterBackground();

		/**
		@brief  The function be called when the application enter foreground
		@param  the pointer of the application
		*/
		virtual void applicationWillEnterForeground();
	};
}

#endif // _SPACECRAZE_APP_DELEGATE_H_

