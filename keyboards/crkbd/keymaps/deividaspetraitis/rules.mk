SPLIT_KEYBOARD = yes
RGBLIGHT_ENABLE = yes        # Enable WS2812 RGB underlight.
TAP_DANCE_ENABLE = yes

# Build Options for right half
#################################
# OLED_ENABLE = false # only for left side
# POINTING_DEVICE_ENABLE = yes # Generic Pointer, not as big as mouse keys hopefully.
# POINTING_DEVICE_DRIVER = custom
# MOUSEKEY_ENABLE = yes        # Mouse keys(+4700)
# EXTRAKEY_ENABLE = yes        # Audio control and System control(+450)

# BOOTLOADER = atmel-dfu

# # Add support for the BB 8520 trackpad
# SRC += trackpad.c

# Build options for left half
#################################
OLED_ENABLE     = yes
OLED_DRIVER     = SSD1306

# If you want to change the display of OLED, you need to change here
SRC +=  ./lib/rgb_state_reader.c \
        ./lib/layer_state_reader.c \
        ./lib/logo_reader.c \
        ./lib/keylogger.c \
