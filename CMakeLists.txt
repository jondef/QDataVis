cmake_minimum_required(VERSION 3.14) # current version 3.14.5
message("CMAKE VERSION IS " ${CMAKE_VERSION})
set(CMAKE_CXX_STANDARD 17)

project(GUI_APP)
set(DISABLE_CMD_WINDOW ON)

function(print_all_variables)
    get_cmake_property(_variableNames VARIABLES)
    list(SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach ()
endfunction()


include_directories(
        ${PROJECT_SOURCE_DIR}/Headers
        ${PROJECT_SOURCE_DIR}/Sources
)

file(GLOB SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/Headers/*.h"
        "${PROJECT_SOURCE_DIR}/Headers/*.hpp"
        "${PROJECT_SOURCE_DIR}/Sources/*.cpp"
        "${PROJECT_SOURCE_DIR}/Sources/*.c"
        )


#SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
#SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_PREFIX_PATH "C:\\Qt\\5.12.3\\mingw73_64")


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++") # enable static linking of libstdc++
#set(CMAKE_CXX_FLAGS "-pthread") # enable multiple threads
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
set(CMAKE_VERBOSE_MAKEFILE OFF) # enable verbose output

set(CMAKE_INCLUDE_CURRENT_DIR ON) # Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON) # Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOUIC ON) # Create code from a list of Qt designer ui files
set(CMAKE_AUTORCC ON) # ^^


# prepare dlls to be linked to exe
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    message("BUILD TYPE SET TO " ${CMAKE_BUILD_TYPE})
    set(PROJECT_LINK_LIBS qcustomplotd2)
elseif (${CMAKE_BUILD_TYPE} STREQUAL Release)
    message("BUILD TYPE SET TO " ${CMAKE_BUILD_TYPE})
    set(PROJECT_LINK_LIBS qcustomplot2)
endif ()


find_package(Qt5 5.12.3 EXACT REQUIRED PATHS C:/Qt/5.12.3/mingw73_64/lib/cmake COMPONENTS Core Gui Widgets REQUIRED)
set(DEPENDENCIES qcustomplot)

foreach (dependency ${DEPENDENCIES})
    link_directories(${PROJECT_SOURCE_DIR}/dependencies/${dependency}/bin)
endforeach ()


# compile exe
if (DISABLE_CMD_WINDOW)
    add_executable(Application WIN32 ${SOURCE_FILES})
else ()
    add_executable(Application ${SOURCE_FILES})
endif ()

# link dlls to exe
target_link_libraries(Application PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
target_link_libraries(Application PUBLIC ${PROJECT_LINK_LIBS})
