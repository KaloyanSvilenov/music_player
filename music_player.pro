# Basic project configuration
QT       += core gui widgets multimedia multimediawidgets
CONFIG   += c++17

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = music_player
TEMPLATE = app

# Source files
SOURCES += \
    src/modules/main/main.cpp \
    src/modules/main/mainwindow.cpp \
    src/modules/dirTree/treewidgetwindow.cpp \
    src/modules/player/hoverbutton.cpp \
    src/modules/player/loopbutton.cpp \
    src/modules/player/playbutton.cpp \
    src/modules/player/waveformprogressbar.cpp \
    src/modules/tableQueue/audiometadata.cpp \
    src/modules/tableQueue/audioqueuetable.cpp

# Header files
HEADERS += \
    src/modules/main/mainwindow.h \
    src/modules/dirTree/treewidgetwindow.h \
    src/modules/player/hoverbutton.h \
    src/modules/player/loopbutton.h \
    src/modules/player/playbutton.h \
    src/modules/player/waveformprogressbar.h \
    src/modules/tableQueue/audiometadata.h \
    src/modules/tableQueue/audioqueuetable.h

# UI files
FORMS += \
    src/modules/main/mainwindow.ui

# Resource files
RESOURCES += \
    resources/icons/back_hover.png \
    resources/icons/back.png \
    resources/icons/filter_hover.png \
    resources/icons/filter.png \
    resources/icons/loop_hover.png \
    resources/icons/loop.png \
    resources/icons/next_hover.png \
    resources/icons/next.png \
    resources/icons/pause_hover.png \
    resources/icons/pause.png \
    resources/icons/play_hover.png \
    resources/icons/play.png \
    resources/icons/shuffle_hover.png \
    resources/icons/shuffle.png \
    resources/icons/volume_hover.png \
    resources/icons/volume.png \
    resources/styles/darktheme.qss

# Additional configuration
INCLUDEPATH += src/modules

# Build directory
DESTDIR = build
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

# Release build configuration
CONFIG(release, debug|release) {
    DESTDIR = release
    OBJECTS_DIR = release/obj
}

# Debug build configuration
CONFIG(debug, debug|release) {
    DESTDIR = debug
    OBJECTS_DIR = debug/obj
}
