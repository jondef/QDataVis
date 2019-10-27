
project(QDataVis_tests)

################################
# GTest
################################
add_subdirectory(lib)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


# add google test target
include_directories(
        ${qdatavis_SOURCE_DIR}/Headers
        ${qdatavis_SOURCE_DIR}/Sources
)

file(GLOB SOURCE_FILES
        "${qdatavis_SOURCE_DIR}/Sources/*.cpp"
        "${qdatavis_SOURCE_DIR}/Sources/*.c"
        )

file(GLOB HEADER_FILES
        "${qdatavis_SOURCE_DIR}/Headers/*.h"
        "${qdatavis_SOURCE_DIR}/Headers/*.hpp"
        )

file(GLOB UI_FILES
        "${qdatavis_SOURCE_DIR}/Headers/*.ui"
        )

add_executable(Google_Tests_run BinaryTree_tests.cpp
        ${qdatavis_SOURCE_DIR}/Headers/BinaryTree.h
        ${qdatavis_SOURCE_DIR}/Sources/BinaryTree.cpp
        ${qdatavis_SOURCE_DIR}/Sources/Node.cpp
        ${qdatavis_SOURCE_DIR}/Sources/Node.cpp)

# link google test run with main app
target_link_libraries(Google_Tests_run Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
#target_link_libraries(Google_Tests_run qdatavis)

target_link_libraries(Google_Tests_run gtest gtest_main pthread)

