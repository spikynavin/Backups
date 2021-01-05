#!/bin/sh
mkimage -A arm -O linux -T kernel -C none -a 10800000 -e 10800000 -n "linux kernel" -d zImage uImage
chmod 777 uImage

