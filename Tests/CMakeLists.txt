project(Tests)
enable_testing()

################################
# Pre-compilation steps
################################
include_directories(
        ${CMAKE_SOURCE_DIR}/QDataVis/Headers
)

file(GLOB SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/QDataVis/Sources/BinaryTree.cpp
        ${CMAKE_SOURCE_DIR}/QDataVis/Sources/Node.cpp
        )

find_package(Qt6 COMPONENTS Test REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

################################
# Compiling
################################
add_executable(Tests BinaryTreeTest.cpp ${SOURCE_FILES})
add_test(Tests Tests)

################################
# Linking
################################
target_link_libraries(Tests Qt6::Test)