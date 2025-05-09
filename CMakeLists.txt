cmake_minimum_required(VERSION 3.19)
project(music_player LANGUAGES CXX)

# Find Qt packages
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Multimedia)

# Initialize Qt project
qt_standard_project_setup()

# Enable automatic moc, uic, and rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Create executable
qt_add_executable(music_player
    WIN32 MACOSX_BUNDLE
    src/modules/main/main.cpp
    src/modules/main/mainwindow.cpp
    src/modules/main/mainwindow.h
    src/modules/main/mainwindow.ui
    
    src/modules/dirTree/treewidgetwindow.cpp
    src/modules/dirTree/treewidgetwindow.h
    
    src/modules/tableQueue/audioqueuetable.cpp
    src/modules/tableQueue/audioqueuetable.h
    src/modules/tableQueue/audiometadata.cpp
    src/modules/tableQueue/audiometadata.h
    
    src/modules/player/hoverbutton.cpp
    src/modules/player/hoverbutton.h
    src/modules/player/loopbutton.cpp
    src/modules/player/loopbutton.h
    src/modules/player/playbutton.cpp
    src/modules/player/playbutton.h
    src/modules/player/waveformprogressbar.cpp
    src/modules/player/waveformprogressbar.h
)

# Add resource files (if you have .qrc files)
 qt_add_resources(music_player "resources"
     PREFIX "/"
     FILES
         resources/styles/style.qrc
         resources/icons/icons.qrc
 )

# Copy resources to build directory
file(COPY ${CMAKE_SOURCE_DIR}/resources/styles DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/resources/icons DESTINATION ${CMAKE_BINARY_DIR})

# Link libraries
target_link_libraries(music_player PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
)

# Include directories
target_include_directories(music_player PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Installation
include(GNUInstallDirs)
install(TARGETS music_player
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Deploy script
qt_generate_deploy_app_script(
    TARGET music_player
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
