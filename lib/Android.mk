LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_AAPT_FLAGS := --shared-lib
LOCAL_PACKAGE_NAME := SharedLibrary

LOCAL_EXPORT_PACKAGE_RESOURCES := true

LOCAL_MODULE_TAGS := optional
LOCAL_JNI_SHARED_LIBRARIES := libhello-jni
LOCAL_MULTILIB := 32
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_CERTIFICATE := platform
include $(BUILD_PACKAGE)

include $(LOCAL_PATH)/jni/Android.mk
include $(call all-makefiles-under, $(LOCAL_PATH))
