# Exract cpio initramfs file in linux

# below commands are used to extract the file
#gzip -d file.cpio.gz
#cpio -idm < file.cpio

find . | cpio --create --format='newc' | gzip > ../ramdisk.cpio.gz
mkimage -A arm -O linux -T ramdisk -C none -a 0x12C00000 -n ramdisk -d ../ramdisk.cpio.gz ../initramfs.cpio.gz
