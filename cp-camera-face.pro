QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += multimedia multimediawidgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    licensedialog.cpp \
    main.cpp \
    mainwindow.cpp \
    ArcFaceEngine.cpp \
    facecameraviewfinder.cpp \
    clickablelabel.cpp

HEADERS += \
    licensedialog.h \
    mainwindow.h \
    cp-multi-camera/include/cpcamera.h \
    cp-multi-camera/include/cpcamera-interface.h \
    cp-multi-camera/include/cpcameraviewfinder.h \
    cp-multi-camera/include/cp-multi-camera_global.h \
    ArcFaceEngine.h \
    facecameraviewfinder.h \
    clickablelabel.h


FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    cp-camera-face_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations


INCLUDEPATH += $$PWD/cp-multi-camera/include

INCLUDEPATH += $$PWD/arcsoft/inc
INCLUDEPATH += $$PWD/arcsoft/opencv/include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/cp-multi-camera/lib/release -lcp-multi-camera
    DEPENDPATH += $$PWD/arcsoft/libs/x64/release/
    LIBS += -L$$PWD/arcsoft/libs/x64/release/ \
            -llibarcsoft_face_engine \
            -lopencv_core249 \
            -lopencv_highgui249 \
            -lopencv_imgproc249

    DYLD_LIBRARY_PATH += -L$$PWD/arcsoft/libs/x64/release/
}

win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/cp-multi-camera/lib/debug -lcp-multi-camera
    DEPENDPATH += $$PWD/arcsoft/libs/x64/debug/
    LIBS += -L$$PWD/arcsoft/libs/x64/debug/ \
        -llibarcsoft_face_engine \
        -lopencv_core249d \
        -lopencv_highgui249d \
        -lopencv_imgproc249d
    DYLD_LIBRARY_PATH += -L$$PWD/arcsoft/libs/x64/debug/
}

RESOURCES += \
    cp-camera-face.qrc

copydata.commands = $(COPY_DIR) $$shell_path($$PWD/setting.ini) $$shell_path($$OUT_PWD/release) #$$PWD/setting.int $$OUT_PWD/release
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)

QMAKE_EXTRA_TARGETS += first copydata

copyimages.commands = $(COPY_DIR) $$shell_path($$PWD/images) $$shell_path($$OUT_PWD/images)
last.depends = $(last) copyimages
export(last.depends)
export(copyimages.commands)

QMAKE_EXTRA_TARGETS += last copyimages


RC_ICONS = app.ico

VERSION = 1.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
