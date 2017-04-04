# Makefile

# List the C src code files for ARM

ARM := ./arm
C_FILES := $(ARM)/beaglebot.c 
C_FILES += $(ARM)/mio.c
C_FILES += $(ARM)/child.c
C_FILES += $(ARM)/PRUlib.c
C_FILES += $(ARM)/servo_driver.c
# C_FILES += $(ARM)/srf02.c
# C_FILES += $(ARM)/color_sensor.c
# C_FILES += $(ARM)/accel.c
# C_FILES += $(ARM)/rtc.c
C_FILES += $(ARM)/bbbLib.c
C_FILES += $(ARM)/robotLib.c
C_FILES += $(ARM)/libIO.c
C_FILES += $(ARM)/navigation.c
C_FILES += $(ARM)/die.c
C_FILES += $(ARM)/amg.c
C_FILES += $(ARM)/testGyro.c
C_FILES += $(ARM)/ir.c

# List the assembler files for the PRUs

PRU := ./pru
P_FILES := $(PRU)/pru1.p

# List the dts files that need tBBo be compiles

DTS_FILES := ./dts/BB-PRUPID-MOTOR-00A0.dts
DTS_FILES += ./dts/ROBOT-00A0.dts

# Start address for PRU C object

C_FLAGS := -DSTART_ADDR=$(START_ADDR) 

# PRU development directory and cross tool

#CROSSTOOL = ""

# Compiler

CC := $(CROSS_COMPILE)gcc

# Linker
LD := $(CROSS_COMPILE)gcc
STRIP := $(CROSS_COMPILE)strip

# Important to have hard option!

C_FLAGS += -Wall -Wno-char-subscripts -O2 -mtune=cortex-a8 -march=armv7-a  -mfloat-abi=hard
C_FLAGS += -I $(PRU_SDK_DIR)/include
C_FLAGS += -I ./include


#L_FLAGS += -L $(PRU_SDK_DIR)/lib

L_FLAGS += -L $(PRU_SDK_DIR)/lib 
L_FLAGS += -mfloat-abi=hard

# Libraries

L_LIBS += -lprussdrv 
L_LIBS += -lm
L_LIBS += -lopencv_core
L_LIBS += -lopencv_imgproc
L_LIBS += -lopencv_calib3d
L_LIBS += -lopencv_video
L_LIBS += -lopencv_features2d
L_LIBS += -lopencv_ml
L_LIBS += -lopencv_highgui
L_LIBS += -lopencv_objdetect
L_LIBS += -lopencv_contrib
L_LIBS += -lopencv_legacy

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
	g++ arm/dice/dice.cpp -o arm/dice/dice -lopencv_core -lopencv_imgproc -lopencv_calib3d -lopencv_video -lopencv_features2d -lopencv_ml -lopencv_highgui -lopencv_objdetect -lopencv_contrib -lopencv_legacy
	g++ arm/dice/cache.cpp -o arm/dice/cache -lopencv_core -lopencv_imgproc -lopencv_calib3d -lopencv_video -lopencv_features2d -lopencv_ml -lopencv_highgui -lopencv_objdetect -lopencv_contrib -lopencv_legacy

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
