#
# This file was derived from the Morpho-sdk example recipe in the
# Yocto Project Development Manual.
#
DESCRIPTION = "morpho precompiled libary for linux"
SECTION = "libs"
LICENSE = "CLOSED"
DEPENDS ="libusb1"

SRCREV = "${AUTOREV}"
SRC_URI = "git://github.com/spikynavin/morpho-libary.git"

S = "${WORKDIR}/git"

do_install () {
	install -d ${D}${libdir}
	install -m 0755 ${WORKDIR}/git/* ${D}${libdir} 
}
#INSANE_SKIP_${PN} += "build-deps"
#FILES_${PN} = "/usr/lib/* "	
FILES_${PN} += "${libdir}/*.so"
FILES_SOLIBSDEV = ""
INSANE_SKIP_${PN} += "dev-so"
