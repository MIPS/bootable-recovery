#
# Copyright (C) 2018 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PACKAGE_NAME := SystemUpdaterSampleTests
LOCAL_SDK_VERSION := system_current
LOCAL_MODULE_TAGS := tests
LOCAL_JAVA_LIBRARIES := \
    android.test.base.stubs \
    android.test.runner.stubs \
    guava \
    mockito-target-minus-junit4
LOCAL_STATIC_JAVA_LIBRARIES := android-support-test
LOCAL_INSTRUMENTATION_FOR := SystemUpdaterSample
LOCAL_PROGUARD_ENABLED := disabled

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res

LOCAL_SRC_FILES := $(call all-java-files-under, src)

include $(BUILD_PACKAGE)
