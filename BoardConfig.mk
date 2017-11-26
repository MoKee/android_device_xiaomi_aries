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

# Bootloader
TARGET_NO_RADIOIMAGE              := true
TARGET_NO_BOOTLOADER.             := true
TARGET_BOOTLOADER_NAME            := aries
TARGET_BOARD_INFO_FILE            := $(LOCAL_PATH)/board-info.txt
QCOM_BOARD_PLATFORMS              := msm8960
TARGET_BOARD_PLATFORM             := msm8960
TARGET_BOOTLOADER_BOARD_NAME      := MSM8960

# Architecture
TARGET_ARCH_VARIANT_CPU    := cortex-a9
TARGET_CPU_ABI             := armeabi-v7a
TARGET_CPU_ABI2            := armeabi
TARGET_CPU_SMP             := true
TARGET_CPU_VARIANT         := krait
TARGET_ARCH                := arm
TARGET_ARCH_VARIANT        := armv7-a-neon
ARCH_ARM_HAVE_TLS_REGISTER := true
BOARD_USES_QCOM_HARDWARE   := true

# Krait optimizations
TARGET_USE_QCOM_BIONIC_OPTIMIZATION  := true
TARGET_USE_KRAIT_PLD_SET             := true
TARGET_KRAIT_BIONIC_PLDOFFS          := 10
TARGET_KRAIT_BIONIC_PLDTHRESH        := 10
TARGET_KRAIT_BIONIC_BBTHRESH         := 64
TARGET_KRAIT_BIONIC_PLDSIZE          := 64

# Audio
BOARD_USES_ALSA_AUDIO                   := true
TARGET_USES_QCOM_MM_AUDIO               := true
TARGET_USES_QCOM_COMPRESSED_AUDIO       := true
BOARD_USES_LEGACY_ALSA_AUDIO            := false
QCOM_ANC_HEADSET_ENABLED                := true
QCOM_PROXY_DEVICE_ENABLED               := true
AUDIO_FEATURE_ENABLED_PROXY_DEVICE      := true
QCOM_FLUENCE_ENABLED                    := true
QCOM_MULTI_VOICE_SESSION_ENABLED        := true
BOARD_HAVE_NEW_QCOM_CSDCLIENT           := true
BOARD_HAVE_CSD_FAST_CALL_SWITCH         := true
BOARD_HAVE_AUDIENCE_ES310               := true

# Blobs
TARGET_NEEDS_PLATFORM_TEXT_RELOCATIONS := true

# Bluetooth
BOARD_HAVE_BLUETOOTH                        := true
BOARD_HAVE_BLUETOOTH_QCOM                   := true
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(LOCAL_PATH)/bluetooth
BLUETOOTH_HCI_USE_MCT                       := true

# Display
BOARD_EGL_CFG                               := $(LOCAL_PATH)/rootdir/etc/egl.cfg
HAVE_ADRENO_SOURCE                          := false
NUM_FRAMEBUFFER_SURFACE_BUFFERS             := 3
OVERRIDE_RS_DRIVER                          := libRSDriver_adreno.so
TARGET_USES_ION                             := true
TARGET_USES_C2D_COMPOSITION                 := true
USE_OPENGL_RENDERER                         := true

# Flags
TARGET_GLOBAL_CFLAGS += -mfpu=neon -mfloat-abi=softfp -DQCOM_HARDWARE
TARGET_GLOBAL_CPPFLAGS += -mfpu=neon -mfloat-abi=softfp -DQCOM_HARDWARE
BOARD_GLOBAL_CFLAGS += -D__ARM_USE_PLD -D__ARM_CACHE_LINE_SIZE=64

# FM
QCOM_FM_ENABLED := true
TARGET_FM_LEGACY_PATCHLOADER := true
BOARD_HAVE_QCOM_FM := true

# Fonts
EXTENDED_FONT_FOOTPRINT := true

# Kernel
BOARD_KERNEL_BASE    := 0x80200000
BOARD_KERNEL_PAGESIZE := 2048
BOARD_RAMDISK_OFFSET := 0x02000000
BOARD_MKBOOTIMG_ARGS := --ramdisk_offset 0x02000000
BOARD_KERNEL_CMDLINE := console=ttyHSL0,115200,n8 androidboot.hardware=aries lpj=67677 user_debug=31 msm_rtb.filter=0x3F ehci-hcd.park=3 lge.kcal=0|0|0|x
BOARD_KERNEL_CMDLINE += androidboot.selinux=permissive
TARGET_KERNEL_ARCH := arm
TARGET_KERNEL_SOURCE := kernel/xiaomi/msm8960
TARGET_KERNEL_CONFIG := aries-perf-user_defconfig
TARGET_KERNEL_CROSS_COMPILE_PREFIX := arm-linux-androideabi-

BOARD_BOOTIMAGE_PARTITION_SIZE     := 0x01E00000 # 44M
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x00F00000 # 22M
BOARD_SYSTEMIMAGE_PARTITION_SIZE   := 1073741824
BOARD_USERDATAIMAGE_PARTITION_SIZE := 536870912
BOARD_PERSISTIMAGE_PARTITION_SIZE  := 8388608
BOARD_CACHEIMAGE_PARTITION_SIZE    := 402653184
BOARD_FLASH_BLOCK_SIZE             := 131072 # (BOARD_KERNEL_PAGESIZE * 64)

# Light
TARGET_PROVIDES_LIBLIGHT := true

# Malloc
MALLOC_SVELTE := true

# OTA assert
TARGET_OTA_ASSERT_DEVICE := aries
TARGET_RELEASETOOLS_EXTENSIONS := $(LOCAL_PATH)

# Qualcomm support
TARGET_USES_QCOM_BSP := true

# QCOM enhanced A/V
TARGET_ENABLE_QC_AV_ENHANCEMENTS := true

# Radio
TARGET_RIL_VARIANT := caf
BOARD_RIL_NO_CELLINFOLIST := true

# Wifi
BOARD_HAS_QCOM_WLAN := true
BOARD_WLAN_DEVICE := qcwcn
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
TARGET_USES_WCNSS_CTRL := true
TARGET_WCNSS_MAC_PREFIX := e8bba8
WIFI_DRIVER_FW_PATH_STA := "sta"
WIFI_DRIVER_FW_PATH_AP  := "ap"
WPA_SUPPLICANT_VERSION := VER_0_8_X

# Webkit
ENABLE_WEBGL            := true
TARGET_FORCE_CPU_UPLOAD := true

# Recovery
RECOVERY_FSTAB_VERSION           := 2
TARGET_RECOVERY_FSTAB            := $(LOCAL_PATH)/rootdir/ramdisk/fstab.qcom
TARGET_RECOVERY_PIXEL_FORMAT     := "RGBX_8888"
BOARD_HAS_NO_SELECT_BUTTON       := true
TARGET_USERIMAGES_USE_EXT4       := true

# Security
BOARD_USES_SECURE_SERVICES := true

# SELinux policies
# qcom sepolicy
include device/qcom/sepolicy/sepolicy.mk

# DEXPREOPT
DONT_DEXPREOPT_PREBUILTS := true

-include vendor/xiaomi/aries/BoardConfigVendor.mk

