mkdir /media/disk -p
mount /dev/sda1 /media/disk
flash_eraseall -j /dev/mtd1
nandwrite -p /dev/mtd1 /media/disk/uImage
