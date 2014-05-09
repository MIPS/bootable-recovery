ifneq ($(TARGET_SIMULATOR),true)
ifeq ($(TARGET_ARCH),mips)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := nand_utils.c

LOCAL_MODULE := libnand_utils

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := nand_test.c
LOCAL_MODULE := nand_test
LOCAL_MODULE_TAGS := optional
LOCAL_STATIC_LIBRARIES := libnand_utils libcutils libc
LOCAL_FORCE_STATIC_EXECUTABLE := true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES += system/core/mkbootimg
LOCAL_SRC_FILES := nand_flash_image.c
LOCAL_MODULE := nand_flash_image
LOCAL_MODULE_TAGS := optional
LOCAL_STATIC_LIBRARIES := libnand_utils libcutils libc
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)
include $(BUILD_EXECUTABLE)

endif	# TARGET_ARCH == mips
endif	# !TARGET_SIMULATOR
