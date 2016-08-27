LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos/prebuilt-mk)

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

#LOCAL_SRC_FILES := hellocpp/main.cpp \
#                   ../../Classes/colorsmasher\AppDelegate.cpp \
#				   ../../Classes/colorsmasher\GameWorldScene.cpp \
#                   ../../Classes/colorsmasher\MainMenuScene.cpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/dragondash\AppDelegate.cpp \
				   ../../Classes/dragondash\DragonManager.cpp \
                   ../../Classes/dragondash\FairyTaleManager.cpp \
                   ../../Classes/dragondash\GameWorldScene.cpp \
                   ../../Classes/dragondash\MainMenuScene.cpp \
                   ../../Classes/dragondash\TowerManager.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

#$(call import-module,./prebuilt-mk)
$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
