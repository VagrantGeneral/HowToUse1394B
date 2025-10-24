QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ccnodework.cpp \
    main.cpp \
    mainwidget.cpp \
    rnnodework.cpp

HEADERS += \
    XT1394B/Mil1394_Emu_Addr.h \
    XT1394B/Mil1394_Emu_XT.h \
    ccnodework.h \
    mainwidget.h \
    messagestats.h \
    rnnodework.h

FORMS += \
    mainwidget.ui

# 1394B头文件及库
INCLUDEPATH += $$PWD/XT1394B/
LIBS += -L$$PWD/XT1394B/ -lMil1394_Emu_XT

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
