DESCRIPTION = "Network daemon for HHC75xx"
SECTION = "Daemons"
LICENSE = "CLOSED"
#LIC_FILES_CHKSUM = ""

SRC_URI = "git://github.com/spikynavin/Daemons_75xx.git;branch=eepromtool"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git/eepromtool"

inherit qt4x11

do_install(){
       install -d ${D}/${base_prefix}/usr/sbin
       install -m 0755 ${S}/eepromtool ${D}${base_prefix}/usr/sbin
}
PARALLEL_MAKE = ""

