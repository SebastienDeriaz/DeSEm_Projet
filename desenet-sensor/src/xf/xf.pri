PRI_PATH = $$PWD

XF_PATH = "$${PRI_PATH}"

INCLUDEPATH += \
    "$${XF_PATH}/.."

HEADERS += \
    $${PRI_PATH}/initialevent.h \
    $${PRI_PATH}/ixfevent.h \
    $${PRI_PATH}/ixfreactive.h \
    $${PRI_PATH}/xf.h \
    $${PRI_PATH}/xfevent.h \
    $${PRI_PATH}/xfeventqueue.hpp \
    $${PRI_PATH}/xfeventstatus.h \
    $${PRI_PATH}/xfnulltransition.h \
    $${PRI_PATH}/xfreactive.h \
    $${PRI_PATH}/xfstaticevent.h \
    $${PRI_PATH}/xfthread.h \
    $${PRI_PATH}/xftimeout.h \
    $${PRI_PATH}/xftimeoutmanager.h

SOURCES += \
    $${PRI_PATH}/initialevent.cpp \
    $${PRI_PATH}/xf.cpp \
    $${PRI_PATH}/xfevent.cpp \
    $${PRI_PATH}/xfnulltransition.cpp \
    $${PRI_PATH}/xfreactive.cpp \
    $${PRI_PATH}/xfstaticevent.cpp \
    $${PRI_PATH}/xfthread.cpp \
    $${PRI_PATH}/xftimeout.cpp \
    $${PRI_PATH}/xftimeoutmanager.cpp
