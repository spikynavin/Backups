source /opt/poky/1.4.4/environment-setup-armv5te-poky-linux-gnueabi
cd $PWD/Gpio/
arm-poky-linux-gnueabi-gcc -o gpiod gpio_daemon.c
arm-poky-linux-gnueabi-strip gpiod
cp gpiod ../build/
cd ..
cd $PWD/Network/
arm-poky-linux-gnueabi-gcc -o netd network_daemon.c -lpthread
arm-poky-linux-gnueabi-strip netd
cp netd ../build/
cd ..
cd $PWD/Gps/
make
arm-poky-linux-gnueabi-strip gpsd
cp gpsd ../build/
cd ..
cd $PWD/Taskbar
arm-poky-linux-gnueabi-gcc -o taskd taskbar_daemon.c
arm-poky-linux-gnueabi-strip taskd
cp taskd ../build/
#cd ..
#cd $PWD/Toolbar
#arm-poky-linux-gnueabi-gcc -o toold toolbar.c
#cp toold ../build/
cd ..
cd $PWD/Keypad
arm-poky-linux-gnueabi-gcc -o keyd keypad_rotate.c -lpthread -lrt
arm-poky-linux-gnueabi-gcc -o standbyd standby_daemon.c -lpthread -lrt
arm-poky-linux-gnueabi-strip standbyd
arm-poky-linux-gnueabi-strip keyd
cp keyd ../build/
cp standbyd ../build/
cd ..
cd $PWD/rmgmtd
qmake
make
arm-poky-linux-gnueabi-strip rmgmtd
cp rmgmtd ../build
#cd ..
#cd $PWD/shutdown
#make
#cp shutdown ../build/shutd
cd ..
cd $PWD/Timezone
qmake
make
arm-poky-linux-gnueabi-strip timezone
cp timezone ../../Filesystem/MATCHBOX_14112016/usr/bin/
cd ..
cd $PWD/usb_autorun
qmake
make
cp usb_autorun ../../Filesystem/MATCHBOX_14112016/usr/share/scripts/
cd ..
cd $PWD/usb_autoremove
qmake
make
arm-poky-linux-gnueabi-strip usb_autoremove
cp usb_autoremove ../../Filesystem/MATCHBOX_14112016/usr/share/scripts/
cd ..
cd $PWD/mmc_autorun
qmake
make
arm-poky-linux-gnueabi-strip mmc_autorun
cp mmc_autorun ../../Filesystem/MATCHBOX_14112016/usr/share/scripts/
cd ..
cd $PWD/mmc_autoremove
qmake
make
arm-poky-linux-gnueabi-strip mmc_autoremove
cp mmc_autoremove ../../Filesystem/MATCHBOX_14112016/usr/share/scripts/
cd ..

cd $PWD/eepromtool
qmake
make
arm-poky-linux-gnueabi-strip eepromtool
cp eepromtool ../../Filesystem/MATCHBOX_14112016/usr/bin/
cd ..

cd $PWD/machid
qmake
make
arm-poky-linux-gnueabi-strip machid
cp machid ../../Filesystem/MATCHBOX_14112016/opt/daemon_files/
cd ..

cd $PWD/master_download
arm-poky-linux-gnueabi-gcc -o master_download master_download.c
arm-poky-linux-gnueabi-strip master_download
cp master_download ../../Filesystem/MATCHBOX_14112016/opt/daemon_files/
cd ..


chmod 777 build/*
cp build/* ../Filesystem/MATCHBOX_14112016/opt/daemon_files/
echo "Copied to 7510 Filesystem"
