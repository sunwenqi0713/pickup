######################################################
# TESTS

set(PICKUP_TESTS
    BitOperatorTest.cpp
    anglesTest.cpp
)

set(TEST_DEPENDECIES
    ${PICKUP_LIBRARY}
    pickup)

enable_testing()

find_package(GTest REQUIRED)

add_executable(${PICKUP_LIBRARY}_test ${PICKUP_TESTS})

target_link_libraries(${PROJECT_NAME}_test
    ${TEST_DEPENDECIES}
    ${GTEST_LIBRARIES}
    Threads::Threads)
