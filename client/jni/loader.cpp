/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Uncomment for verbose logging.
#define LOG_NDEBUG 0
#define LOG_TAG "testloader"

#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <jni.h>
#include <android/dlext.h>
#include <nativeloader/native_loader.h>
#include <utils/Log.h>

#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))

namespace android {
namespace {

jint LIBLOAD_SUCCESS;
jint LIBLOAD_FAILED_TO_OPEN_RELRO_FILE;
jint LIBLOAD_FAILED_TO_LOAD_LIBRARY;
jint LIBLOAD_FAILED_JNI_CALL;
jint LIBLOAD_FAILED_TO_FIND_NAMESPACE;


jint DoLoadWithRelroFile(JNIEnv* env, const char* lib, const char* relro,
                         jobject clazzLoader) {
    ALOGE("MapleStory DoLoadWithRelroFile lib: %s", lib);
    ALOGE("MapleStory DoLoadWithRelroFile relro: %s", relro);

  android_namespace_t* ns =
      android::FindNamespaceByClassLoader(env, clazzLoader);
    ALOGE("MapleStory DoLoadWithRelroFile get classloader namespace");
  if (ns == NULL) {
    ALOGE("Failed to find classloader namespace");
    return LIBLOAD_FAILED_TO_FIND_NAMESPACE;
  }
  android_dlextinfo extinfo;
  extinfo.flags = ANDROID_DLEXT_RESERVED_ADDRESS | ANDROID_DLEXT_USE_RELRO |
                  ANDROID_DLEXT_USE_NAMESPACE;
  extinfo.library_namespace = ns;
  void* handle = android_dlopen_ext(lib, RTLD_NOW, &extinfo);
  if (handle == NULL) {
    ALOGE("MapleStory Failed to load library");
    return LIBLOAD_FAILED_TO_LOAD_LIBRARY;
  }
  ALOGV("MapleStory Success to load library");
  return LIBLOAD_SUCCESS;
}

jint LoadWithRelroFile(JNIEnv* env, jclass, jstring lib32, jstring lib64,
                       jstring relro32, jstring relro64, jobject clazzLoader) {
                         #ifdef __LP64__
                           jstring lib = lib64;
                           jstring relro = relro64;
                           (void)lib32; (void)relro32;
                         #else
                           jstring lib = lib32;
                           jstring relro = relro32;
                           (void)lib64; (void)relro64;
                         #endif
                           jint ret = LIBLOAD_FAILED_JNI_CALL;
                           const char* lib_utf8 = env->GetStringUTFChars(lib, NULL);
                           if (lib_utf8 != NULL) {
                             const char* relro_utf8 = env->GetStringUTFChars(relro, NULL);
                             if (relro_utf8 != NULL) {
                               ret = DoLoadWithRelroFile(env, lib_utf8, relro_utf8, clazzLoader);
                               env->ReleaseStringUTFChars(relro, relro_utf8);
                             }
                             env->ReleaseStringUTFChars(lib, lib_utf8);
                           }
                           return ret;
}


const char kClassName[] = "com/google/android/test/lib_client/ActivityMain";
const JNINativeMethod kJniMethods[] = {
  { "nativeLoadWithRelroFile",
      "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)I",
      reinterpret_cast<void*>(LoadWithRelroFile) },
};

}  // namespace

void RegisterWebViewFactory(JNIEnv* env) {
  // If either of these fail, it will set an exception that will be thrown on
  // return, so no need to handle errors here.
  jclass clazz = env->FindClass(kClassName);
  if (clazz) {
    env->RegisterNatives(clazz, kJniMethods, NELEM(kJniMethods));
  }
}

}  // namespace android

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*) {
  JNIEnv* env = NULL;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    ALOGE("GetEnv failed");
    return JNI_ERR;
  }
  android::RegisterWebViewFactory(env);
  return JNI_VERSION_1_6;
}
