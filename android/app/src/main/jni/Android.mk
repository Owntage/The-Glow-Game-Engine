LOCAL_PATH := $(call my-dir)

LS_CPP=$(subst $(1)/,,$(wildcard $(1)/$(2)/*.cpp))
BOX2D_CPP:= $(call LS_CPP,$(LOCAL_PATH),Box2D/Collision) \
            $(call LS_CPP,$(LOCAL_PATH),Box2D/Collision/Shapes) \
            $(call LS_CPP,$(LOCAL_PATH),Box2D/Common) \
            $(call LS_CPP,$(LOCAL_PATH),Box2D/Dynamics) \
            $(call LS_CPP,$(LOCAL_PATH),Box2D/Dynamics/Contacts) \
            $(call LS_CPP,$(LOCAL_PATH),Box2D/Dynamics/Joints) \
            $(call LS_CPP,$(LOCAL_PATH),Box2D/Rope)

include $(CLEAR_VARS)

LOCAL_MODULE:= box2d_static
LOCAL_SRC_FILES:= $(BOX2D_CPP)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/Box2D
LOCAL_C_INCLUDES := $(LOCAL_EXPORT_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE    := sfml-example

LOCAL_SRC_FILES := $(call LS_CPP,$(LOCAL_PATH),../../../../../source) \
					$(call LS_CPP,$(LOCAL_PATH),../../../../../source/components)

LOCAL_C_INCLUDES := $(wildcard $(LOCAL_PATH)/../../../../../android-include)
LOCAL_CPP_FEATURES := rtti
LOCAL_CPP_FEATURES += exceptions

LOCAL_SHARED_LIBRARIES := sfml-system-d
LOCAL_SHARED_LIBRARIES += sfml-window-d
LOCAL_SHARED_LIBRARIES += sfml-graphics-d
LOCAL_SHARED_LIBRARIES += sfml-audio-d
LOCAL_SHARED_LIBRARIES += sfml-network-d
LOCAL_SHARED_LIBRARIES += sfml-activity-d
LOCAL_SHARED_LIBRARIES += openal
LOCAL_WHOLE_STATIC_LIBRARIES := sfml-main-d

LOCAL_STATIC_LIBRARIES := box2d_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,third_party/sfml)
