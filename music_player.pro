# Basic project configuration
QT       += core gui widgets multimedia multimediawidgets
CONFIG   += c++17

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = music_player
TEMPLATE = app

# Source files
SOURCES += \
    src/modules/dirTree/searchbox.cpp \
    src/modules/main/main.cpp \
    src/modules/main/mainwindow.cpp \
    src/modules/dirTree/treewidgetwindow.cpp \
    src/modules/player/audioplayer.cpp \
    src/modules/player/hoverbutton.cpp \
    src/modules/player/loopbutton.cpp \
    src/modules/player/playbutton.cpp \
    src/modules/player/waveformprogressbar.cpp \
    src/modules/tableQueue/audiometadata.cpp \
    src/modules/tableQueue/audioqueuetable.cpp

# Header files
HEADERS += \
    src/modules/dirTree/searchbox.h \
    src/modules/main/mainwindow.h \
    src/modules/dirTree/treewidgetwindow.h \
    src/modules/player/audioplayer.h \
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
    resources/icons/icons.qrc \
    resources/styles/styles.qrc

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

    # Optimizations
    QMAKE_CXXFLAGS_RELEASE += -O2
    QMAKE_CFLAGS_RELEASE += -O2
    DEFINES += QT_NO_DEBUG_OUTPUT

    # Disable debug symbols
    CONFIG += strip
    CONFIG += no_debug_info

    # Deployment with plugin control
    win32 {
        QMAKE_POST_LINK += $$quote(windeployqt --release --no-opengl-sw $$shell_path($$DESTDIR/$${TARGET}.exe))
    }
}

# Debug build configuration
CONFIG(debug, debug|release) {
    DESTDIR = debug
    OBJECTS_DIR = debug/obj
}
