QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

QT += multimedia multimediawidgets

QT += multimedia
QT += bluetooth

# Указание библиотек зависит от платформы
win32 {
    # MSVC использует .lib файлы напрямую
    LIBS += -lPowrProf
    LIBS += -lsetupapi
    LIBS += -lCfgmgr32

    # Обязательная библиотека для Windows API (User32.dll, Gdi32.dll, Kernel32.dll)
    LIBS += -luser32
    LIBS += -lgdi32
    LIBS += -lole32
    LIBS += -loleaut32
    LIBS += -luuid
}


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    animationplayer.cpp \
    batterymonitor.cpp \
    bluetoothapp.cpp \
    camerahandler.cpp \
    main.cpp \
    mainwindow.cpp \
    pcideviceswidget.cpp \
    usbwindow.cpp

HEADERS += \
    animationplayer.h \
    animations.h \
    batterymonitor.h \
    bluetoothapp.h \
    camerahandler.h \
    mainwindow.h \
    pcideviceswidget.h \
    usbwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc

DISTFILES += \
    resources/img/Background/Background_main4k.png
