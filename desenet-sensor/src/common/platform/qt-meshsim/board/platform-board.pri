PRI_PATH = $$PWD

BOARD_PATH = "$${PRI_PATH}"

INCLUDEPATH += \
    "$${BOARD_PATH}/.." \
    "$${BOARD_PATH}"

HEADERS += \
    $$PWD/accel/accelerometer.h \
    $$PWD/board.h \
    $$PWD/display/simpledisplay.h \
    $$PWD/joystick.h \
    $$PWD/ledcontroller.h \
    $$PWD/meshsimboard.h

SOURCES += \
    $$PWD/accel/accelerometer.cpp \
    $$PWD/board.cpp \
    $$PWD/display/simpledisplay.cpp \
    $$PWD/joystick.cpp \
    $$PWD/ledcontroller.cpp \
    $$PWD/meshsimboard.cpp \
    $$PWD/trace.cpp
