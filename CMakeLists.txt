cmake_minimum_required(VERSION 3.15)

project (pickup)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# create compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#---- project configuration ----
option(PICKUP_STATIC_LIBS "Build shared libraries" ON)
option(PICKUP_UNIT_TESTS "Build the unit tests" OFF)
option(PICKUP_EXAMPLES "Build tutorials and examples" OFF)

set(PICKUP_LIBRARY ${PROJECT_NAME})

include_directories(
 include
)

list(APPEND PROJECT_SOURCES
    src/angles.cpp
    src/StringUtils.cpp
    src/Thread.cpp
    src/ThreadPool.cpp
    src/Event.cpp
    src/Application.cpp
    src/Result.cpp
    src/hex.cpp
    src/Timestamp.cpp
    src/Timespan.cpp
    src/DateTime.cpp
)

if (PICKUP_STATIC_LIBS)
    add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
else()
    add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCES})
endif()

######################################################

if (PICKUP_UNIT_TESTS)
    add_subdirectory(tests)
endif()

if(PICKUP_EXAMPLES)
    add_subdirectory(examples)
endif()

######################################################
# INSTALL