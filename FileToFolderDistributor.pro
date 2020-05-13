QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileToFolderDistributor
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        main_window.cpp \
        preview_frame.cpp \
        preview_image.cpp \
        preview_text.cpp \
        preview_utils.cpp

HEADERS += \
        main_window.h \
        preview_frame.h \
        preview_image.h \
        preview_text.h \
        preview_utils.h
