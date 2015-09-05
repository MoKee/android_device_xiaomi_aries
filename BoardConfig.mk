#
# Copyright (C) 2011 The Android Open-Source Project
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

LOCAL_PATH := device/xiaomi/aries

# Vendor Init
TARGET_UNIFIED_DEVICE := true
TARGET_INIT_VENDOR_LIB := libinit_msm
TARGET_LIBINIT_DEFINES_FILE := device/xiaomi/aries/init/init_aries.c

TARGET_NO_RADIOIMAGE         := true
TARGET_BOOTLOADER_NAME       := aries
TARGET_NO_BOOTLOADER         := true
QCOM_BOARD_PLATFORMS         := msm8960
TARGET_BOARD_PLATFORM        := msm8960
TARGET_BOOTLOADER_BOARD_NAME := MSM8960

# Flags
TARGET_GLOBAL_CFLAGS += -mfpu=neon -mfloat-abi=softfp -DQCOM_HARDWARE
TARGET_GLOBAL_CPPFLAGS += -mfpu=neon -mfloat-abi=softfp -DQCOM_HARDWARE
COMMON_GLOBAL_CFLAGS += -D__ARM_USE_PLD -D__ARM_CACHE_LINE_SIZE=64

# Architecture
TARGET_ARCH_VARIANT_CPU      := cortex-a9
TARGET_CPU_ABI               := armeabi-v7a
TARGET_CPU_ABI2              := armeabi
TARGET_CPU_SMP               := true
TARGET_CPU_VARIANT           := krait
TARGET_ARCH                  := arm
TARGET_ARCH_VARIANT          := armv7-a-neon
ARCH_ARM_HAVE_TLS_REGISTER   := true
BOARD_USES_QCOM_HARDWARE     := true

# Krait optimizations
TARGET_USE_QCOM_BIONIC_OPTIMIZATION  := true
TARGET_USE_KRAIT_PLD_SET             := true
TARGET_KRAIT_BIONIC_PLDOFFS          := 10
TARGET_KRAIT_BIONIC_PLDTHRESH        := 10
TARGET_KRAIT_BIONIC_BBTHRESH         := 64
TARGET_KRAIT_BIONIC_PLDSIZE          := 64

BOARD_KERNEL_BASE     := 0x80200000
BOARD_KERNEL_PAGESIZE := 2048
BOARD_RAMDISK_OFFSET  := 0x02000000
BOARD_MKBOOTIMG_ARGS  := --ramdisk_offset 0x02000000
BOARD_KERNEL_CMDLINE  := console=null androidboot.hardware=qcom ehci-hcd.park=3 maxcpus=2 androidboot.bootdevice=msm_sdcc.1 androidboot.selinux=permissive

#kernel
TARGET_KERNEL_CONFIG := aries-perf-user_defconfig
TARGET_KERNEL_SOURCE := kernel/xiaomi/aries
TARGET_KERNEL_CUSTOM_TOOLCHAIN := arm-eabi-4.8

# Wifi
BOARD_HAS_QCOM_WLAN              := true
BOARD_WLAN_DEVICE                := qcwcn
WPA_SUPPLICANT_VERSION           := VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER      := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
BOARD_HOSTAPD_DRIVER             := NL80211
BOARD_HOSTAPD_PRIVATE_LIB        := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
WIFI_DRIVER_MODULE_PATH          := "/system/lib/modules/wlan.ko"
WIFI_DRIVER_MODULE_NAME          := "wlan"
WIFI_DRIVER_FW_PATH_STA          := "sta"
WIFI_DRIVER_FW_PATH_AP           := "ap"

# FM
COMMON_GLOBAL_CFLAGS += -DQCOM_FM_ENABLED
QCOM_FM_ENABLED := true
BOARD_HAVE_QCOM_FM := true
AUDIO_FEATURE_ENABLED_FM := true

TARGET_BOARD_INFO_FILE       := $(LOCAL_PATH)/board-info.txt

BOARD_EGL_CFG := $(LOCAL_PATH)/rootdir/etc/egl.cfg

TARGET_USES_QCOM_BSP        := true
TARGET_RELEASETOOLS_EXTENSIONS := $(LOCAL_PATH)

# QCOM enhanced A/V
TARGET_ENABLE_QC_AV_ENHANCEMENTS := true

# Display
TARGET_USES_ION             := true
USE_OPENGL_RENDERER         := true
TARGET_USES_C2D_COMPOSITION := true
NUM_FRAMEBUFFER_SURFACE_BUFFERS := 3

