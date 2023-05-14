cmake_minimum_required(VERSION 3.5)

project (pickup)

set(CMAKE_CXX_STANDARD 14)

include_directories(
 src
)

file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/src "*.cpp")
add_library(${PROJECT_NAME} STATIC
    ${PROJECT_SOURCES}
)