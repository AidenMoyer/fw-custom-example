include $(BOARD_DIR)/firmware/firmware.mk

BOARDINC += $(BOARD_DIR)/generated/controllers/generated

# defines SHORT_BOARD_NAME
include $(BOARD_DIR)/meta-info.env

# this would save some flash while being unable to update WBO controller firmware
DDEFS += -DEFI_WIDEBAND_FIRMWARE_UPDATE=FALSE

# red LED on PD15 is the critical-error LED (PD14 is the Lua status LED on this board)
DDEFS += -DLED_CRITICAL_ERROR_BRAIN_PIN=Gpio::D15

# 2 x MAX31855K thermocouple chips on SPI2
DDEFS += -DEFI_MAX_31855=TRUE
