PRI_PATH = $$PWD

CLOCKWORK_SRC_PATH = "$${PRI_PATH}"

INCLUDEPATH += \
    "$${CLOCKWORK_SRC_PATH}/.."

HEADERS += \
    $${CLOCKWORK_SRC_PATH}/clockwork.h \
    $${CLOCKWORK_SRC_PATH}/ttime.h

SOURCES += \
    $${CLOCKWORK_SRC_PATH}/clockwork.cpp \
    $${CLOCKWORK_SRC_PATH}/ttime.cpp
