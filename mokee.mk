# Inherit device configuration
$(call inherit-product, device/xiaomi/aries/full_aries.mk)

# Boot animation
TARGET_SCREEN_HEIGHT := 1280
TARGET_SCREEN_WIDTH := 720

# Inherit some common MK stuff.
$(call inherit-product, vendor/mk/config/common_full_phone.mk)

## Device identifier. This must come after all inclusions
PRODUCT_DEVICE := aries
PRODUCT_NAME := mk_aries
PRODUCT_BRAND := Xiaomi
PRODUCT_MODEL := MI 2
PRODUCT_MANUFACTURER := Xiaomi

PRODUCT_BUILD_PROP_OVERRIDES += PRODUCT_NAME=aries BUILD_FINGERPRINT=Xiaomi/aries/aries:5.1.1/LMY48B/3a5187b7b6:userdebug/test-key PRIVATE_BUILD_DESC="aries-userdebug 5.1.1 LMY48B 3a5187b7b6 test-keys"

# Enable Torch
PRODUCT_PACKAGES += Torch
