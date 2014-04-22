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

# include additional build utilities
-include device/xiaomi/aries/utils.mk

# Inherit from the common Open Source product configuration
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Get the long list of APNs
PRODUCT_COPY_FILES := device/xiaomi/aries/configs/apns-conf.xml:system/etc/apns-conf.xml

PRODUCT_NAME := full_aries
PRODUCT_DEVICE := aries
PRODUCT_BRAND := Xiaomi
PRODUCT_MODEL := MI 2
PRODUCT_MANUFACTURER := Xiaomi
PRODUCT_RESTRICT_VENDOR_FILES := false

## VoiceProc modules                                                                                              
PRODUCT_PACKAGES += voiceproc_init.img
PRODUCT_PACKAGES += voiceproc.img

# Wifi
PRODUCT_PACKAGES += wpa_supplicant_overlay.conf
PRODUCT_PACKAGES += p2p_supplicant_overlay.conf

# Radio and Telephony
PRODUCT_PROPERTY_OVERRIDES += \
    ro.telephony.ril_class=XiaomiQualcommRIL

# GMS
PRODUCT_PROPERTY_OVERRIDES += \
    ro.com.google.clientidbase=android-xiaomi

# Inherit from hardware-specific part of the product configuration
$(call inherit-product, device/xiaomi/aries/device.mk)
$(call inherit-product-if-exists, vendor/xiaomi/aries/aries-vendor.mk)
$(call inherit-product-if-exists, vendor/xiaomi/qcom-common/qcom-common-vendor.mk)
