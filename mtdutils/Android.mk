ifneq ($(TARGET_SIMULATOR),true)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_BUILD_MTDUTILS:=false
ifeq ($(TARGET_ARCH),arm)
LOCAL_BUILD_MTDUTILS:=true
endif
ifeq ($(TARGET_ARCH),mips)
LOCAL_BUILD_MTDUTILS:=true
endif

ifeq ($(LOCAL_BUILD_MTDUTILS),true)
LOCAL_SRC_FILES := \
	mtdutils.c \
	mounts.c

LOCAL_MODULE := libmtdutils

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := flash_image.c
LOCAL_MODULE := flash_image
LOCAL_MODULE_TAGS := eng
LOCAL_STATIC_LIBRARIES := libmtdutils
LOCAL_SHARED_LIBRARIES := libcutils libc
include $(BUILD_EXECUTABLE)

endif	# LOCAL_BUILD_MTDUTILS
endif	# !TARGET_SIMULATOR
