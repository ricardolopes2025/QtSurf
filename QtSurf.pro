QT += core gui widgets webenginewidgets webchannel positioning svg

CONFIG += c++17

SOURCES += \
    main.cpp \
    qtsurfwindow.cpp \
    tabwebview.cpp \
    settingsdialog.cpp \
    settings.cpp \
    downloadmanager.cpp \
    downloadwidget.cpp


HEADERS += \
    qtsurfwindow.h \
    tabwebview.h \
    settingsdialog.h \
    settings.h \
    downloadmanager.h \
    downloadwidget.h

FORMS += \
    qtsurfwindow.ui

RESOURCES += \
   QtSurf.qrc

# NOVO: Arquivo de recurso Windows para embutir o ícone
win32:RC_FILE += QtSurf.rc

TRANSLATIONS += \
    QtSurf_pt_BR.ts

CONFIG += lrelease
CONFIG += embed_translations

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
