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
# Deploy options
#-------------------------------------------------------------------------------

win32* {
    RC_FILE = deploy/windows/resources/info.rc
}

macx* {
    ICON = deploy/mac-osx/icon.icns
    RC_FILE = deploy/mac-osx/icon.icns
    QMAKE_INFO_PLIST = deploy/mac-osx/info.plist
    #QMAKE_POST_LINK = macdeployqt cansat-gss.app -qmldir=$$PWD/assets/qml
}

#-------------------------------------------------------------------------------
# Import source code
#-------------------------------------------------------------------------------

HEADERS += \
    src/AppInfo.h \
    src/Comms/NetworkComms.h \
    src/Comms/P2P_Connection.h \
    src/Comms/P2P_Manager.h \
    src/Comms/TCP_Listener.h \
    src/LSB/Crypto.h \
    src/LSB/LSB.h \
    src/QmlBridge.h

SOURCES += \
    src/Comms/NetworkComms.cpp \
    src/Comms/P2P_Connection.cpp \
    src/Comms/P2P_Manager.cpp \
    src/Comms/TCP_Listener.cpp \
    src/LSB/Crypto.cpp \
    src/LSB/LSB.cpp \
    src/QmlBridge.cpp \
    src/main.cpp

RESOURCES += \
    assets/assets.qrc

DISTFILES += \
    assets/qml/MainWindow.qml \
    assets/qml/Widgets/ChatLog.qml \
    assets/qml/Widgets/ChatRoom.qml \
    assets/qml/Widgets/ChatTextBox.qml \
    assets/qml/Widgets/Controls.qml \
    assets/qml/Widgets/ImagePreview.qml \
    assets/qml/Widgets/PeerList.qml \
    assets/qml/Widgets/WindowToolbar.qml \
    assets/qml/main.qml
