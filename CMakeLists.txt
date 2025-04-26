cmake_minimum_required(VERSION 3.19)
project(music_player LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Multimedia)

qt_standard_project_setup()

qt_add_executable(music_player
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    treewidgetwindow.h treewidgetwindow.cpp
    audioqueuetable.h audioqueuetable.cpp
    audiometadata.h audiometadata.cpp
    waveformprogressbar.h waveformprogressbar.cpp
    hoverbutton.h hoverbutton.cpp
    playbutton.h playbutton.cpp
    loopbutton.h loopbutton.cpp
)

file(COPY ${CMAKE_SOURCE_DIR}/styles DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/images DESTINATION ${CMAKE_BINARY_DIR})


target_link_libraries(music_player
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Multimedia
)

target_include_directories(music_player PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

include(GNUInstallDirs)

install(TARGETS music_player
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET music_player
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
