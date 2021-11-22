PRI_PATH = $$PWD

PLATFORM_MCU_PATH = "$${PRI_PATH}"

INCLUDEPATH += \
    "$${PLATFORM_MCU_PATH}/.." \
    "$${PLATFORM_MCU_PATH}"

HEADERS += \
    $${PLATFORM_MCU_PATH}/critical/critical.h

SOURCES += \
    $${PLATFORM_MCU_PATH}/critical/critical.cpp
