# https://google.github.io/googletest/quickstart-cmake.html
cmake_minimum_required(VERSION 3.14)
project(Flexheeple_Unit_Tests)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723 # Release v1.17.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(fsv_unit_test fsv_unit_test.cpp)
target_link_libraries(
    fsv_unit_test
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(fsv_unit_test)
