#
# espi-mbus
#
# Copyright (C) 2019 packom.net Limited
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
#

# SDK versions, etc
SDK_BASE ?= /opt/esp-open-sdk
ESP_SDK = sdk

# Build tools
XTENSA_DIR = $(SDK_BASE)/xtensa-lx106-elf/bin
OBJDUMP = $(XTENSA_DIR)/xtensa-lx106-elf-objdump
OBJCOPY = $(XTENSA_DIR)/xtensa-lx106-elf-objcopy
NM = $(XTENSA_DIR)/xtensa-lx106-elf-nm
CC = $(XTENSA_DIR)/xtensa-lx106-elf-gcc
LD = $(XTENSA_DIR)/xtensa-lx106-elf-gcc
AR = $(XTENSA_DIR)/xtensa-lx106-elf-ar
ESPTOOL_PY = python2 $(XTENSA_DIR)/esptool.py
MAKE = make

# Serial connection information
SERIAL_PORT ?= /dev/ttyUSB0
SERIAL_BAUD ?= 115200
SERIAL_BAUD_230 = 230400
SERIAL_BAUD_74 = 74880
SERIAL_CMD = python2.7 /usr/lib/python2.7/dist-packages/serial/tools/miniterm.py
SERIAL = $(SERIAL_CMD) $(SERIAL_PORT) $(SERIAL_BAUD) --raw

# esptool.py options
ESPBAUD = 115200
ESPPORT = $(SERIAL_PORT)
ESPTOOL_PY_OPTS=--port $(ESPPORT) --baud $(ESPBAUD)

# esptool2 options
E2_OPTS = -quiet -bin -boot0
SPI_SIZE = 4M
SPI_SPEED = 40

ifeq ($(SPI_SIZE), 256K)
        E2_OPTS += -256
else ifeq ($(SPI_SIZE), 512K)
        E2_OPTS += -512
else ifeq ($(SPI_SIZE), 1M)
        E2_OPTS += -1024
else ifeq ($(SPI_SIZE), 2M)
        E2_OPTS += -2048
else ifeq ($(SPI_SIZE), 4M)
        E2_OPTS += -4096
else ifeq ($(SPI_SIZE), 8M)
        E2_OPTS += -8192
else ifeq ($(SPI_SIZE), 16M)
        E2_OPTS += -16384
endif
ifeq ($(SPI_MODE), qio)
        E2_OPTS += -qio
else ifeq ($(SPI_MODE), dio)
        E2_OPTS += -dio
else ifeq ($(SPI_MODE), qout)
        E2_OPTS += -qout
else ifeq ($(SPI_MODE), dout)
        E2_OPTS += -dout
endif
ifeq ($(SPI_SPEED), 20)
        E2_OPTS += -20
else ifeq ($(SPI_SPEED), 26)
        E2_OPTS += -26.7
else ifeq ($(SPI_SPEED), 40)
        E2_OPTS += -40
else ifeq ($(SPI_SPEED), 80)
        E2_OPTS += -80
endif

# Compile options
CFLAGS = -Os -Iinclude -Iinclude/boards -I$(SDK_BASE)/sdk/include -mlongcalls -c -ggdb -Wpointer-arith -Wundef -Wno-address -Wl,-El -fno-inline-functions -nostdlib -mtext-section-literals -DICACHE_FLASH -Werror -D__ets__ -std=c99 -Ilib/esp8266-software-uart/softuart/include
ESPIMBUS_CFLAGS = $(CFLAGS)
SOFTUART_CFLAGS = $(CFLAGS)

# Link options
LD_DIR = ld
LDLIBS = -Wl,--start-group -lc -lcirom -lgcc -lhal -lphy -lpp -lnet80211 -lwpa -lmain2 -llwip -Wl,--end-group
BUILD_NUM_FILE = include/otb_build_num.txt
LDFLAGS = -T$(SDK_BASE)/$(ESP_SDK)/ld/eagle.app.v6.ld -nostdlib -Wl,--no-check-sections -Wl,-static -L$(SDK_BASE)/$(ESP_SDK)/lib -u call_user_start -u Cache_Read_Enable_New -Lbin
LD_SCRIPT = $(SDK_BASE)/$(ESP_SDK)/ld/eagle.app.v6.ld

