project(QCustomPlot)

set(CMAKE_INCLUDE_CURRENT_DIR ON) # Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON) # Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOUIC ON) # Create code from a list of Qt designer ui files
set(CMAKE_AUTORCC ON) # ^

add_library(QCustomPlot SHARED qcustomplot.cpp)
target_link_libraries(QCustomPlot PUBLIC Qt5::Core Qt5::PrintSupport)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    # see: https://stackoverflow.com/questions/34187770/cmake-target-link-libraries-cannot-find-lfoo-but-it-is-in-the-same-directory
    set_target_properties(QCustomPlot PROPERTIES OUTPUT_NAME "qcustomplotd2")
    #    set_target_properties(QCustomPlot PROPERTIES OUTPUT_NAME "qcustomplotd2" PREFIX "")
elseif (${CMAKE_BUILD_TYPE} STREQUAL Release)
    set_target_properties(QCustomPlot PROPERTIES OUTPUT_NAME "qcustomplot2")
endif ()