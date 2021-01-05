#
# This recipes for nfc library with binary for pn5xx"
#
DESCRIPTION = "Nfc library & binary for pn5xx"
SECTION = "Communication"
LICENSE = "CLOSED"

SRC_URI = "git://github.com/NXPNFCLinux/linux_libnfc-nci.git"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"

do_configure_prepend() {
	current_dir=`pwd`
	cd ${S}
	./bootstrap --force
	cd $current_dir
}

inherit autotools gettext




