TEMPLATE	= app
CONFIG		+= qt release
HEADERS		+= qfasttrackproctl.h
HEADERS		+= qfasttrackproctl_volume.h
SOURCES		+= qfasttrackproctl.cpp
SOURCES		+= qfasttrackproctl_volume.cpp

RESOURCES	+= qfasttrackproctl.qrc

TARGET		= qfasttrackproctl

target.path	= $${PREFIX}/bin
INSTALLS	+= target

icons.path	= $${PREFIX}/share/pixmaps
icons.files	= qfasttrackproctl.png
INSTALLS	+= icons

desktop.path	= $${PREFIX}/share/applications
desktop.files	= qfasttrackproctl.desktop
INSTALLS	+= desktop

