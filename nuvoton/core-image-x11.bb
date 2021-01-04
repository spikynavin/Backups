SUMMARY = "A very basic X11 image with a terminal"

IMAGE_FEATURES += "splash package-management x11-base"

LICENSE = "MIT"

image_strip() {
        #${BUILD_STRIP} ${IMAGE_ROOTFS}/usr/lib/lib*
        rm -rf ${IMAGE_ROOTFS}/usr/bin/qt4
        rm -rf ${IMAGE_ROOTFS}/usr/share/doc/qt4
	rm -rf ${IMAGE_ROOTFS}/usr/lib/python3.5
	rm -rf ${IMAGE_ROOTFS}/usr/lib/*.a
	rm -rf ${IMAGE_ROOTFS}/lib/*.a
	rm -r ${IMAGE_ROOTFS}/etc/init.d/bluetooth
        rm -r ${IMAGE_ROOTFS}/etc/init.d/mountnfs.sh
        rm -r ${IMAGE_ROOTFS}/etc/init.d/umountnfs.sh
        rm -r ${IMAGE_ROOTFS}/etc/init.d/avahi-daemon
        rm -r ${IMAGE_ROOTFS}/etc/init.d/alsa-state
        rm -r ${IMAGE_ROOTFS}/etc/init.d/nfscommon
        rm -r ${IMAGE_ROOTFS}/etc/init.d/nfsserver
        rm -r ${IMAGE_ROOTFS}/etc/init.d/mosquitto
}
ROOTFS_POSTPROCESS_COMMAND += "image_strip;"


inherit core-image distro_features_check

REQUIRED_DISTRO_FEATURES = "x11"
