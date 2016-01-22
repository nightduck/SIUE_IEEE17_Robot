#!/usr/bin/env sh
/sbin/modprobe uio_pruss
cp ./motor-00A0.dtbo $FW
echo motor > $SLOTS
./main
