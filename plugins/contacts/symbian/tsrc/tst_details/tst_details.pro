QT += testlib
TEMPLATE=app
TARGET=tst_details
CONFIG+=testcase

include(../../../common.pri)

INCLUDEPATH += ../../../src/contacts \
               ../../../src/contacts/details \
               ../../../src/contacts/requests \
               ../../../src/contacts/filters

qtAddLibrary(QtContacts)

SOURCES  += tst_details.cpp

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}