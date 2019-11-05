cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)

project(QDataVis)
set(DISABLE_CMD_WINDOW OFF)

message("CMAKE VERSION IS " ${CMAKE_VERSION})
message("BUILD TYPE SET TO " ${CMAKE_BUILD_TYPE})

function(print_all_variables)
    get_cmake_property(_variableNames VARIABLES)
    list(SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach ()
endfunction()

# ! Qt stuff
SET(QT_USE_QTNETWORK true)
set(CMAKE_PREFIX_PATH "C:\\Qt\\5.12.5\\mingw73_64\\lib\\cmake")
FIND_PACKAGE(Qt5 COMPONENTS Core Gui Widgets Network PrintSupport Test QuickTest REQUIRED)

# QCustomPlot stuff
include_directories(exprtk)
include_directories(QCustomPlot)
include_directories(QDataVis/Headers)

# add subdirectories
add_subdirectory(QCustomPlot)
add_subdirectory(QDataVis)
add_subdirectory(QDataVis_tests)
