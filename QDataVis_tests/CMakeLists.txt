project(QDataVis_tests)
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

FIND_PACKAGE(Qt5 COMPONENTS Core REQUIRED)

# add google test target
add_executable(Google_Tests_run function_check.cpp)

# link google test run with main app
target_link_libraries(Google_Tests_run qdatavis)

target_link_libraries(Google_Tests_run gtest gtest_main)