SUMMARY = "A very basic X11 image with a terminal"

#IMAGE_FEATURES += "splash package-management x11-base"
IMAGE_FEATURES += "package-management x11-base"

LICENSE = "MIT"

image_strip() {
	#${BUILD_STRIP} ${IMAGE_ROOTFS}/usr/lib/lib*
        rm -rf ${IMAGE_ROOTFS}/usr/share/doc/*
        rm -r ${IMAGE_ROOTFS}/etc/init.d/mountnfs.sh
        rm -r ${IMAGE_ROOTFS}/etc/init.d/umountnfs.sh
        rm -r ${IMAGE_ROOTFS}/etc/init.d/avahi-daemon
        rm -r ${IMAGE_ROOTFS}/etc/init.d/hwclock.sh
        rm -r ${IMAGE_ROOTFS}/etc/init.d/banner.sh
}
ROOTFS_POSTPROCESS_COMMAND += "image_strip;"

inherit core-image distro_features_check

REQUIRED_DISTRO_FEATURES = "x11"

#EXTRA_USERS_PARAMS = "usermod -P '123' root;"

IMAGE_INSTALL += "packagegroup-qt5-core packagegroup-qt5-qtdeclarative packagegroup-qt5-qtdeclarative-qml qtbase qtquick1 qtx11extras qtxmlpatterns"
