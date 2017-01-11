/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.google.android.test.lib_client;

import android.app.Activity;
import android.os.Bundle;
import com.google.android.test.shared_library.SharedLibraryMain;
import android.app.AppGlobals;
import android.annotation.SystemApi;
import android.app.ActivityManagerInternal;
import android.app.ActivityManagerNative;
import android.app.AppGlobals;
import android.app.Application;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.os.Build;
import android.os.Process;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.StrictMode;
import android.os.SystemProperties;
import android.os.Trace;
import android.text.TextUtils;
import android.util.AndroidRuntimeException;
import android.util.ArraySet;
import android.util.Log;

import com.android.server.LocalServices;

import dalvik.system.VMRuntime;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public class ActivityMain extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        try {
          System.loadLibrary("loader");

          Application initialApplication = AppGlobals.getInitialApplication();
          PackageInfo newPackageInfo = initialApplication.getPackageManager().getPackageInfo(
              "com.google.android.test.shared_library",
              PackageManager.GET_SHARED_LIBRARY_FILES
              | PackageManager.MATCH_DEBUG_TRIAGED_MISSING
              // Make sure that we fetch the current provider even if its not
              // installed for the current user
              | PackageManager.MATCH_UNINSTALLED_PACKAGES
              // Fetch signatures for verification
              | PackageManager.GET_SIGNATURES
              // Get meta-data for meta data flag verification
              | PackageManager.GET_META_DATA);
          Context libcontext = initialApplication.createApplicationContext(
                  newPackageInfo.applicationInfo,
                  Context.CONTEXT_INCLUDE_CODE | Context.CONTEXT_IGNORE_SECURITY);
          ClassLoader clazzLoader = libcontext.getClassLoader();
          System.out.println("MapleStory first Load");
          nativeLoadWithRelroFile("/system/app/SharedLibrary/SharedLibrary.apk!/lib/armeabi-v7a/libhello-jni.so", "/system/app/SharedLibrary/SharedLibrary.apk!/lib/armeabi-v7a/libhello-jni.so","relro32", "relro64", clazzLoader);
          System.out.println("MapleStory second Load");
          nativeLoadWithRelroFile("/system/app/SharedLibrary/lib/arm/libhello-jni.so", "/system/app/SharedLibrary/lib/arm/libhello-jni.so","relro32", "relro64", clazzLoader);

          try {
          //  System.out.println(com.google.android.test.shared_library.ActivityMain.stringFromJNI());
          }catch(Exception e) {
            e.printStackTrace();
          }
        }catch(Exception e) {
          e.printStackTrace();
        }

        String[] expectedAnimals = new String[] {
                "Racoon",
                "Rhino",
                "Elephant"
        };
		//System.out.println("HelloJni.stringFromJNI()" + com.google.android.test.shared_library.ActivityMain.stringFromJNI());
        String[] animals = getResources().getStringArray(com.google.android.test.shared_library.R.array.animals);
        if (animals == null || animals.length != expectedAnimals.length) {
            throw new AssertionError("Animal list from shared library is null or wrong length.");
        }

        for (int i = 0; i < expectedAnimals.length; i++) {
            if (!expectedAnimals[i].equals(animals[i])) {
                throw new AssertionError("Expected '" + expectedAnimals[i]
                        + "' at index " + i + " but got '" + animals[i]);
            }
        }

        SharedLibraryMain.ensureVersion(this, SharedLibraryMain.VERSION_BASE);
    }

    private static native int nativeLoadWithRelroFile(String lib32, String lib64,
                                                      String relro32, String relro64,
                                                      ClassLoader clazzLoader);
}
