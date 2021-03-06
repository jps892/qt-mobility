SOURCES += tst_qnearfieldmanager.cpp
TARGET = tst_qnearfieldmanager
CONFIG += testcase

QT = core

INCLUDEPATH += ../../../src/connectivity/nfc
DEPENDPATH += ../../../src/connectivity/nfc

HEADERS += \
    qnearfieldmanagervirtualbase_p.h \
    qnearfieldmanager_emulator_p.h \
    qnearfieldtarget_emulator_p.h \
    targetemulator_p.h

SOURCES += \
    qnearfieldmanagervirtualbase.cpp \
    qnearfieldmanager_emulator.cpp \
    qnearfieldtarget_emulator.cpp \
    targetemulator.cpp


include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity

maemo* {
    testfiles.sources = ../nfcdata/*
    testfiles.path = nfcdata
    DEPLOYMENT += testfiles
    DEFINES += SRCDIR=\\\"$$MAEMO_TEST_INSTALL_ROOT/nfcdata\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD/../nfcdata\\\"
}

maemo*:CONFIG += insignificant_test
