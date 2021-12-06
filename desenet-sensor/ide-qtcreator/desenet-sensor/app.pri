PRI_PATH = $$PWD

SENSOR_SRC_PATH = "$${PRI_PATH}/../../src"
APP_SRC_PATH = "$${SENSOR_SRC_PATH}/app"

INCLUDEPATH += \
    "$${SENSOR_SRC_PATH}"

HEADERS += \
    $$PWD/../../src/app/joystickapplication.h \
    $${APP_SRC_PATH}/accelerometerapplication.h \
    $${APP_SRC_PATH}/factory.h

SOURCES += \
    $$PWD/../../src/app/joystickapplication.cpp \
    $${APP_SRC_PATH}/accelerometerapplication.cpp \
    $${APP_SRC_PATH}/factory.cpp
