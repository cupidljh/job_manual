#
# This file was derived from the 'Hello World!' example recipe in the
# Yocto Project Development Manual.
#

SUMMARY = "Simple helloworld application"
SECTION = "examples"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://hdlc.c\
	   file://hdlc.h\
	   file://debug.h\
	   file://run.c\
	   file://crc_table.h\
	   file://text.txt"


S = "${WORKDIR}"

do_compile() {
	${CC} -o run hdlc.c run.c
}

do_install() {
	     install -d ${D}${bindir}/
	     install -m 0755 ${S}/run ${D}${bindir}/
}

