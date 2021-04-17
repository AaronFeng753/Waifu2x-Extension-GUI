#    Copyright (C) 2021  Aaron Feng

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Affero General Public License as
#    published by the Free Software Foundation, either version 3 of the
#    License, or (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Affero General Public License for more details.

#    You should have received a copy of the GNU Affero General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#    My Github homepage: https://github.com/AaronFeng753

QT       += core gui
QT       += concurrent
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Anime4K.cpp \
    CompatibilityTest.cpp \
    Current_File_Progress.cpp \
    CustomResolution.cpp \
    Donate.cpp \
    Finish_Action.cpp \
    Frame_Interpolation.cpp \
    Right-click_Menu.cpp \
    SystemTrayIcon.cpp \
    Web_Activities.cpp \
    checkupdate.cpp \
    files.cpp \
    gif.cpp \
    image.cpp \
    main.cpp \
    mainwindow.cpp \
    progressBar.cpp \
    realsr_ncnn_vulkan.cpp \
    settings.cpp \
    srmd-cuda.cpp \
    srmd_ncnn_vulkan.cpp \
    table.cpp \
    textBrowser.cpp \
    topsupporterslist.cpp \
    video.cpp \
    waifu2x.cpp \
    waifu2x_caffe.cpp \
    waifu2x_converter.cpp \
    waifu2x_ncnn_vulkan.cpp

HEADERS += \
    mainwindow.h \
    topsupporterslist.h

FORMS += \
    mainwindow.ui \
    topsupporterslist.ui

TRANSLATIONS += language_English.ts \
               language_Chinese.ts \
               language_TraditionalChinese.ts

TARGET = Waifu2x-Extension-GUI

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    OtherPic.qrc \
    donate.qrc \
    icon.qrc

RC_ICONS =icon/icon.ico
