# ============================================================
# Unit Tests
# ============================================================

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
    GIT_SHALLOW    TRUE
)

# 防止 GTest 覆盖父项目的编译器/链接器设置（Windows 必须）
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

find_package(Threads REQUIRED)

set(TEST_SOURCES
    BitOperatorTest.cpp
    anglesTest.cpp
)

add_executable(${PROJECT_NAME}_test ${TEST_SOURCES})

target_link_libraries(${PROJECT_NAME}_test
    PRIVATE
        ${PROJECT_NAME}::${PROJECT_NAME}
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)
