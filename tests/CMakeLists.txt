# ============================================================
# Unit Tests
# ============================================================

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

set(TEST_SOURCES
    BitOperatorTest.cpp
    anglesTest.cpp
)

add_executable(${PROJECT_NAME}_test ${TEST_SOURCES})

target_link_libraries(${PROJECT_NAME}_test
    PRIVATE
        ${PROJECT_NAME}::${PROJECT_NAME}
        GTest::GTest
        GTest::Main
        Threads::Threads
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)
