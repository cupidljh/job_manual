SUMMARY = "Poky-tiny init"
DESCRIPTION = "Basic init system for poky-tiny"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

PR = "r2"

RDEPENDS_${PN} = "busybox"

SRC_URI = "\
	   file://hdlc.h\
	   file://hdlc.c\
	   file://run.c\
	   file://crc_table.h\
	   file://Makefile\
	  "

S = "${WORKDIR}"

do_configure() {
	:
}

do_compile() {
	make
}

do_install() {
	install -d ${D}${bindir}/
	install -m 0755 ${S}/hdlc_run ${D}${bindir}/
}

do_package_qa() {
	:
}

