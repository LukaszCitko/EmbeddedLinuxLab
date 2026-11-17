SUMMARY = "System status agent for Raspberry Pi"
LICENSE = "CLOSED"

SRC_URI = "file://EdgeStatusAgent"
S = "${UNPACKDIR}/EdgeStatusAgent"

SRC_URI:append = " file://edge-status-agent.service"

inherit cmake
inherit systemd

SYSTEMD_SERVICE:${PN} = "edge-status-agent.service"

do_install:append() {
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${UNPACKDIR}/edge-status-agent.service \
        ${D}${systemd_system_unitdir}/edge-status-agent.service
}

