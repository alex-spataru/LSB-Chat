QT += testlib xml core network widgets

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../program/src

SOURCES +=  \
    ../../program/src/Comms/NetworkComms.cpp \
    ../../program/src/Comms/P2P_Connection.cpp \
    ../../program/src/Comms/P2P_Manager.cpp \
    ../../program/src/Comms/TCP_Listener.cpp \
    ../../program/src/LSB/Crypto.cpp \
    ../../program/src/LSB/LSB.cpp \
    TestMain.cpp

HEADERS += \
    ../../program/src/Comms/NetworkComms.h \
    ../../program/src/Comms/P2P_Connection.h \
    ../../program/src/Comms/P2P_Manager.h \
    ../../program/src/Comms/TCP_Listener.h \
    ../../program/src/LSB/Crypto.h \
    ../../program/src/LSB/LSB.h