# Audio
BOARD_HAVE_AUDIENCE_ES310               := true
BOARD_HAVE_NEW_QCOM_CSDCLIENT := true
BOARD_HAVE_CSD_FAST_CALL_SWITCH := true
OVERRIDE_RS_DRIVER := libRSDriver_adreno.so
HAVE_ADRENO_SOURCE := false
TARGET_QCOM_MEDIA_VARIANT   := caf
TARGET_QCOM_DISPLAY_VARIANT := caf
TARGET_QCOM_AUDIO_VARIANT   := caf
BOARD_USES_ALSA_AUDIO                   := true
TARGET_USES_QCOM_MM_AUDIO               := true
TARGET_USES_QCOM_COMPRESSED_AUDIO       := true
BOARD_USES_LEGACY_ALSA_AUDIO            := true
QCOM_ANC_HEADSET_ENABLED := true
QCOM_FLUENCE_ENABLED := false
TUNNEL_MODE_SUPPORTS_AMRWB              := true
USE_TUNNEL_MODE                         := true
QCOM_TUNNEL_LPA_ENABLED := true
QCOM_PROXY_DEVICE_ENABLED := true
QCOM_ACDB_ENABLED := true
QCOM_AUDIO_FORMAT_ENABLED := true
QCOM_CSDCLIENT_ENABLED := true
AUDIO_FEATURE_ENABLED_PROXY_DEVICE := true
AUDIO_FEATURE_ENABLED_AUXPCM_BT := false
AUDIO_FEATURE_ENABLED_COMPRESS_VOIP := true
AUDIO_FEATURE_ENABLED_EXTN_FORMATS := true
QCOM_OUTPUT_FLAGS_ENABLED := true
QCOM_USBAUDIO_ENABLED := true
QCOM_FLUENCE_ENABLED := true
QCOM_MULTI_VOICE_SESSION_ENABLED := true

# Camera
COMMON_GLOBAL_CFLAGS       += -DQCOM_BSP

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(LOCAL_PATH)/bluetooth
BOARD_HAVE_BLUETOOTH                        := true
BOARD_HAVE_BLUETOOTH_QCOM                   := true
BLUETOOTH_HCI_USE_MCT                       := true

# Webkit
ENABLE_WEBGL            := true
TARGET_FORCE_CPU_UPLOAD := true

# Recovery
TARGET_RECOVERY_FSTAB            := $(LOCAL_PATH)/rootdir/ramdisk/fstab.qcom
RECOVERY_FSTAB_VERSION           := 2
TARGET_RECOVERY_PIXEL_FORMAT     := "RGBX_8888"
BOARD_HAS_NO_SELECT_BUTTON       := true

TARGET_USERIMAGES_USE_EXT4         := true
BOARD_BOOTIMAGE_PARTITION_SIZE     := 0x00A00000
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x00A00000
BOARD_SYSTEMIMAGE_PARTITION_SIZE   := 536870912
BOARD_USERDATAIMAGE_PARTITION_SIZE := 536870912
BOARD_PERSISTIMAGE_PARTITION_SIZE  := 8388608
BOARD_CACHEIMAGE_PARTITION_SIZE    := 402653184
BOARD_FLASH_BLOCK_SIZE             := 131072 # (BOARD_KERNEL_PAGESIZE * 64)

BOARD_USES_SECURE_SERVICES := true

USE_DEVICE_SPECIFIC_CAMERA:= true
USE_DEVICE_SPECIFIC_QCOM_PROPRIETARY:= true

HAVE_ADRENO_SOURCE:= false

SUPERUSER_EMBEDDED:= true

PRODUCT_BOOT_JARS += \
    qcom.fmradio \
    qcmediaplayer \
    org.codeaurora.Performance \
    tcmiface \
    dolby_ds

# Include an expanded selection of fonts
EXTENDED_FONT_FOOTPRINT := true

BOARD_USES_LEGACY_MMAP := true

MALLOC_IMPL := dlmalloc

PRODUCT_BUILD_GMS := false
TARGET_USES_LOGD := false

# Flags
COMMON_GLOBAL_CFLAGS += -DNO_SECURE_DISCARD

# MK Hardware
BOARD_HARDWARE_CLASS := device/xiaomi/aries/cmhw/

-include vendor/xiaomi/aries/BoardConfigVendor.mk

