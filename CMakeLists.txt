cmake_minimum_required(VERSION 3.17)
message(${CMAKE_CURRENT_SOURCE_DIR})
project(QDataVis)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE OFF)
message(${CMAKE_CURRENT_SOURCE_DIR})
message("CMAKE VERSION IS " ${CMAKE_VERSION})
message("BUILD TYPE SET TO " ${CMAKE_BUILD_TYPE})
if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()
message(${CMAKE_CURRENT_SOURCE_DIR})
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic")
endif ()
message(${CMAKE_CURRENT_SOURCE_DIR})
function(print_all_variables)
    get_cmake_property(_variableNames VARIABLES)
    list(SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach ()
endfunction()
message(${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH "bin" CACHE STRING "Path to place executables relative to ${CMAKE_INSTALL_PREFIX}")
set(LIBRARY_OUTPUT_PATH "bin" CACHE STRING "Path to place libraries relative to ${CMAKE_INSTALL_PREFIX}")
message(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
message(${CMAKE_CURRENT_SOURCE_DIR})
################################
# Qt
################################
set(QT_VERSION 6)
set(REQUIRED_LIBS Core Gui Widgets OpenGL PrintSupport)
message(${CMAKE_CURRENT_SOURCE_DIR})
find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
message(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_INCLUDE_CURRENT_DIR ON) # Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON) # Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOUIC ON) # Create code from a list of Qt designer ui files
set(CMAKE_AUTORCC ON) # Create Resource files
message(${CMAKE_CURRENT_SOURCE_DIR})
################################
# QCustomPlot
################################
include_directories(QCustomPlot/src)
message(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/QCustomPlot)
message(${CMAKE_CURRENT_SOURCE_DIR})

################################
# QDataVis
################################
include_directories(QDataVis/Headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/QDataVis)
add_subdirectory(Tests)