PRI_PATH = $$PWD

DESENET_SENSOR_SRC_PATH = "$${PRI_PATH}"

INCLUDEPATH += \
    "$${DESENET_SENSOR_SRC_PATH}"

HEADERS += \
    $${DESENET_SENSOR_SRC_PATH}/abstractapplication.h \
    $${DESENET_SENSOR_SRC_PATH}/net.h \
    $${DESENET_SENSOR_SRC_PATH}/networkentity.h

SOURCES += \
    $${DESENET_SENSOR_SRC_PATH}/abstractapplication.cpp \
    $${DESENET_SENSOR_SRC_PATH}/net.cpp \
    $${DESENET_SENSOR_SRC_PATH}/networkentity.cpp
