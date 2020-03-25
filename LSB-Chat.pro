#-------------------------------------------------------------------------------
# Make options
#-------------------------------------------------------------------------------

UI_DIR = uic
MOC_DIR = moc
RCC_DIR = qrc
OBJECTS_DIR = obj

CONFIG += c++11

#-------------------------------------------------------------------------------
# Qt configuration
#-------------------------------------------------------------------------------

TEMPLATE = app
TARGET = lsb-chat

CONFIG += qtc_runnable
CONFIG += resources_big
CONFIG += qtquickcompiler

QT += xml
QT += svg
QT += sql
QT += core
QT += quick
QT += network
QT += quickcontrols2

QTPLUGIN += qsvg

#-------------------------------------------------------------------------------
# Import source code
#-------------------------------------------------------------------------------

HEADERS += \
    src/AppInfo.h \
    src/Comms/Discovery.h \
    src/Comms/NetworkComms.h \
    src/Comms/P2P_Client.h \
    src/LSB/Crypto.h \
    src/LSB/LSB-Reader.h \
    src/LSB/LSB-Writer.h \
    src/LSB/LSB.h \
    src/QmlBridge.h

SOURCES += \
    src/Comms/Discovery.cpp \
    src/Comms/NetworkComms.cpp \
    src/Comms/P2P_Client.cpp \
    src/LSB/Crypto.cpp \
    src/LSB/LSB-Reader.cpp \
    src/LSB/LSB-Writer.cpp \
    src/LSB/LSB.cpp \
    src/QmlBridge.cpp \
    src/main.cpp

RESOURCES += \
    assets/assets.qrc

DISTFILES += \
    assets/qml/AboutWindow.qml \
    assets/qml/MainWindow.qml \
    assets/qml/PreferencesWindow.qml \
    assets/qml/Widgets/ChatRoom.qml \
    assets/qml/Widgets/ChatTextBox.qml \
    assets/qml/Widgets/WindowToolbar.qml \
    assets/qml/main.qml
