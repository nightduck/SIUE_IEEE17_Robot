# Makefile


# List the C src code files for ARM

ARM := ./arm
C_FILES := $(ARM)/testMotor.c 
C_FILES += $(ARM)/mio.c
C_FILES += $(ARM)/servo_driver.c
C_FILES += $(ARM)/srf02.c
C_FILES += $(ARM)/BBBlib.c
C_FILES += $(ARM)/ROBOTlib.c
C_FILES += $(ARM)/child.c

# List the assembler files for the PRUs

PRU := ./pru
P_FILES := $(PRU)/pru1.p

# List the dts files that need tBBo be compiles

DTS_FILES := ./motor-00A0.dts

# Start address for PRU C object

C_FLAGS := -DSTART_ADDR=$(START_ADDR) 

# PRU development directory and cross tool

#CROSSTOOL = ""

# Compiler

CC := $(CROSS_COMPILE)gcc

# Linker
LD := $(CROSS_COMPILE)gcc
STRIP := $(CROSS_COMPILE)strip

# Assembler

#PASM := /usr/bin/pasm

# Device tree overlay compiler

#DTC := /usr/bin/dtc

# Important to have hard option!

C_FLAGS += -Wall -O2 -mtune=cortex-a8 -march=armv7-a  -mfloat-abi=hard
C_FLAGS += -I $(PRU_SDK_DIR)/include
C_FLAGS += -I ./include


#L_FLAGS += -L $(PRU_SDK_DIR)/lib

L_FLAGS += -L $(PRU_SDK_DIR)/lib 
L_FLAGS += -mfloat-abi=hard

# Libraries

L_LIBS += -lprussdrv 
L_LIBS += -lm

# Recipe for assembler files

BIN_FILES := $(P_FILES:.p=.bin)

# Recipe for C code

O_FILES := $(C_FILES:.c=.o)

# Recipe for device tree overlay files

DTBO_FILES := $(DTS_FILES:.dts=.dtbo)

# ----------------------------------------------

all:	main $(BIN_FILES) $(DTBO_FILES)

main:	$(O_FILES)
	$(LD) -static -o $@ $(O_FILES) $(L_FLAGS) $(L_LIBS)
	$(STRIP) $@

%.bin : %.p
	$(PASM) -V2 -I$(PRU_SDK_DIR)/include -b $<

%.o : %.c
	$(CC) $(C_FLAGS) -c -o $@ $<

%.dtbo : %.dts
	$(DTC) -@ -O dtb -o $@ $<

.PHONY	: clean all
clean	:
	-rm -f $(O_FILES)
	-rm -f $(BIN_FILES)
	-rm -f $(DTBO_FILES)
	-rm -f main
