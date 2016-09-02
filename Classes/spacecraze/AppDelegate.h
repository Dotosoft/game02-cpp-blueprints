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
	static cocos2d::Size designResolutionSize = cocos2d::Size(375, 620);	// 0.5625
																			// static cocos2d::Size designResolutionSize = cocos2d::Size(768, 1184);	// 0.5625
	static cocos2d::Size smallResolutionSize = cocos2d::Size(320, 480);
	static cocos2d::Size mediumResolutionSize = cocos2d::Size(768, 1024);
	static cocos2d::Size largeResolutionSize = cocos2d::Size(1536, 2048);

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

