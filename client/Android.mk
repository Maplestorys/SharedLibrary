LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_RES_LIBRARIES := SharedLibrary

LOCAL_PACKAGE_NAME := SharedLibraryClient

LOCAL_MODULE_TAGS := optional

LOCAL_JNI_SHARED_LIBRARIES := libloader

LOCAL_MULTILIB := 32

LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_PACKAGE)
