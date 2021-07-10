project(QDataVis)

################################
# Versioning
################################
# set version number
set(VERSION 1.0)
# get current date time
string(TIMESTAMP BUILDDATE "%Y-%m-%d %H:%M:%S" UTC)
# Get git commit date
execute_process(
        COMMAND git --git-dir ${CMAKE_SOURCE_DIR}/.git --work-tree ${CMAKE_SOURCE_DIR} show -s --format=%ci HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Get git commit hash
execute_process(
        COMMAND git --git-dir ${CMAKE_SOURCE_DIR}/.git --work-tree ${CMAKE_SOURCE_DIR} rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
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
# Compilation
################################
qt6_wrap_ui(ui_wrap ${UI_FILES})

if (WIN32 AND ${CMAKE_BUILD_TYPE} MATCHES Release)  # if on windows and release, disable cmd prompt
    add_executable(QDataVis WIN32 ${SOURCE_FILES} ${HEADER_FILES} ${ui_wrap} ${QRC_FILES})
else ()
    add_executable(QDataVis ${SOURCE_FILES} ${HEADER_FILES} ${ui_wrap} ${QRC_FILES})
endif ()

################################
# Linking
################################
link_directories(${CMAKE_BINARY_DIR}/lib)

target_link_libraries(QDataVis PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets)
target_link_libraries(QDataVis PUBLIC QCustomPlot)

################################
# Post-build
################################
if (WIN32 AND CMAKE_PREFIX_PATH)
    set(DEBUG_SUFFIX)
    set(PROJECT_NAME QDataVis)
    # find the qt install path
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    # copy the /plugins/platforms/qwindows dll if it exists
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    # copy the remaining required dlls
    foreach (QT_LIB ${REQUIRED_LIBS})
        add_custom_command(TARGET QDataVis POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt${QT_VERSION}${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()