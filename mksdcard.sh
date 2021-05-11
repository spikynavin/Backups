#!/bin/sh

# -uS or -uB or -uC or -uM
# Accept or report in units of sectors (blocks, cylinders, megabytes, respectively). The default is cylinders, at least when the geometry is known.
# force = do what i say even it is stupid
# node = input device /dev/mmcblk0
# 0c = means windows fat partition
# 83 = means linux partition
# ${BOOT_ROM_SIZE}M,500M,0c = This means that the EMMC's 10M position to the 500M position is the first partition. As for the previous 10M function, it is not clear, it is estimated that the UBOOT part is stored, and the first partition is on my development board. Store the kernel and dtb. 600M,, 83 #This means that all memory from 600M is the second partition.
# partition size in MB

BOOT_ROM_SIZE=10


# call sfdisk to create partition table
# destroy the partition table
node=$1
dd if=/dev/zero of=${node} bs=1024 count=1

sfdisk --force -uM ${node} << EOF
${BOOT_ROM_SIZE},10,0c
20,500,83
600,,83
EOF
