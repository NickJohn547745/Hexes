QT       += core gui testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    blockvalue.cpp \
    deleteruledialog.cpp \
    hexviewer.cpp \
    main.cpp \
    mainwindow.cpp \
    rule.cpp \
    ruledialog.cpp \
    rulepreview.cpp \
    testhexviewer.cpp

HEADERS += \
    blockvalue.h \
    deleteruledialog.h \
    hexviewer.h \
    mainwindow.h \
    rule.h \
    ruledialog.h \
    rulepreview.h \
    testhexviewer.h

FORMS += \
    deleteruledialog.ui \
    mainwindow.ui \
    ruledialog.ui \
    rulepreview.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
