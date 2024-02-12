QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Components/Config/config.cpp \
    Components/Firmware/Firmware.cpp \
    Components/OpenAgreementHID/OHID/OHID_Pack.c \
    Components/OpenAgreementHID/OHID/OHID_Port.c \
    Components/OpenAgreementHID/OHID/OHID_Signature.c \
    Components/OpenAgreementHID/OHID/OHID_board_keyboard.c \
    Components/hid/InterfaceHID.cpp \
    Library/hex2bin/binary.c \
    Library/hex2bin/common.c \
    Library/hex2bin/hex2bin.c \
    Library/hex2bin/hex2bin_test.c \
    Library/hex2bin/libcrc.c \
    Library/hex2bin/mot2bin.c \
    formcfg.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindow_base.cpp \
    mainwindow_iap.cpp \

HEADERS += \
    Components/Config/config.h \
    Components/Firmware/Firmware.h \
    Components/OpenAgreementHID/OHID/OHID_Board.h \
    Components/OpenAgreementHID/OHID/OHID_Board_id.h \
    Components/OpenAgreementHID/OHID/OHID_Pack.h \
    Components/OpenAgreementHID/OHID/OHID_Port.h \
    Components/OpenAgreementHID/OHID/OHID_expanding.h \
    Components/OpenAgreementHID/OHID/OHID_type.h \
    Components/device_status.h \
    Components/hid/InterfaceHID.h \
    Library/hex2bin/binary.h \
    Library/hex2bin/common.h \
    Library/hex2bin/libcrc.h \
    formcfg.h \
    mainwindow.h \

FORMS += \
    formcfg.ui \
    mainwindow.ui

INCLUDEPATH += $$_PRO_FILE_PWD_/Components/OpenAgreementHID
INCLUDEPATH += $$_PRO_FILE_PWD_/Components
INCLUDEPATH += $$_PRO_FILE_PWD_/Library

win32: LIBS += -L$$_PRO_FILE_PWD_/Library/hidapi/win/x64  -lhidapi
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

