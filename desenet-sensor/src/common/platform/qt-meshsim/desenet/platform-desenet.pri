PRI_PATH = $$PWD

PLATFORM_DESNET_PATH = "$${PRI_PATH}"

INCLUDEPATH += \
    "$${PLATFORM_DESNET_PATH}/.." \
    "$${PLATFORM_DESNET_PATH}"

HEADERS += \
    $${PLATFORM_DESNET_PATH}/timeslotmanager.h

SOURCES += \
    $${PLATFORM_DESNET_PATH}/nic-meshsim.cpp \
    $${PLATFORM_DESNET_PATH}/nic-meshsim-wrapper.cpp \
    $${PLATFORM_DESNET_PATH}/timeslotmanager.cpp
