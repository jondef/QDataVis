project(QDataVis)

################################
# Versioning
################################
# set version number
set(VERSION 0.1)
# get current date time
if (WIN32)
    execute_process(
            COMMAND date -R # date /t
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE BUILDDATE
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
elseif (UNIX)
    execute_process(
            COMMAND date -R
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE BUILDDATE
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
elseif (APPLE)
    execute_process(
            COMMAND date -R
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE BUILDDATE
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif ()
# Get git commit hash
execute_process(
        COMMAND git --git-dir ${CMAKE_SOURCE_DIR}/.git --work-tree ${CMAKE_SOURCE_DIR} rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Get git commit date
execute_process(
        COMMAND git --git-dir ${CMAKE_SOURCE_DIR}/.git --work-tree ${CMAKE_SOURCE_DIR} show -s --format=%ci HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Get git branch
execute_process(
        COMMAND git --git-dir ${CMAKE_SOURCE_DIR}/.git --work-tree ${CMAKE_SOURCE_DIR} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_definitions(-DVERSION="${VERSION}")
add_definitions(-DBUILDDATE="${BUILDDATE}")
add_definitions(-DGIT_COMMIT_HASH="${GIT_COMMIT_HASH}")
add_definitions(-DGIT_COMMIT_DATE="${GIT_COMMIT_DATE}")
add_definitions(-DGIT_BRANCH="${GIT_BRANCH}")
add_definitions(-DQCUSTOMPLOT_USE_LIBRARY)

################################
# Pre-compilation steps
################################
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/Headers
)

file(GLOB SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Sources/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Sources/*.c
        )

file(GLOB HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Headers/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Headers/*.hpp
        )

file(GLOB UI_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Headers/*.ui
        )

file(GLOB QRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/*.qrc
        )

################################
# Compiling
################################
qt5_wrap_ui(ui_wrap ${UI_FILES})

if (WIN32 AND ${CMAKE_BUILD_TYPE} STREQUAL Release)  # if on windows and release, disable cmd prompt
    add_executable(QDataVis WIN32 ${SOURCE_FILES} ${HEADER_FILES} ${ui_wrap} ${QRC_FILES})
else ()
    add_executable(QDataVis ${SOURCE_FILES} ${HEADER_FILES} ${ui_wrap} ${QRC_FILES})
endif ()

################################
# Linking
################################
link_directories(${CMAKE_BINARY_DIR}/lib)

target_link_libraries(QDataVis PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
target_link_libraries(QDataVis PUBLIC QCustomPlot)
