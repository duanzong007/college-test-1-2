QT += core gui widgets charts
QT += concurrent webenginecore
QT += webchannel webenginewidgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
LIBS += -luser32

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
    RC_ICONS = $$PWD/resources/logo.ico
}


SOURCES += \
    algo_code/data_preload.cpp \
    algo_code/flow_cydiffer.cpp \
    algo_code/flow_stationManager.cpp \
    algo_code/flow_trainManager.cpp \
    algo_code/ip_check.cpp \
    core_system.cpp \
    file_code/lineManager.cpp \
    file_code/orderManager.cpp \
    file_code/stationManager.cpp \
    file_code/trainManager.cpp \
    filemanager.cpp \
    gui/1/lineCard.cpp \
    gui/1/lineWindow.cpp \
    gui/1/mapManager.cpp \
    gui/1/p0_fileManager.cpp \
    gui/1/p0_timeManager.cpp \
    gui/1/page0_main.cpp \
    gui/1/trainCard.cpp \
    gui/1/trainWindow.cpp \
    gui/2/Page1TimeManager.cpp \
    gui/2/p1_fileManager.cpp \
    gui/2/p1_mapManager.cpp \
    gui/2/page1_main.cpp \
    gui/3/c1/DataCardWidget.cpp \
    gui/3/c1/FilterManager.cpp \
    gui/3/c1/card1.cpp \
    gui/3/c2/CYCardWidget.cpp \
    gui/3/c2/FilterManager_c2.cpp \
    gui/3/c2/card2.cpp \
    gui/3/c3/card3.cpp \
    gui/3/c3/predictor.cpp \
    gui/3/page2_main.cpp \
    gui/4/logwindow.cpp \
    gui/4/page3_main.cpp \
    gui/4/dis_page.cpp \
    gui/4/print.cpp \
    gui/4/chart_change.cpp \
    gui/4/tableedittracker.cpp \
    gui/LoadWindowLay.cpp \
    gui/sideBar.cpp \
    logger.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    algo_code/data_preload.h \
    algo_code/flow_cydiffer.h \
    algo_code/flow_stationManager.h \
    algo_code/flow_trainManager.h \
    algo_code/ip_check.h \
    core_system.h \
    file_code/lineManager.h \
    file_code/orderManager.h \
    file_code/stationManager.h \
    file_code/trainManager.h \
    filemanager.h \
    gui/1/lineCard.h \
    gui/1/lineWindow.h \
    gui/1/mapManager.h \
    gui/1/p0_fileManager.h \
    gui/1/p0_timeManager.h \
    gui/1/page0_main.h \
    gui/1/trainCard.h \
    gui/1/trainWindow.h \
    gui/2/Page1TimeManager.h \
    gui/2/p1_fileManager.h \
    gui/2/p1_mapManager.h \
    gui/2/page1_main.h \
    gui/3/c1/DataCardWidget.h \
    gui/3/c1/FilterManager.h \
    gui/3/c1/card1.h \
    gui/3/c2/CYCardWidget.h \
    gui/3/c2/FilterManager_c2.h \
    gui/3/c2/card2.h \
    gui/3/c3/card3.h \
    gui/3/c3/predictor.h \
    gui/3/page2_main.h \
    gui/4/logwindow.h \
    gui/4/page3_main.h \
    gui/4/dis_page.h \
    gui/4/print.h \
    gui/4/chart_change.h \
    gui/4/tableedittracker.h \
    gui/LoadWindowLay.h \
    gui/sideBar.h \
    logger.h \
    mainwindow.h

FORMS += \
    gui/1/lineCard.ui \
    gui/1/lineWindow.ui \
    gui/1/page0_main.ui \
    gui/1/trainCard.ui \
    gui/1/trainWindow.ui \
    gui/2/page1_main.ui \
    gui/3/c1/DataCardWidget.ui \
    gui/3/c1/card1.ui \
    gui/3/c2/CYCardWidget.ui \
    gui/3/c2/card2.ui \
    gui/3/c3/card3.ui \
    gui/3/page2_main.ui \
    gui/4/logwindow.ui \
    gui/4/page3_main.ui \
    gui/LoadWindowLay.ui \
    gui/sideBar.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/font/font.qrc \
    resources/html.qrc \
    resources/windows.qrc
