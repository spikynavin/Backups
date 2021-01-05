DESCRIPTION = "HHC75xx Libary with toolchain"
SECTION = "libs"
DEPENDS = "lcms tslib"
LICENSE = "GPLv3"
LIC_FILES_CHKSUM = "file://LICENSE;md5=15204c2230b36be384db021378b490ae"
SRCREV ="${AUTOREV}"
SRC_URI = "git://github.com/spikynavin/hhc75xx-sdklib.git"

S = "${WORKDIR}/git"

#inherit autotools gettext
inherit qt4x11
PARALLEL_MAKE = ""

