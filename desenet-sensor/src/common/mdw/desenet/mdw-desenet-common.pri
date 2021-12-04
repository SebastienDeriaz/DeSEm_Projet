PRI_PATH = $$PWD

DESENET_COMMON_SRC_PATH = "$${PRI_PATH}"

INCLUDEPATH += \
    "$${DESENET_COMMON_SRC_PATH}/.."

HEADERS += \
    $$PWD/mpdu.h \
    $${DESENET_COMMON_SRC_PATH}/address.hpp \
    $${DESENET_COMMON_SRC_PATH}/beacon.h \
    $${DESENET_COMMON_SRC_PATH}/desenet.h \
    $${DESENET_COMMON_SRC_PATH}/frame.h \
    $${DESENET_COMMON_SRC_PATH}/itimeslotmanager.h \
    $${DESENET_COMMON_SRC_PATH}/networkinterfacedriver.h \
    $${DESENET_COMMON_SRC_PATH}/networktimeprovider.h \
    $${DESENET_COMMON_SRC_PATH}/types.h


SOURCES += \
    $$PWD/mpdu.cpp \
    $${DESENET_COMMON_SRC_PATH}/beacon.cpp \
    $${DESENET_COMMON_SRC_PATH}/frame.cpp \
    $${DESENET_COMMON_SRC_PATH}/networkinterfacedriver.cpp \
    $${DESENET_COMMON_SRC_PATH}/networktimeprovider.cpp
