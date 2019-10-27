project(qdatavis)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/Headers
        ${CMAKE_CURRENT_SOURCE_DIR}/Sources
)

file(GLOB SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/Sources/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Sources/*.c"
        )

file(GLOB HEADER_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/Headers/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Headers/*.hpp"
        )

file(GLOB UI_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/Headers/*.ui"
        )


#SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
#SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++") # enable static linking of libstdc++
#set(CMAKE_CXX_FLAGS "-pthread") # enable multiple threads
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
set(CMAKE_VERBOSE_MAKEFILE OFF) # enable verbose output

# need this to include the auto generated ui_mainwindow.h
# file from the qt5_wrap_ui call below.
set(CMAKE_INCLUDE_CURRENT_DIR ON) # Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON) # Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOUIC ON) # Create code from a list of Qt designer ui files
set(CMAKE_AUTORCC ON) # ^


# prepare dlls to be linked to exe
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(PROJECT_LINK_LIBS qcustomplotd2)
elseif (${CMAKE_BUILD_TYPE} STREQUAL Release)
    set(PROJECT_LINK_LIBS qcustomplot2)
endif ()

link_directories(${CMAKE_BINARY_DIR}/QCustomPlot)

# wrap the ui file to a c++ header
qt5_wrap_ui(ui_wrap ${UI_FILES})

# compile exe
if (DISABLE_CMD_WINDOW)
    add_executable(QDataVis WIN32 ${SOURCE_FILES} ${HEADER_FILES} ${ui_wrap})
else ()
    add_executable(QDataVis ${SOURCE_FILES} ${HEADER_FILES} ${ui_wrap})
endif ()

# link dlls to exe
target_link_libraries(QDataVis PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
target_link_libraries(QDataVis PUBLIC ${PROJECT_LINK_LIBS})
