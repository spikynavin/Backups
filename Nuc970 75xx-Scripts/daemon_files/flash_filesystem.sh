mount /dev/sda1 /media/disk
flash_eraseall -j /dev/mtd2
nandwrite -p /dev/mtd2 /media/disk/ubi.img
ubiattach /dev/ubi_ctrl -m 2 -O 2048
