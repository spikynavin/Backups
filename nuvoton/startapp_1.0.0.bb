DESCRIPTION = "Startapplication for 75xx"
SECTION = "Testtool"
#DEPENDS = "icu"
#LICENSE = "GPLv3"
LICENSE = "CLOSED"
#LIC_FILES_CHKSUM = "file://${S}/LICENSE;md5=01d6f96ddf706e595e24681c212042cb"
#SRC_URI = "git://github.com/spikynavin/startapplication-75xx.git"
SRC_URI = "git://github.com/spikynavin/Yocto-test-startapplication.git"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/git"

inherit qt4x11
#RRECOMMENDS_${PN} += "qt4-plugin-imageformat-jpeg qt4-plugin-imageformat-tiff"
#FILES_${PN} = "${WORKDIR}/git/x-session-manager
do_install(){
	install -d ${D}/${bindir}
	#install -m 0755 ${S}/startApplication ${D}/${bindir}
	install -m 0755 ${S}/simple_dialog ${D}/${bindir}
	install -m 0755 ${S}/x-session-manager ${D}/${bindir}
} 
PARALLEL_MAKE = ""
