QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += multimedia multimediawidgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h \
    cp-multi-camera/include/cpcamera.h \
    cp-multi-camera/include/cpcamera-interface.h \
    cp-multi-camera/include/cpcameraviewfinder.h \
    cp-multi-camera/include/cp-multi-camera_global.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    cp-camera-face_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations


INCLUDEPATH += $$PWD/cp-multi-camera/include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/cp-multi-camera/lib -lcp-multi-camera
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/cp-multi-camera/lib -lcp-multi-camera
else:unix: LIBS += -L$$PWD/libs/x64/

RESOURCES += \
    cp-camera-face.qrc
