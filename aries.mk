DEVICE_PATH := device/xiaomi/aries

$(call inherit-product, device/xiaomi/msm8960-common/msm8960-common.mk)
$(call inherit-product, vendor/xiaomi/aries/aries-vendor.mk)

# System properties
-include $(DEVICE_PATH)/system_prop.mk

# Audio
PRODUCT_PACKAGES += \
    audio_amplifier.msm8960
