QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += TC_MESHSIM
DEFINES += USE_DISPLAY

INCLUDEPATH += \
    platform/qt-meshsim

include(app.pri)
include(../../src/common/mdw/desenet/mdw-desenet-common.pri)
include(../../src/common/mdw/desenet/sensor/mdw-desenet-sensor.pri)
include(../../src/common/mdw/trace/mdw-trace.pri)
include(../../src/common/mdw/clockwork/mdw-clockwork.pri)
include(../../src/common/mdw/gui/mdw-gui.pri)
include(../../src/common/platform/qt-meshsim/board/platform-board.pri)
include(../../src/common/platform/qt-meshsim/desenet/platform-desenet.pri)
include(../../src/common/platform/qt-meshsim/mcu/platform-mcu.pri)
include(../../src/common/platform/platform-common/board/platform-board-common.pri)
include(../../src/xf/xf.pri)

SOURCES += \
    main.cpp


HEADERS += \
    platform/qt-meshsim/platform-config.h


FORMS +=

linux {
    QMAKE_LFLAGS += -no-pie
}

win32 {
    LIBS += -lws2_32
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
