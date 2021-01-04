#
# This file was derived from the 'Hello World!' example recipe in the
# Yocto Project Development Manual.
#

DESCRIPTION = "Zint Barcode & QRCode Generator"
SECTION = "support"
#DEPENDS = ""
LICENSE = "GPLv3"
LIC_FILES_CHKSUM = "file://COPYING;md5=d32239bcb673463ab874e80d47fae504"

#FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

SRCREV = "dddf2934fc7b22a6ef64b43f97137a880581f51e"
SRC_URI = "git://github.com/woo-j/zint.git"

S = "${WORKDIR}/git"

inherit cmake pkgconfig

DEPENDS += "libpng"

do_install_append() {
	rm -rf ${D}/usr/share
}

#FILE_${PN} += ""
#INSANE_SKIP_${PN} += "installed-vs-shipped"
FILES_${PN} = "${bindir}/* ${libdir}/*"
FILES_${PN}-dev = "${libdir}/* ${includedir}/*"
INSANE_SKIP_${PN} += "build-deps"
EXTRA_OECMAKE += "-DCMAKE_SKIP_RPATH=TRUE"

# The autotools configuration I am basing this on seems to have a problem with a race condition when parallel make is enabled
PARALLEL_MAKE = ""

BCLASSEXTEND = "native nativesdk"
