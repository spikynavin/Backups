SUMMARY = "A very basic X11 image with a QT5 Packages"

#IMAGE_FEATURES += "package-management x11-base"
IMAGE_FEATURES += "x11-base"

LICENSE = "MIT"

inherit core-image distro_features_check

REQUIRED_DISTRO_FEATURES = "x11"

CONFLICT_DISTRO_FEATURES = "directfb"

X11_IMAGE_INSTALL = "${@base_contains('DISTRO_FEATURES', 'x11', \
    'libxkbcommon', '', d)}"

OPENCV_INSTALL = "opencv opencv-dev opencv-apps opencv-samples"

QT5_IMAGE_INSTALL = ""
QT5_IMAGE_INSTALL_common = " \
    packagegroup-qt5-core \
    packagegroup-qt5-qtdeclarative \
    packagegroup-qt5-qtdeclarative-qml \
    ${X11_IMAGE_INSTALL} \
    ${OPENCV_INSTALL} \
    "
QT5_IMAGE_INSTALL_mx6 = " \
    ${QT5_IMAGE_INSTALL_common} \
    "
inherit extrausers
EXTRA_USERS_PARAMS = "usermod -P 123 root;"

export IMAGE_BASENAME = "iris-image-qt5"

