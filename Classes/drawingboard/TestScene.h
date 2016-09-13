#ifndef __TESTSCENE_H__
#define __TESTSCENE_H__

#include "GameGlobals.h"

class TestScene : public Layer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static Scene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(TestScene);
};

#endif // __TESTSCENE_H__
