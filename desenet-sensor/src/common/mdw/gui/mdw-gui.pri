PRI_PATH = $$PWD

MDW_GUI_SRC_PATH = "$${PRI_PATH}"

INCLUDEPATH += \
    "$${MDW_GUI_SRC_PATH}/.." \
    "$${MDW_GUI_SRC_PATH}"

HEADERS += \
    $${MDW_GUI_SRC_PATH}/monochromedisplaypainter.h

SOURCES += \
    $${MDW_GUI_SRC_PATH}/monochromedisplaypainter.cpp \
    $${MDW_GUI_SRC_PATH}/fontlookup.cpp
