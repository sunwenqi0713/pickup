cmake_minimum_required(VERSION 3.15)

project (pickup)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# create compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#---- project configuration ----
option(PICKUP_STATIC_LIBS "Build shared libraries" ON)
option(PICKUP_ENABLE_TESTS "Build the tests" OFF)
option(PICKUP_EXAMPLES "Build tutorials and examples" ON)

if (PICKUP_ENABLE_TESTS)
    find_package(GTest)
endif()

set(PICKUP_LIBRARY ${PROJECT_NAME})

include_directories(
 include
)

list(APPEND PROJECT_SOURCES
    src/angles.cpp
    src/StringUtils.cpp
)

if (PICKUP_STATIC_LIBS)
    add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCES})
else()
    add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
endif()

if(PICKUP_EXAMPLES)
    add_subdirectory(examples)
endif()