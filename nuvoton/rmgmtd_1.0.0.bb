DESCRIPTION = "Network daemon for HHC75xx"
SECTION = "Daemons"
DEPENDS = "mosquitto"
LICENSE = "CLOSED"
#LIC_FILES_CHKSUM = ""

SRC_URI = "git://github.com/spikynavin/Daemons_75xx.git;branch=rmgmtd"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git/rmgmtd"

inherit qt4x11

do_install(){
       install -d ${D}/${base_prefix}/usr/sbin
       install -m 0755 ${S}/rmgmtd ${D}${base_prefix}/usr/sbin
}
PARALLEL_MAKE = ""

