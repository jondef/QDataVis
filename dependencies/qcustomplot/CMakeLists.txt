cmake_minimum_required(VERSION 3.15)
project(QCustomPlot)

set(CMAKE_CXX_STANDARD 17)

# need this to include the auto generated ui_mainwindow.h
# file from the qt5_wrap_ui call below.
set(CMAKE_INCLUDE_CURRENT_DIR ON) # Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON) # Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOUIC ON) # Create code from a list of Qt designer ui files
set(CMAKE_AUTORCC ON) # ^

set(CMAKE_PREFIX_PATH "C:\\Qt\\5.12.3\\mingw73_64\\lib\\cmake")
FIND_PACKAGE(Qt5 COMPONENTS Core Gui Widgets Network PrintSupport Test QuickTest REQUIRED)

add_library(QCustomPlot SHARED qcustomplot.cpp)
target_link_libraries(QCustomPlot PUBLIC Qt5::Core Qt5::PrintSupport)

# QCUSTOMPLOT STUFF
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set_target_properties(QCustomPlot PROPERTIES OUTPUT_NAME "qcustomplotd2" PREFIX "")
elseif (${CMAKE_BUILD_TYPE} STREQUAL Release)
    set_target_properties(QCustomPlot PROPERTIES OUTPUT_NAME "qcustomplot2" PREFIX "")
endif ()