QT       += core gui multimedia quick
CONFIG += c++17
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
SOURCES += \
    audioinput.cpp \
    audiooutput.cpp \
    main.cpp \
    webrtc.cpp
HEADERS += \
    audioinput.h \
    audiooutput.h \
    webrtc.h
# Adding include paths for WebRTC and Opus
INCLUDEPATH += C:/Users/Amirali/Desktop/UT/Network/Projects/lib/libdatachannel
INCLUDEPATH += C:/Users/Amirali/Desktop/UT/Network/Projects/lib/opus/include
INCLUDEPATH += C:/Users/Amirali/Desktop/UT/Network/Projects/lib/libdatachannel/include/rtc
# Linking with WebRTC library and Opus library
LIBS += -LC:\Users\Amirali\Desktop\UT\Network\Projects\lib\libdatachannel/Windows/Mingw64 -ldatachannel.dll
LIBS += -LC:/Qt/Tools/OpenSSLv3/Win_x64/bin -lcrypto-3-x64 -lssl-3-x64
LIBS += -LC:/Users/Amirali/Desktop/UT/Network/Projects/lib/opus/Windows/Mingw64 -lopus
# Add additional libraries for Windows
LIBS += -lws2_32 -lssp
LIBS += -lssp

# Disable stack protection to prevent "__stack_chk_fail" error
QMAKE_CXXFLAGS += -fno-stack-protector
QMAKE_CFLAGS += -fno-stack-protector
QMAKE_LFLAGS += -fuse-ld=lld
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



RESOURCES += \
    resources.qrc

QT += multimedia
