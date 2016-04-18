#!/usr/bin/env sh
/sbin/modprobe uio_pruss
cp ./motor-revA-00A0.dtbo $FW
cp ./motor-revB-00A0.dtbo $FW
echo motor-revA > $SLOTS
./main
