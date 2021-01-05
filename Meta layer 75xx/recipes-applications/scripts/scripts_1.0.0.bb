DESCRIPTION = "Scripts for HHC75xx"
SECTION = "scripts"
LICENSE = "CLOSED"
#LIC_FILES_CHKSUM = ""

SRC_URI = "git://github.com/spikynavin/hhc75xx-scripts.git"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"

FILES_${PN} += "/usr/share/*"
FILES_${PN} += "/opt/*"
FILES_${PN} += "${sysconfdir}/*"

RDEPENDS_${PN} += "bash"
#inherit allarch

do_install(){
        install -d ${D}/${base_prefix}/usr/share/
	install -d ${D}/${base_prefix}/opt/daemon_files/
	install -d ${D}/${base_prefix}/usr/share/X11/xorg.conf.d/
	install -d ${D}${sysconfdir}/init.d
	install -d ${D}${sysconfdir}/rc0.d
	install -d ${D}${sysconfdir}/rc5.d
	install -d ${D}${sysconfdir}/rc6.d
        #install -m 0755 ${S}/* ${D}${base_prefix}/usr/share/scripts
	cp -r ${WORKDIR}/git/scripts ${D}${base_prefix}/usr/share/
	cp -r ${WORKDIR}/git/status ${D}${base_prefix}/usr/share/
	cp -r ${WORKDIR}/git/daemon_files ${D}${base_prefix}/opt/
	cp -r ${WORKDIR}/git/start-init.sh ${D}${sysconfdir}/init.d/
	cp -r ${WORKDIR}/git/startapp.sh ${D}${sysconfdir}/init.d/
	cp -r ${WORKDIR}/git/xorg.conf ${D}${base_prefix}/usr/share/X11/xorg.conf.d/
	chmod 0755 ${D}${sysconfdir}/init.d/start-init.sh
	chmod 0755 ${D}${sysconfdir}/init.d/startapp.sh
	ln -s -r ${D}${sysconfdir}/init.d/startup.sh ${D}${sysconfdir}/rc0.d/S50start-init
	ln -s -r ${D}${sysconfdir}/init.d/startup.sh ${D}${sysconfdir}/rc5.d/S50start-init
	ln -s -r ${D}${sysconfdir}/init.d/startup.sh ${D}${sysconfdir}/rc6.d/S50start-init
	ln -s -r ${D}${sysconfdir}/init.d/startapplication.sh ${D}${sysconfdir}/rc0.d/S99startapp
	ln -s -r ${D}${sysconfdir}/init.d/startapplication.sh ${D}${sysconfdir}/rc5.d/S99startapp
	ln -s -r ${D}${sysconfdir}/init.d/startapplication.sh ${D}${sysconfdir}/rc6.d/S99startapp
}	

