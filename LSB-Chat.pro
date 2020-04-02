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
QT += core
QT += quick
QT += network
QT += quickcontrols2

QTPLUGIN += qsvg

#-------------------------------------------------------------------------------
# Translations
#-------------------------------------------------------------------------------

TRANSLATIONS += \
    program/assets/translations/en.ts \
    program/assets/translations/es.ts

#-------------------------------------------------------------------------------
# Deploy options
#-------------------------------------------------------------------------------

win32* {
    RC_FILE = program/deploy/windows/resources/info.rc
}

macx* {
    ICON = program/deploy/mac-osx/icon.icns
    RC_FILE = program/deploy/mac-osx/icon.icns
    QMAKE_INFO_PLIST = program/deploy/mac-osx/info.plist
    QMAKE_POST_LINK = macdeployqt lsb-chat.app -qmldir=$$PWD/program/assets/qml
}

#-------------------------------------------------------------------------------
# Import source code
#-------------------------------------------------------------------------------

HEADERS += \
    program/src/AppInfo.h \
    program/src/Comms/NetworkComms.h \
    program/src/Comms/P2P_Connection.h \
    program/src/Comms/P2P_Manager.h \
    program/src/Comms/TCP_Listener.h \
    program/src/LSB/Crypto.h \
    program/src/LSB/LSB.h \
    program/src/QmlBridge.h \
    program/src/Translator.h

SOURCES += \
    program/src/Comms/NetworkComms.cpp \
    program/src/Comms/P2P_Connection.cpp \
    program/src/Comms/P2P_Manager.cpp \
    program/src/Comms/TCP_Listener.cpp \
    program/src/LSB/Crypto.cpp \
    program/src/LSB/LSB.cpp \
    program/src/QmlBridge.cpp \
    program/src/Translator.cpp \
    program/src/main.cpp

RESOURCES += \
    program/assets/assets.qrc

DISTFILES += \
    program/assets/qml/MainWindow.qml \
    program/assets/qml/Widgets/ChatLog.qml \
    program/assets/qml/Widgets/ChatRoom.qml \
    program/assets/qml/Widgets/ChatTextBox.qml \
    program/assets/qml/Widgets/Controls.qml \
    program/assets/qml/Widgets/ImagePreview.qml \
    program/assets/qml/Widgets/PeerList.qml \
    program/assets/qml/Widgets/WindowToolbar.qml \
    program/assets/qml/main.qml
