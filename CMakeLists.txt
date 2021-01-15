cmake_minimum_required(VERSION 3.15)
project(QDataVis)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE OFF)

message("CMAKE VERSION IS " ${CMAKE_VERSION})
message("BUILD TYPE SET TO " ${CMAKE_BUILD_TYPE})

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic")
endif ()

function(print_all_variables)
    get_cmake_property(_variableNames VARIABLES)
    list(SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach ()
endfunction()

set(EXECUTABLE_OUTPUT_PATH "bin" CACHE STRING "Path to place executables relative to ${CMAKE_INSTALL_PREFIX}")
set(LIBRARY_OUTPUT_PATH "bin" CACHE STRING "Path to place libraries relative to ${CMAKE_INSTALL_PREFIX}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

################################
# Qt
################################
FIND_PACKAGE(Qt5 COMPONENTS Core Gui Widgets PrintSupport REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON) # Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON) # Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOUIC ON) # Create code from a list of Qt designer ui files
set(CMAKE_AUTORCC ON) # Create Resource files

################################
# QCustomPlot
################################
include_directories(QCustomPlot/src)
add_subdirectory(QCustomPlot)

################################
# QDataVis
################################
include_directories(QDataVis/Headers)
add_subdirectory(QDataVis)
add_subdirectory(Tests)