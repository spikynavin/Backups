#!/bin/bash
		if [ -e /dev/ubi1 ]
		then
			echo "ubi1 exists"
			df -h
		else
			echo "ubi1 doesnot exists"
			ubiformat /dev/mtd3
			ubiattach /dev/ubi_ctrl -m 3 -O 2048
			ubimkvol /dev/ubi1 -N user -m
			mount -t ubifs ubi1:user /media/nand

	fi

exit 0
