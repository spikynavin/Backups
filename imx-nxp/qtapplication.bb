SUMMARY = "Qt Simple Automotive Cluster Application Recipe"
DESCRIPTION = "This recipe builds a Qt project for a simple automotive cluster application."
#LICENSE = "MIT"
LICENSE = "CLOSED"
#LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS += "qtbase"

SRC_URI = "git://github.com/spikynavin/qtcluster.git;branch=7inch"
SRCREV = "c39cce6e24515daeba6361f0d3e101cfb8cd7e6a"

S = "${WORKDIR}/git"

do_install_append() {
    install -d ${D}/usr/bin/
    install -m 0755 QT_demo_App ${D}/usr/bin/demo-cluster
}

FILES_${PN} += "/usr/bin/QT_demo_App"

inherit qmake5
