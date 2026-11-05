require recipes-core/images/core-image-minimal.bb

SUMMARY = "Minimal Raspberry Pi image with EdgeStatusAgent"
IMAGE_INSTALL:append = " edge-status-agent wpa-supplicant systemd-networkd edge-network edge-access linux-firmware-rpidistro-bcm43455 kernel-modules"
IMAGE_FEATURES:append = " ssh-server-dropbear allow-root-login"

inherit extrausers
EXTRA_USERS_PARAMS:append = " usermod -a -G systemd-journal lukasz;"

