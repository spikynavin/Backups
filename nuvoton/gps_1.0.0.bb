DESCRIPTION = "Network daemon for HHC75xx"
SECTION = "Daemons"
LICENSE = "CLOSED"
#LIC_FILES_CHKSUM = ""

SRC_URI = "git://github.com/spikynavin/Daemons_75xx.git;branch=Gps"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git/Gps"

#inherit autotools
do_compile(){
	oe_runmake
}

do_install(){
       install -d ${D}/${base_prefix}/usr/sbin
       install -m 0755 ${S}/gpsd ${D}${base_prefix}/usr/sbin
}
INSANE_SKIP_${PN} = "ldflags"
PARALLEL_MAKE = ""