# Source and object directories
ESPIMBUS_SRC_DIR = src
ESPIMBUS_OBJ_DIR = obj/espimbus
SOFTUART_SRC_DIR = lib/esp8266-software-uart/softuart
SOFTUART_OBJ_DIR = obj/softuart

# Object files
espimbusObjects = $(ESPIMBUS_OBJ_DIR)/main.o
espimbusDep = $(espimbusObjects:%.o=%.d)

softuartObjects = $(SOFTUART_OBJ_DIR)/softuart.o 
softuartDep = $(softuartObjects:%.o=%.d)

all: directories app_image

app_image: bin/app_image.elf
	$(NM) -n bin/app_image.elf > bin/app_image.sym
	$(OBJDUMP) -d bin/app_image.elf > bin/app_image.dis
	$(ESPTOOL_PY) elf2image $^

bin/app_image.elf: libmain2 espimbus_objects softuart_objects
	$(LD) $(LDFLAGS) -o bin/app_image.elf $(espimbusObjects) $(softuartObjects) $(LDLIBS)

-include $(espimbusDep) $(softuartDep)

sdk_init_data.bin: directories
	rm -f bin/sdk_init_data.bin
	if test -f $(SDK_BASE)/$(ESP_SDK)/bin/esp_init_data_default_v05.bin; then ln -s $(SDK_BASE)/$(ESP_SDK)/bin/esp_init_data_default_v05.bin bin/sdk_init_data.bin; else ln -s $(SDK_BASE)/$(ESP_SDK)/bin/esp_init_data_default.bin bin/sdk_init_data.bin; fi

# can replace with our own version (from rboot-bigflash.c)
libmain2: directories
	$(OBJCOPY) -W Cache_Read_Enable_New $(SDK_BASE)/$(ESP_SDK)/lib/libmain.a bin/libmain2.a

espimbus_objects: $(espimbusObjects)

softuart_objects: $(softuartObjects)

$(ESPIMBUS_OBJ_DIR)/%.o: $(ESPIMBUS_SRC_DIR)/%.c
	$(CC) $(ESPIMBUS_CFLAGS) -MMD -c $< -o $@ 

$(SOFTUART_OBJ_DIR)/%.o: $(SOFTUART_SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(SOFTUART_CFLAGS) -MMD -c $< -o $@ 

flash_app: app_image
	$(ESPTOOL_PY) $(ESPTOOL_PY_OPTS) write_flash 0x0 bin/app_image.elf-0x00000.bin 0x10000 bin/app_image.elf-0x10000.bin

flash: flash_app

flash_blank: 
	$(ESPTOOL_PY) $(ESPTOOL_PY_OPTS) write_flash 0x3fb000 $(SDK_BASE)/$(ESP_SDK)/bin/blank.bin
	$(ESPTOOL_PY) $(ESPTOOL_PY_OPTS) write_flash 0x3fe000 $(SDK_BASE)/$(ESP_SDK)/bin/blank.bin

flash_sdk: sdk_init_data.bin
	$(ESPTOOL_PY) $(ESPTOOL_PY_OPTS) write_flash 0x3fc000 bin/sdk_init_data.bin

flash_initial: flash_all

flash_all: directories erase_flash flash_blank flash_sdk flash_app

con: connect

connect:
	$(SERIAL_CMD) $(SERIAL_PORT) $(SERIAL_BAUD) --raw

con230:
	$(SERIAL_CMD) $(SERIAL_PORT) $(SERIAL_BAUD_230) --raw

con74:
	$(SERIAL_CMD) $(SERIAL_PORT) $(SERIAL_BAUD_74) --raw

clean: 
	@rm -fr bin obj 

erase_flash:
	$(ESPTOOL_PY) $(ESPTOOL_PY_OPTS) erase_flash

flash_40mhz:
	$(ESPTOOL_PY) $(ESPTOOL_PY_OPTS) write_flash 0x3fc000 flash/esp_init_data_40mhz_xtal.hex

directories:
	mkdir -p bin $(ESPIMBUS_OBJ_DIR) $(SOFTUART_OBJ_DIR)

docs: FORCE
	-$(MAKE) -C docs html

clean_docs: FORCE
	-$(MAKE) -C docs clean

FORCE:

