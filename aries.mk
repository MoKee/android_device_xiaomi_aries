#
# Copyright 2012 The Android Open Source Project
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

# Sample: This is where we'd set a backup provider if we had one
# $(call inherit-product, device/sample/products/backup_overlay.mk)

PRODUCT_RESTRICT_VENDOR_FILES := false

LOCAL_PATH := device/xiaomi/aries

# Camera
PRODUCT_PACKAGES += \
    camera.msm8960

# Light
PRODUCT_PACKAGES += \
    lights.msm8960

# Overlay
DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay

# Ramdisk
PRODUCT_PACKAGES += \
    fstab.aries \
    init.aries.rc  \
    init.class_main.sh \
    init.qcom.usb.rc \
    init.qcom.usb.sh \
    init.qcom.class_core.sh \
    init.qcom.early_boot.sh \
    init.qcom.sh \
    init.qcom.syspart_fixup.sh \
    init.target.rc \
    ueventd.aries.rc

# Wi-Fi
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/wifi/WCNSS_qcom_cfg.ini:system/etc/firmware/wlan/prima/WCNSS_qcom_cfg.ini \

# Audio
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/audio/snd_soc_msm_2x_Fusion3:system/etc/snd_soc_msm/snd_soc_msm_2x_Fusion3 \
    $(LOCAL_PATH)/voiceproc_init.img:system/etc/firmware/voiceproc_init.img \
    $(LOCAL_PATH)/voiceproc.img:system/etc/firmware/voiceproc.img

# Prebuilt kl and kcm keymaps
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/keylayout/apq8064-tabla-snd-card_Button_Jack.kl:system/usr/keylayout/apq8064-tabla-snd-card_Button_Jack.kl \
    $(LOCAL_PATH)/rootdir/keylayout/atmel_mxt_ts.kl:system/usr/keylayout/atmel_mxt_ts.kl \
    $(LOCAL_PATH)/rootdir/keylayout/cyttsp-i2c.kl:system/usr/keylayout/cyttsp-i2c.kl \
    $(LOCAL_PATH)/rootdir/keylayout/gpio-keys.kl:system/usr/keylayout/gpio-keys.kl \
    $(LOCAL_PATH)/rootdir/keylayout/keypad_8960.kl:system/usr/keylayout/keypad_8960.kl \
    $(LOCAL_PATH)/rootdir/keylayout/keypad_8960_liquid.kl:system/usr/keylayout/keypad_8960_liquid.kl \
    $(LOCAL_PATH)/rootdir/keylayout/philips_remote_ir.kl:system/usr/keylayout/philips_remote_ir.kl \
    $(LOCAL_PATH)/rootdir/keylayout/samsung_remote_ir.kl:system/usr/keylayout/samsung_remote_ir.kl \
    $(LOCAL_PATH)/rootdir/keylayout/sensor00fn1a.kl:system/usr/keylayout/sensor00fn1a.kl \
    $(LOCAL_PATH)/rootdir/keylayout/ue_rf4ce_remote.kl:system/usr/keylayout/ue_rf4ce_remote.kl \
    $(LOCAL_PATH)/rootdir/keylayout/ft5x06.kl:system/usr/keylayout/ft5x06.kl

# These are the hardware-specific features
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.telephony.cdma.xml:system/etc/permissions/android.hardware.telephony.cdma.xml

# Audio Configuration
PRODUCT_PROPERTY_OVERRIDES += \
    persist.audio.vns.mode=2

PRODUCT_PROPERTY_OVERRIDES += \
    ro.cdma.home.operator.numeric=46003 \
    ro.telephony.default_cdma_sub=0 \
    persist.omh.enabled=true

$(call inherit-product, frameworks/native/build/phone-xhdpi-2048-dalvik-heap.mk)

# Inherit from hardware-specific part of the product configuration
$(call inherit-product-if-exists, vendor/xiaomi/aries/aries-vendor.mk)
