PRI_PATH = $$PWD

BOARD_COMMON_SRC_PATH = "$${PRI_PATH}"

INCLUDEPATH += \
    "$${BOARD_COMMON_SRC_PATH}/.." \
    "$${BOARD_COMMON_SRC_PATH}"

HEADERS += \
    $${BOARD_COMMON_SRC_PATH}/interfaces/ijoystick.h \
    $${BOARD_COMMON_SRC_PATH}/interfaces/ijoystickobserver.h \
    $${BOARD_COMMON_SRC_PATH}/interfaces/imonochromedisplay.h \
    $${BOARD_COMMON_SRC_PATH}/ledflasher.h


SOURCES += \
    $${BOARD_COMMON_SRC_PATH}/ledflasher.cpp
