#!/bin/bash
cp ../BB-PRUPID-MOTOR-00A0.dtbo /lib/firmware/
cat ./PRUMOTOR.data > /sys/bus/i2c/devices/1-0054/eeprom 
chmod +x ./dtbo
cp ./dtbo /etc/initramfs-tools/hooks/
cp /boot/uboot/initrd.img-??????-bone?? /boot/uboot/initrd.img.bak
/opt/scripts/tools/update_initrd.sh
