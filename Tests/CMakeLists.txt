project(Tests)

################################
# Pre-compilation steps
################################
find_package(Qt6 COMPONENTS Test REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
enable_testing(true)
add_definitions(-DQCUSTOMPLOT_USE_LIBRARY)

include_directories(
        ${CMAKE_SOURCE_DIR}/QDataVis/Headers
        ${CMAKE_SOURCE_DIR}/QCustomPlot/src
)

file(GLOB SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/QDataVis/Sources/BinaryTree.cpp
        ${CMAKE_SOURCE_DIR}/QDataVis/Sources/Node.cpp
)

################################
# Compiling
################################
add_executable(Tests BinaryTreeTest.cpp ${SOURCE_FILES})
add_test(Tests Tests)

################################
# Linking
################################
link_directories(${CMAKE_BINARY_DIR}/lib)

target_link_libraries(Tests PRIVATE Qt6::Test) # ! Qt6Test.dll required !
target_link_libraries(Tests PUBLIC QCustomPlot)