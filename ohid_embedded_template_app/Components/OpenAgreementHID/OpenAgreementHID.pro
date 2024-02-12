QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        OHID/OHID_Pack.c \
        OHID/OHID_Port.c \
        OHID/OHID_Signature.c \
        OHID/OHID_board_keyboard.c \
        OHID_Pack_Test.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    OHID/OHID_Board.h \
    OHID/OHID_Board_id.h \
    OHID/OHID_Pack.h \
    OHID/OHID_Port.h \
    OHID/OHID_expanding.h \
    OHID/OHID_type.h \
    OHID_Pack_Test.h
