QT       += core gui multimedia quick

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


DEFINES += _WEBSOCKETPP_CPP11_STL_
DEFINES += _WEBSOCKETPP_CPP11_FUNCTIONAL_
# DEFINES += SIO_TLS
DEFINES += ASIO_STANDALONE

SOURCES += \
    SocketIO/internal/sio_client_impl.cpp \
    SocketIO/internal/sio_packet.cpp \
    SocketIO/sio_client.cpp \
    SocketIO/sio_socket.cpp \
    audioinput.cpp \
    audiooutput.cpp \
    main.cpp \
    signalingserver.cpp \
    webrtc.cpp

HEADERS += \
    SocketIO/internal/sio_client_impl.h \
    SocketIO/internal/sio_packet.h \
    SocketIO/sio_client.h \
    SocketIO/sio_message.h \
    SocketIO/sio_socket.h \
    audioinput.h \
    audiooutput.h \
    signalingserver.h \
    webrtc.h

# Adding include paths for WebRTC and Opus
INCLUDEPATH += C:/Users/Amirali/Desktop/UT/Network/Projects/lib/libdatachannel
INCLUDEPATH += C:/Users/Amirali/Desktop/UT/Network/Projects/lib/opus/include
INCLUDEPATH += C:/Users/Amirali/Desktop/UT/Network/Projects/lib/libdatachannel/include/rtc

INCLUDEPATH += C:/Users/Amirali/Desktop/UT/Network/Projects/lib/socket.io-client-cpp/lib/websocketpp
INCLUDEPATH += C:/Users/Amirali/Desktop/UT/Network/Projects/lib/socket.io-client-cpp/lib/asio/asio/include
INCLUDEPATH += C:/Users/Amirali/Desktop/UT/Network/Projects/lib/socket.io-client-cpp/lib/rapidjson/include

# Linking with WebRTC library and Opus library
LIBS += -LC:\Users\Amirali\Desktop\UT\Network\Projects\lib\libdatachannel/Windows/Mingw64 -ldatachannel.dll
LIBS += -LC:/Qt/Tools/OpenSSLv3/Win_x64/bin -lcrypto-3-x64 -lssl-3-x64
LIBS += -LC:/Users/Amirali/Desktop/UT/Network/Projects/lib/opus/Windows/Mingw64 -lopus
# LIBS += -LD:\SHAHZAD\00UT\sems\sem7\07-ComputerNetworks\Projects\lib\libdatachannel\Windows\Mingw64 -l

# Add additional libraries for Windows
LIBS += -lws2_32 -lssp
LIBS += -lssp

# Disable stack protection to prevent "__stack_chk_fail" error
QMAKE_CXXFLAGS += -fno-stack-protector
QMAKE_CFLAGS += -fno-stack-protector
#QMAKE_LFLAGS += -fuse-ld=lld    ## for ID error

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

QT += multimedia
