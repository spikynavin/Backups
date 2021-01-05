DESCRIPTION = "Pre compiled ko for hhc75xx printer,nfc,backlight,wifi"
SECTION = "Daemons"
LICENSE = "CLOSED"

SRC_URI = "git://github.com/spikynavin/Daemons_75xx.git;branch=Ko-files"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/git/ko-files"

FILES_${PN} += "/opt/daemon_files/*"


do_install(){
        install -d ${D}/${base_prefix}/opt/daemon_files
        install -m 0755 ${S}/*.ko ${D}${base_prefix}/opt/daemon_files
}

