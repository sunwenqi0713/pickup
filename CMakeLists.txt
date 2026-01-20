cmake_minimum_required(VERSION 3.15)

project(pickup
    VERSION 1.0.0
    DESCRIPTION "C++ utility library"
    LANGUAGES CXX
)

# ============================================================
# Global Settings
# ============================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for IDE/tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================
# Options
# ============================================================

option(PICKUP_BUILD_STATIC "Build static library (OFF for shared)" ON)
option(PICKUP_BUILD_TESTS "Build unit tests" OFF)
option(PICKUP_BUILD_EXAMPLES "Build examples" OFF)
option(PICKUP_ENABLE_WARNINGS "Enable compiler warnings" ON)

# ============================================================
# Sources
# ============================================================

set(PICKUP_SOURCES
    src/angles.cpp
    src/Application.cpp
    src/base64.cpp
    src/ByteBuffer.cpp
    src/CircularBuffer.cpp
    src/Component.cpp
    src/Event.cpp
    src/FileUtils.cpp
    src/hex.cpp
    src/SignalHandler.cpp
    src/StringUtils.cpp
    src/Thread.cpp
    src/ThreadPool.cpp
    src/Time.cpp
    src/Timespan.cpp
    src/url.cpp
)

# ============================================================
# Library Target
# ============================================================

if(PICKUP_BUILD_STATIC)
    add_library(${PROJECT_NAME} STATIC ${PICKUP_SOURCES})
else()
    add_library(${PROJECT_NAME} SHARED ${PICKUP_SOURCES})
endif()

# Create alias for use in subdirectories and by consumers
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Platform-specific link libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
elseif(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
endif()

# ============================================================
# Compiler Warnings
# ============================================================

if(PICKUP_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion
        )
    endif()
endif()

# ============================================================
# Subdirectories
# ============================================================

if(PICKUP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(PICKUP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ============================================================
# Install
# ============================================================

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Generate and install package config files
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)