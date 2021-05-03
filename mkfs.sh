#!/bin/sh

source toolchain_path

echo "Compiling Kernel"
cd linux-imx/
rm -rf imx6dl-sabreauto.dtb uImage bin_sd8801/ lib/modules/*
#make imx_v7_defconfig
#cp -rf arch/arm/configs/working_config_iris_3.14.28 .config
make menuconfig
make -j8
make INSTALL_MOD_PATH=$PWD/ modules_install
make uImage LOADADDR=0x10800000
make dtbs
cp arch/arm/boot/uImage $PWD
cp arch/arm/boot/dts/imx6dl-sabreauto.dtb $PWD
chmod 777 uImage
cd ../

echo "Compiling ublox wifi driver with linux 3.14.28"
cd ublox/
make -e MAKEFLAGS= KERNELDIR=../ CROSS_COMPILE=${CROSS_COMPILE} CROSS=${CROSS_COMPILE} build
sync
cd ../

echo "Creating ROOTFS"
cd rootfs/
sudo tar -cvjSf ../rootfs.tar.bz2 ./*
sync
cd ../
