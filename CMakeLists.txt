cmake_minimum_required(VERSION 3.16)
project(RawRead VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 (or Qt5)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialPort)

set(SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

# Create executable target
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(RawRead
        MANUAL_FINALIZATION
        ${SOURCES}
    )
    qt_finalize_executable(RawRead)
else()
    add_executable(RawRead ${SOURCES})
endif()

# Link with both Widgets and SerialPort
target_link_libraries(RawRead PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::SerialPort
)

# Install settings (optional)
set_target_properties(RawRead PROPERTIES
    WIN32_EXECUTABLE TRUE
)
install(TARGETS RawRead RUNTIME DESTINATION bin)
