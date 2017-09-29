isEmpty(PREFIX) {
    PREFIX=/usr/local
}
greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets
}
TEMPLATE	= app
CONFIG		+= qt release
HEADERS		+= qfasttrackultractl.h
HEADERS		+= qfasttrackultractl_volume.h
SOURCES		+= qfasttrackultractl.cpp
SOURCES		+= qfasttrackultractl_volume.cpp

RESOURCES	+= qfasttrackultractl.qrc

TARGET		= qfasttrackultractl

target.path	= $${PREFIX}/bin
INSTALLS	+= target

icons.path	= $${PREFIX}/share/pixmaps
icons.files	= qfasttrackultractl.png
INSTALLS	+= icons

desktop.path	= $${PREFIX}/share/applications
desktop.files	= qfasttrackultractl.desktop
INSTALLS	+= desktop

